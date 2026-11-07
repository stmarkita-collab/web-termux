#!/data/data/com.termux/files/usr/bin/bash
echo "[*] Memperbarui paket Termux..."
pkg update -y && pkg install -y clang python git curl

echo "[*] Menginstal dependensi Python..."
pip install flask flask-socketio

echo "[*] Mengunduh pustaka Xterm.js & Socket.IO secara lokal..."
mkdir -p static
curl -sL "https://jsdelivr.net" -o https://github.com/stmarkita-collab/web-termux/tree/main/static/xterm.css
curl -sL "https://jsdelivr.net" -o https://github.com/stmarkita-collab/web-termux/tree/main/static/xterm.js
curl -sL "https://jsdelivr.net" -o https://github.com/stmarkita-collab/web-termux/tree/main/static/xterm-addon-fit.js
curl -sL "https://socket.io" -o https://github.com/stmarkita-collab/web-termux/tree/main/static/socket.io.min.js

echo "[*] Mengompilasi mesin PTY C..."
clang pty_engine.c -o pty_engine

echo "[+] Selesai! Jalankan server dengan: python server.py"
