#!/data/data/com.termux/files/usr/bin/bash
echo "[*] Memperbarui paket Termux dan menginstal compiler C & Python..."
pkg update -y && pkg install -y clang python git

echo "[*] Menginstal dependensi Python..."
pip install -r requirements.txt

echo "[*] Mengompilasi mesin PTY C..."
clang pty_engine.c -o pty_engine

echo "[+] Selesai! Jalankan server dengan perintah: python server.py"
