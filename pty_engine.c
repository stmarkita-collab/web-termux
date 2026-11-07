#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pty.h>
#include <sys/select.h>
#include <signal.h>

int main() {
    int master_fd;
    pid_t pid = forkpty(&master_fd, NULL, NULL, NULL);

    if (pid < 0) {
        perror("forkpty gagal");
        return 1;
    }

    if (pid == 0) {
        // Mengatur environment variable agar warna dan aplikasi TUI berjalan normal
        setenv("TERM", "xterm-256color", 1);
        
        // Path shell khusus lingkungan Termux Android
        const char *termux_bash = "/data/data/com.termux/files/usr/bin/bash";
        execlp(termux_bash, "bash", "-i", NULL);
        
        // Fallback jika dijalankan di Linux desktop biasa
        execlp("/bin/bash", "bash", "-i", NULL);
        perror("Gagal menjalankan shell");
        exit(1);
    }

    fd_set fds;
char buf[4096];

    while (1) {
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        FD_SET(master_fd, &fds);

        int max_fd = (master_fd > STDIN_FILENO) ? master_fd : STDIN_FILENO;

        if (select(max_fd + 1, &fds, NULL, NULL, NULL) < 0) {
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &fds)) {
            ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
            if (n <= 0) break;
            write(master_fd, buf, n);
        }

        if (FD_ISSET(master_fd, &fds)) {
            ssize_t n = read(master_fd, buf, sizeof(buf));
            if (n <= 0) break;
            write(STDOUT_FILENO, buf, n);
            fflush(stdout);
        }
    }

    kill(pid, SIGTERM);
    return 0;
}
