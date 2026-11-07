import os
import subprocess
from flask import Flask, render_template, request
from flask_socketio import SocketIO

app = Flask(__name__)
# Menggunakan async_mode='threading' agar kompatibel dengan subprocess standar
socketio = SocketIO(app, cors_allowed_origins="*", async_mode='threading')

# Menyimpan proses PTY per sesi klien
sessions = {}

def read_pty_output(sid, proc):
    """Membaca output dari mesin C secara terus-menerus dan mengirimnya ke browser."""
    try:
        while proc.poll() is None:
            data = os.read(proc.stdout.fileno(), 4096)
            if not data:
                break
            socketio.emit('pty_output', data.decode('utf-8', errors='replace'), room=sid)
    except Exception:
        pass
    finally:
        socketio.emit('pty_output', '\r\n[Sesi Terminal Berakhir]\r\n', room=sid)

@app.route('/')
def index():
    return render_template('index.html')

@socketio.on('connect')
def handle_connect():
    sid = request.sid
    # Menjalankan mesin C dalam mode unbuffered (bufsize=0)
    proc = subprocess.Popen(
        ['./pty_engine'],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        bufsize=0
    )
    sessions[sid] = proc
    socketio.start_background_task(read_pty_output, sid, proc)

@socketio.on('pty_input')
def handle_input(data):
    """Menerima ketikan tombol dari browser dan mengirimkannya ke mesin C."""
    sid = request.sid
    proc = sessions.get(sid)
    if proc and proc.poll() is None:
        os.write(proc.stdin.fileno(), data.encode('utf-8'))

@socketio.on('disconnect')
def handle_disconnect():
    sid = request.sid
    proc = sessions.pop(sid, None)
    if proc:
        proc.terminate()

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000, debug=False)
