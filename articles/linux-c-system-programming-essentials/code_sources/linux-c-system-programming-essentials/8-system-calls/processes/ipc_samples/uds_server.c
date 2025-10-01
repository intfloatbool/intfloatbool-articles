#include <stdio.h>
#include <sys/un.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>


void on_signal(int signum);
void dispose();

const char* sock_name = "/tmp/my_socket";
int sock_fd = -1;
int data_fd = -1;

int main(void)
{

    struct sigaction sig_action = { 0 };

    sig_action.sa_handler = on_signal;
    sigfillset(&sig_action.sa_mask);
    sig_action.sa_flags = SA_RESTART;

    sigaction(SIGTERM, &sig_action, NULL);
    sigaction(SIGINT, &sig_action, NULL);
    sigaction(SIGQUIT, &sig_action, NULL);
    sigaction(SIGABRT, &sig_action, NULL);
    sigaction(SIGPIPE, &sig_action, NULL);

    sock_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);

    if( sock_fd == -1 )
    {
        perror("socket() failed. ");
        exit(1);
    }

    struct sockaddr_un sock_addr = { 0 };

    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, sock_name);

    int bind_res = bind(
        sock_fd,
        (const struct sockaddr*) &sock_addr,
        sizeof(struct sockaddr_un)
    );

    if( bind_res == -1 )
    {
        perror("bind() failed. ");
        exit(1);
    }

    int listen_res = listen(sock_fd, 20);

    if( listen_res == -1 )
    {
        perror("listen() failed. ");
        exit(1);
    }

    data_fd = accept(sock_fd, NULL, NULL);

    if( data_fd == -1 )
    {
        perror("accept() failed. ");
        exit(1);
    }

    printf("> > client connected.\n");

    const int max_len = 128 * 4;
    char buffer[max_len];
    int read_res = -1;

    while(1)
    {
        while(1)
        {
            read_res = read(data_fd, buffer, max_len );

            if( read_res == -1 )
            {
                perror("read() failed. ");
                dispose();
            }
            else if( read_res == 0 )
            {
                printf("> > client disconnected.\n");
                dispose();
            }
            else
            {
                printf("> > > message from client:\n\t%s\n", buffer);
                break;
            }
        }
        char* back_msg = "message received";
        write(data_fd, back_msg, strlen(back_msg));
    }

    exit(0);
}

void on_signal(int signum)
{
    dispose();
}

void dispose()
{
    close(sock_fd);
    close(data_fd);
    unlink(sock_name);
    exit(0);
}
