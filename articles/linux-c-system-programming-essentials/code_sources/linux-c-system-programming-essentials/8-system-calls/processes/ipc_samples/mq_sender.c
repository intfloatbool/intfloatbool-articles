#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    struct mq_attr msg_attr = { 0 };
    msg_attr.mq_maxmsg = 10;
    msg_attr.mq_msgsize = 2048;

    if ( argc != 2 )
    {
        fprintf(stderr, "message argument required.\n");
        exit(1);
    }

    int mq_d = mq_open("/my_queue", O_CREAT | O_RDWR, 0644, &msg_attr);

    if ( mq_d == -1 )
    {
        perror("mq_open() failed.");
        exit(1);
    }

    int mq_send_res = mq_send( mq_d, argv[1], strlen(argv[1]), 1 );

    if( mq_send_res == -1 )
    {
        perror("mq_send() failed.");
        exit(1);
    }

    mq_close( mq_d );

    exit(0);
}
