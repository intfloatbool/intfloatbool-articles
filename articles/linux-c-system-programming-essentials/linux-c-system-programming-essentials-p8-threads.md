### Потоки (POSIX Threads)

Потоки позволяют сделать выполнение какой-либо функции одновременно, то есть *распараллелить работу*. Благодаря потокам, мы можем ускорять работу приложения, разделив единицы выполнения, использовав *ядра процессора*. В потоках самое важное это синхронизация общей памяти в процессе,  и отсутствие **состояния гонки** - будет показательный пример.

#### Простой пример работы с потоками

```th_sample.c```:  
> [Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/pthreads/th_sample.c)  

**Пример выполнения**  

```bash
./th_sample.out 20 33
....number 20 is power of two.
.....number 33 is NOT power of two.
work done.
```
В двух потоках идет "тяжелая работа" (симуляция ее через ```sleep()```), которая по результату возвращает результат - делится ли число из аргумента на 2, или нет.  

**Рассмотрение программы**  

1. Объявление функций и структур  

> ```is_power_of_two()``` - сама функция, которая будет работать в отдельных потоках, для вычисления результата.  

> ```progress()```- функция, которая работает в отдельном потоке, показывает прогресс-бар в выводе (точками).  

> ```th_data``` - структура, которую будем передавать в функцию ```is_power_of_two```.   

```c
void* is_power_of_two(void* arg);
void* progress(void* arg);

typedef struct {
    unsigned int delay;
    long long int number;
} th_data;
```

2. Создание структур для потоков  
Под каждый поток, создается объект структуры типа ```pthread_t```.  
```c
pthread_t tid_is_power_of_two_1;
pthread_t tid_is_power_of_two_2;
pthread_t tid_progress;
```

3. Создание и инициализация ```pthread_attr_t```  
Это атрибут, который передается при создании потока. В данной задаче он нужен просто чтобы создавать потоки.  
Его нужно проинициализировать через ```pthread_attr_init``` а затем уничтожить с помощью ```pthread_attr_destroy```.  

```c
pthread_attr_t thread_attr = {0};
pthread_attr_init(&thread_attr);
/* ... */
/* в конце программы */
pthread_attr_destroy(&thread_attr);
```

4. Создание потока для функции ```progress```  

```c
pthread_create(&tid_progress, &thread_attr, progress, NULL);

/* не блокируем дальнейшее выполнение программы, отпускаем поток жить фоном */
pthread_detach(tid_progress); 
```

5. Создание потоков для функции ```is_power_of_two```  

```c
/* заполнение структуры данных для потока A*/
th_data data_thread_1 = { 0 };
data_thread_1.delay = 5;
data_thread_1.number = atoll(argv[1]);

/* заполнение структуры данных для потока B*/
th_data data_thread_2 = { 0 };
data_thread_2.delay = 10;
data_thread_2.number = atoll(argv[2]);

/* Создание потоков и передача им данных */
pthread_create(&tid_is_power_of_two_1, &thread_attr, is_power_of_two, &data_thread_1);
pthread_create(&tid_is_power_of_two_2, &thread_attr, is_power_of_two, &data_thread_2);

/* ожидание окончания потоков */
pthread_join(tid_is_power_of_two_1, NULL);
pthread_join(tid_is_power_of_two_2, NULL);

/* .... */

/* после конца работы этих двух потоков, принудительно отменяем наш фоновый поток progress */
int cancel_result = pthread_cancel(tid_progress);

/* .... */

printf("work done.\n");

exit(0);
```

6. Определения функций ```progress``` и ```is_power_of_two```  
```c
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
```

**Итоги**  

1. Запускается 3 потока - (```is_power_of_two``` x 2) + (```progress``` x 1 );
2. Поток ```progress``` работает в фоне, и пишет точки в вывод, отображая какой-то прогресс.
3. Потоки ```is_power_of_two``` работаю одновременно, но основная программа ```main``` блокируется до появления двух последовательных результатов потока, через функцию ```join```.
4. Поток ```progress``` принудительно отменяется, как только оба потока ```is_power_of_two``` заканчивают работу, 
5. Работа закончена.

Видна разница между функциями ```pthread_join``` и ```pthread_detach```, первая прерывает вызывающий эту функцию поток ```main```, а вторая не блокирует вызывающий поток, что удобно для подобных ```progress``` функций, которые что-то делают в фоне.

#### Состояние гонки
  
```th_race_condition.c```:  
> [Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/pthreads/th_race_condition.c)  


**Cостояние гонки** - это ошибка в программе (многопоточной), которая возникает, когда результат выполнения программы зависит от *непредсказуемого* порядка выполнения *параллельных операций* над общими ресурсами.  

То есть **результат выполнения программы - непредсказуемый!**  

**Пример выполнения** (может отличаться, на то это и состояние гонки!):  

```bash
./th_race_condition.out 
sum result by threads: 150 , expected: 150

./th_race_condition.out 
sum result by threads: 100 , expected: 150 #  А вот и состояние гонки!

./th_race_condition.out 
sum result by threads: 150 , expected: 150
```

**Рассмотрение программы**  

Это очень простая программа, где создается 5 потоков, которые одновременно работают над общей памятью - глобальной переменной ```global_value```, изменяя ее (добавляя значения к ней).  

Есть глобальная переменная:  
```c
long long int global_value = 0;
```

Есть массив значений для 5 потоков, которые нужно добавить к этой переменной:  

```c
long long int values[] = {10, 20, 30, 40, 50};

pthread_create(&tid_add_1, &thread_attr, add, &values[0]);
/* дальнейшие создания еще 4х потоков */
/* .... */ 

pthread_join(tid_add_1, NULL);
/* дальнейшие ожидание еще 4х потоков */
/* .... */ 
``` 

При *правильном* выполнении, значение ```global_value``` должно быть равным 150 = (10 + 20 + 30 + 40 + 50).  

Потоки в *непредсказуемом* порядке меняют глобальную переменную:  

```c
void* add(void* arg)
{
    long long int num = *((long long int*) arg);
    for(int i = 0; i < 5; i++)
    {
        global_value += num;
        sleep(1);
        global_value -= num;
    }
    global_value += num;
    return NULL;
}
```

В итоге - получаем состояние гонки и неопределенный результат. Что является очень противным багом!   

Помощь в этом - использование **синхронизации доступа к общим данным** (```global_value```), чтобы только один поток мог обращаться к переменной за единицу времени. 

#### Избегаем состояния гонки с помощью ```mutex```

```th_mutex.c```:  
> [Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/pthreads/th_mutex.c)  


**mutex** это один из механизмов синхронизации, он основан на *блокировке потоков*, когда гарантируется, что только один поток в один момент времени, может получить доступ к общим данным.  

Но mutex, так как он основан на блокировке, может затормозить программу, поэтому востребованные способы синхронизации немного другие, так называемые **non-blocking** , неблокирующие механизмы. Например атомарные типы данных из - <stdatomic.h>, и так далее.   

Программа в этом примере с мьютексом, дополняет программу, где демонстрируется состояние гонки.   

1. Объявление глобальной переменной мьютекса  
```c
pthread_mutex_t global_value_mutex;
```

2. Инициализация и деинициализация мьютекса  

```c
int mutex_init_res = pthread_mutex_init(&global_value_mutex, NULL);

/* .... */

int mutex_destroy_res = pthread_mutex_destroy(&global_value_mutex);
```

3. Делаем операции с глобальной переменной атомарными (только один поток имеет доступ в моменте)  

```c
pthread_mutex_lock(&global_value_mutex);

/* операции над global_value ... */
/* .... */

pthread_mutex_unlock(&global_value_mutex);
```

**Итог**  

Программа **стала заметно медленнее**, потому что идет блокировка выполнения потоков из-за мьютекса. Пока один поток работает с мьютексом, другие тупо ждут.

Зато при выполнении программы, нет никакого **состояния гонки** , так как доступ к общей переменной охраняется мьютексом:  

```bash
./th_mutex.out
sum result by threads: 150 , expected: 150

./th_mutex.out
sum result by threads: 150 , expected: 150

./th_mutex.out
sum result by threads: 150 , expected: 150

### И так далее, 150 как и ожидается.
```

#### Ускоряем работу с ```mutex```


```th_mutex_optimized.c```:  
> [Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/pthreads/th_mutex_optimized.c)  


Это обновленная версия, предыдущего примера. Она работает намного быстрее.  

**Основное**  

-  Минимизация использования мьютекса, потоки обновляют свои **локальные переменные** и только потом обращаются к глобальной переменной через мьютекс.

```c

/* поток работает с локальными переменными */
int value_to_add = 0;

for(int i = 0; i < 5; i++)
{
    value_to_add += num;
    sleep(1);
    value_to_add -= num;
}
value_to_add += num;

/* затем, для синхронизации идет обращение к глобальной переменной, вот тут мьютекс и нужен */
pthread_mutex_lock(&global_value_mutex);

global_value += value_to_add;

pthread_mutex_unlock(&global_value_mutex);
```
