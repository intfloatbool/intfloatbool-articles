#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
        mqd_t mq_d = mq_open("/my_queue", O_RDONLY);

        if( mq_d == -1 )
        {
            perror("mq_open() failed.");
            exit(1);
        }

        struct mq_attr msg_attr = { 0 };
        int mq_get_attr_res = mq_getattr( mq_d, &msg_attr );

        if( mq_get_attr_res == -1 )
        {
            perror("mq_getattr() failed.");
            exit(1);
        }

        long msg_size = msg_attr.mq_msgsize;

        char* buffer = calloc(msg_size, sizeof(char));

        if( buffer == NULL )
        {
            fprintf( stderr, "calloc() failed.\n" );
            exit(1);
        }

        long queue_count = msg_attr.mq_curmsgs;

        printf("current messages count in queue: %ld\n", queue_count);

        for( int i = 0; i < queue_count; i++ )
        {
            ssize_t receive_res = mq_receive(mq_d, buffer, msg_size, NULL);

            if(receive_res == -1)
            {
                perror("mq_receive() failed.");
                exit(1);
            }

            printf("%s\n", buffer );

            memset( buffer, '\0',  msg_size);
        }

        /* dispose */
        free(buffer);
        mq_close(mq_d);
        mq_unlink("/my_queue");

        exit(0);
}
