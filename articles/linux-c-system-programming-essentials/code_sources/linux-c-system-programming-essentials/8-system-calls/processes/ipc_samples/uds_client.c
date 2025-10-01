#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

int main(void)
{
    const int max_len = 128 * 4;
    const char* sock_name = "/tmp/my_socket";

    char send_buffer[max_len];
    char recv_buffer[max_len];

    int sock_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);

    if( sock_fd == -1 )
    {
        perror("socket() failed. ");
        exit(1);
    }

    struct sockaddr_un sock_addr = { 0 };

    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, sock_name);

    int conn_res = connect(
        sock_fd,
        (const struct sockaddr*) &sock_addr,
        sizeof(struct sockaddr_un)
    );

    if( conn_res == -1 )
    {
        perror("connect() failed. ");
        exit(1);
    }

    while(1)
    {
        printf("enter message: ");
        fgets( send_buffer, sizeof(send_buffer), stdin );

        send_buffer[strcspn(send_buffer, "\n")] = '\0';

        int write_res = write(sock_fd, send_buffer, strlen(send_buffer) + 1);
        if( write_res == -1 )
        {
            perror("write() failed. ");
            break;
        }

        int read_res = read(sock_fd, recv_buffer, max_len);
        if( read_res == -1 )
        {
            perror("read() failed. ");
            exit(1);
        }

        printf("%s\n", recv_buffer);
    }

    exit(0);

}
