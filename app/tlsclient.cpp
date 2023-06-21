//
// Created by csepsk on 2022/5/13.
//
#include <termios.h>
#include "src/Client.h"

void SetStdinEcho(bool enable = true) {
    struct termios tty{};
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable)
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

using namespace std;

int main(int argc, char *argv[]) {
    if (argc == 2) {
        Client tlsClient;
        tlsClient.Init(argv[1], PORT);
        printf("用户名:");
        char username[64];
        scanf("%63s", username);
        printf("密码:");
        SetStdinEcho(false);
        char password[64];
        scanf("%63s", password);
        SetStdinEcho(true);
        if (tlsClient.Verify((const char *) username, (const char *) password)) {
            tlsClient.Listen();
        }

    }
    return 0;
}