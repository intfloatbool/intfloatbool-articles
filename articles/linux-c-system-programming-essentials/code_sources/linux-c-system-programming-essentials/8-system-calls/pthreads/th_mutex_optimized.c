#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void* add(void* arg);

int is_with_mutex = 1;
long long int global_value = 0;
pthread_mutex_t global_value_mutex;

int main(int argc, char** argv)
{
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);

    pthread_t tid_add_1;
    pthread_t tid_add_2;
    pthread_t tid_add_3;
    pthread_t tid_add_4;
    pthread_t tid_add_5;

    if(argc == 2)
    {
        is_with_mutex = atoi(argv[1]);
    }

    if(is_with_mutex)
    {
        int mutex_init_res = pthread_mutex_init(&global_value_mutex, NULL);

        if(mutex_init_res != 0)
        {
            perror("pthread_mutex_init failed. ");
            exit(1);
        }
    }


    long long int values[] = {10, 20, 30, 40, 50};

    pthread_create(&tid_add_1, &thread_attr, add, &values[0]);
    pthread_create(&tid_add_2, &thread_attr, add, &values[1]);
    pthread_create(&tid_add_3, &thread_attr, add, &values[2]);
    pthread_create(&tid_add_4, &thread_attr, add, &values[3]);
    pthread_create(&tid_add_5, &thread_attr, add, &values[4]);

    pthread_join(tid_add_1, NULL);
    pthread_join(tid_add_2, NULL);
    pthread_join(tid_add_3, NULL);
    pthread_join(tid_add_4, NULL);
    pthread_join(tid_add_5, NULL);


    long long int expected_sum = 0;
    for(int i = 0; i < 5; i++)
    {
        expected_sum += values[i];
    }

    printf("sum result by threads: %d , expected: %d\n", global_value, expected_sum);

    if(is_with_mutex)
    {
        int mutex_destroy_res = pthread_mutex_destroy(&global_value_mutex);

        if(mutex_destroy_res != 0)
        {
            perror("pthread_mutex_destroy failed.");
            exit(1);
        }

        exit(0);
    }
}

void* add(void* arg)
{
    long long int num = *((long long int*) arg);

    printf("thread run with num: %lld\n", num);



    int value_to_add = 0;

    for(int i = 0; i < 5; i++)
    {
        value_to_add += num;
        sleep(1);
        value_to_add -= num;
    }

    value_to_add += num;

    if(is_with_mutex)
    {
        pthread_mutex_lock(&global_value_mutex);
    }

    global_value += value_to_add;

    if(is_with_mutex)
    {
        pthread_mutex_unlock(&global_value_mutex);
    }

    return NULL;
}
