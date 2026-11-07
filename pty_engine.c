#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pty.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <signal.h>

int main() {
    int master_fd;
    pid_t pid = forkpty(&master_fd, NULL, NULL, NULL);

    if (pid < 0) {
        perror("forkpty gagal");
        return 1;
    }

    if (pid == 0) {
        // Proses Anak: Menjalankan shell interaktif (bash)
        setenv("TERM", "xterm-256color", 1);
        execlp("/bin/bash", "bash", "-i", NULL);
        perror("execlp gagal");
        exit(1);
    }

    // Proses Induk: Meneruskan data antara stdin/stdout (Python) dan master_fd (PTY)
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

        // Baca ketikan dari Python (stdin) -> kirim ke shell (master_fd)
        if (FD_ISSET(STDIN_FILENO, &fds)) {
            ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
            if (n <= 0) break;
            write(master_fd, buf, n);
        }

        // Baca output layar dari shell (master_fd) -> kirim ke Python (stdout)
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
