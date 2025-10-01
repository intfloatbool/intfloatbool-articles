#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* add(void* arg);

long long int global_value = 0;

int main(void)
{
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);

    pthread_t tid_add_1;
    pthread_t tid_add_2;
    pthread_t tid_add_3;
    pthread_t tid_add_4;
    pthread_t tid_add_5;

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
}

void* add(void* arg)
{
    long long int num = *((long long int*) arg);
    for(int i = 0; i < 5; i++)
    {
        global_value += num;
        global_value -= num;
        sleep(1);
        global_value += num;
    }
    return NULL;
}
