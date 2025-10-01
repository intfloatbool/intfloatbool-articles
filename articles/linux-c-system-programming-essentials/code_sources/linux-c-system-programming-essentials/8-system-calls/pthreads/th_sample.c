#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void* is_power_of_two(void* arg);
void* progress(void* arg);

typedef struct {
    unsigned int delay;
    long long int number;
} th_data;

int main(int argc, char** argv)
{

    if( argc != 3 )
    {
        fprintf(stderr, "two numbers required.\n");
        exit(1);
    }

    pthread_t tid_is_power_of_two_1;
    pthread_t tid_is_power_of_two_2;

    pthread_t tid_progress;

    pthread_attr_t thread_attr = {0};

    pthread_attr_init(&thread_attr);

    pthread_create(&tid_progress, &thread_attr, progress, NULL);
    pthread_detach(tid_progress);

    th_data data_thread_1 = { 0 };
    data_thread_1.delay = 5;
    data_thread_1.number = atoll(argv[1]);

    th_data data_thread_2 = { 0 };
    data_thread_2.delay = 10;
    data_thread_2.number = atoll(argv[2]);

    pthread_create(&tid_is_power_of_two_1, &thread_attr, is_power_of_two, &data_thread_1);
    pthread_create(&tid_is_power_of_two_2, &thread_attr, is_power_of_two, &data_thread_2);


    pthread_join(tid_is_power_of_two_1, NULL);
    pthread_join(tid_is_power_of_two_2, NULL);

    pthread_attr_destroy(&thread_attr);

    int cancel_result = pthread_cancel(tid_progress);
    if ( cancel_result != 0 )
    {
        fprintf(stderr, "cancel failed().\n");
        exit(1);
    }

    printf("work done.\n");

    exit(0);
}

void* is_power_of_two(void* data)
{
    th_data* input = (th_data*) data;

    unsigned int seconds = input->delay;
    sleep(seconds);
    long long int number = input->number;

    if(number % 2 == 0)
    {
        printf("number %d is power of two.\n", number);
        return NULL;
    }
    else
    {
         printf("number %d is NOT power of two.\n", number);
         return NULL;
    }
}

void* progress(void* arg)
{
    while(1)
    {
        sleep(1);
        printf(".");
        fflush(stdout);
    }

    return NULL;
}
