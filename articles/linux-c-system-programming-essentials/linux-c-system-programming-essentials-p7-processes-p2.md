
### Коммуникация между процессами  

#### pipe()

Функция ```pipe()``` из заголовка ```<unistd.h>``` открывает 2 файловых дескриптора, один на чтение, другой на запись.  
Можно использовать его для **передачи байтов** между родительским и дочерним процессом (от функции ```fork()```).  

Каждое направление связи должно иметь свой набор дескрипторов от функции ```pipe()``` .  
Если направление одно - от родителя к дочернему, то и ```pipe()``` нужен 1.
Если направлений несколько - от родителя к дочернему и наоборот , то ```pipe()``` нужно несколько.

Итого, сколько направлений , столько и функций ```pipe()``` нужно.  

**Программа**  
```pipe.c```:  
[Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/processes/ipc_samples/pipe.c)  

В этой программе реализован пример передачи сообщений в двух направлениях:  
1. от родительского к дочернему
2. от дочернего к родительскому  

Сперва определяются массивы из двух значений (0 - чтение, 1 -запись):  

```c
int pipe_parent_to_child[2] = { 0 };
int pipe_child_to_parent[2] = { 0 };
```

Затем эти массивы заполняются новыми файловыми дескрипторами для общения, вызовом ```pipe()```:  

```c
pipe( pipe_parent_to_child );
pipe( pipe_child_to_parent );
```

Делается ```fork()``` для разделения процесса:  

```c
pid_t pid = fork();
```

Начинаются условные ветвления, в зависимости от ```pid```, чтобы понять в каком процессе находится программа (родитель или дочерний):  

```c
if( pid > 0 )
{
    /* родительский процесс */
    ...
}
else
{
    /* дочерний процесс */
    ...
}
```  

Для работы с дескрипторами файлов используются **стримы** для удобства работы.

1. Получаем стрим (```FILE*```) через передачу дескриптора в функцию ```fdopen()```
2. Что-то делаем со стримом, пишем в него через ```fprintf()``` или читаем через ```fgets()``` .
3. Освобождаем стрим через ```fflush()``` .

Массив дескрипторов (например, ```pipe_parent_to_child```), заполненный функцией ```pipe()``` **всегда имеет 2 элемента**:    

- ```[0]``` - дескриптор для чтения.
- ```[1]``` - дескриптор для записи.

Пример отправки сообщения от родителя к дочернему:  
 ```c
/* send message to child */
FILE* to_child = fdopen(pipe_parent_to_child[1], "w");
fprintf(to_child, "<message from PARENT>\n");
fflush(to_child);
```

Пример получения сообщения дочерним процессом от родителя:  
```c
/* read message from parent */	
FILE* from_parent = fdopen(pipe_parent_to_child[0], "r");
fgets(parent_message, MESSAGE_MAX_BYTES, from_parent);
printf("message to child -> %s\n", parent_message);
fflush(from_parent);
```  

**Пайпы** (pipes) *хороши своей производительностью*, но не без греха:    
- Работают только на связанных процессах (через ```fork()```) .
- Только передача байтов.
- Раздутый код, на каждое направление передачи требуется свой ```pipe()``` и набор данных для работы с ним.
- Неудобно диагностировать ошибки из-за направленностей и раздутого кода.

#### FIFO

FIFO еще называют **named pipes** . FIFO создается функцией ```mkfifo()``` , которая создает обычный файл.  

> Необходимые заголовки:  
> ```c
> <sys/types.h>
> <sys/stat.h>
> ```

> Сигнатура функции ```mkfifo()``` :  
> ```c
>  int mkfifo(const char *pathname, mode_t mode);
> ```

Этот файл является точкой обмена сообщениями между процессами. Что намного удобнее, чем использовать ```pipe()``` .  

  
**Программа**  

1. ```fifo_sender.c```:
> [Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/processes/ipc_samples/fifo_sender.c)  

2. ```fifo_receiver.c```:
> [Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/processes/ipc_samples/fifo_receiver.c)  

Одна программа **отправитель** - ```fifo_sender.c``` , другая **получатель** - ```fifo_receiver.c``` .  

**Рассмотрим** **```fifo_sender.c```** :  

1. Создается файл ```/tmp/fifo_example22``` функцией ```mkfifo()```
> ```c
> int mkfifo_result = mkfifo(fifo_filename, 0644);
> ```

2. Получаем дескриптор этого файла только для чтения:  
> ```c
> fifo_fd = open(fifo_filename, O_WRONLY);
> ```

3. Пишем в файл сообщение каждую секунду (```sleep(1)```) :   
> ```c
> while(1)
> {
>   dprintf(fifo_fd, "%s\n", argv[1]);
>   fprintf(stdout, "message send.\n");
>   sleep(1);
> }

4. В обработчике сигналов (когда программа прерывается)  делаем высвобождение:  
> ```c
> /* закрываем дескриптор файла */
> int close_res = close(fifo_fd);
>
> /* удаляем сам файл */
> int unlink_res = unlink(fifo_filename); 
> ```  


Как только кто-то с этого файла начинает читать, появляется запись каждую секунду, которую получатель (тот кто читает файл) принимает себе.  

**Рассмотрим** **```fifo_receiver.c```** :  

Максимально простая программа, которая читает файл ```/tmp/fifo_example22``` через стрим ```FILE*``` , и выводит сообщения из файла в терминал .  

```c
/* открываем стрим файла для чтения "r" */
FILE* fifo_file = fopen("/tmp/fifo_example22", "r");
```  

```c
/* посимвольно выводит содержимое файла */
unsigned char c = { 0 };
while( ( c = getc(fifo_file) ) != EOF )
{
    putchar(c);
}
```  


Компилируем программы и запускаем в таком порядке:

> 1. Компиляция и запуск sender'a  
> ```bash
> gcc fifo_sender.c -o fifo_sender.out
>
> ./fifo_sender.out
> ```

*Запуск получателя нужно делать **в отдельном терминале** , это две разные программы, которые должны быть запущены по отдельности!*

> 2. Компиляция и запуск receiver'а  
> ```bash
> gcc fifo_receiver.c -o fifo_receiver.out
> 
> ./fifo_receiver.out
> ```

После запуска будет примерно такой результат:  

**Отправитель**

```bash
./fifo_sender.out kika
message send.
message send.
message send.
message send.
message send.
```

**Получатель**

```bash
./fifo_receiver.out 
kika
kika
kika
kika
kika
```

Как только прерывается программа получателя ```fifo_receiver``` , автоматически (из-за сигналов) закроется программа ```fifo_sender```. 

Итого получился относительно удобный процесс передачи сообщений от одного процесса другому.  

В отличии от ```pipe()``` здесь нет форков, оба процессы независимы, и общаются через один **временный файл** .

#### Очереди сообщений ( Message queues )

Очередь сообщений из заголовка ```<mqueue.h>``` , это более высокоуровневый и удобный инструмент для общения между процессами.  

Общение идет через один ```mq``` файл. **Записанные сообщения сохраняются** пока не будут прочитаны. То есть обмен сообщениями идет в ленивом режиме, только по требованию получателя, и независимо от работающего процесса получателя.   

> **```mq-файл```** *это не самый обычный файл, поэтому все операции над его дескриптором идут не через 
> стандартный ввод вывод, а функциями с префиксом ```mq_``` , например - ```mq_close()```*

Еще из отличий от ```FIFO``` - это обязанность *получателя* закрывать поток данных, а не отправителя.


**Программа**  

1. ```mq_sender.c```:
> [Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/processes/ipc_samples/mq_sender.c)  

2. ```mq_receiver.c```:
> [Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/processes/ipc_samples/mq_receiver.c)  

**Рассмотрим отправителя** **```mq_sender.c```** :  

> 1. Создание структуры-представления файла для очереди сообщений  
> Структура типа ```mq_attr``` является представлением для настройки обменом сообщений.  
> ```c
> struct mq_attr msg_attr = { 0 };
> /* ограничение на кол-во сообщений */
> msg_attr.mq_maxmsg = 10;
> /* максимальный размер сообщения в байтах */
> msg_attr.mq_msgsize = 2048;
>   

> 2. Создание файла для обмена сообщениями  
> Нужно создать специальный файл через функцию ```mq_open()``` , который  
> будет в себе хранить настройки из ```msg_attr``` и самую очередь сообщений.  
> Функция ```mq_open()``` возвращает дескриптор файла.
> ```c
> int mq_d = mq_open("/my_queue", O_CREAT | O_RDWR, 0644, &msg_attr);
> ```

> 3. Запись сообщения в очередь  
> Функцией ```mq_send()``` через дескриптор файла, передается сообщение из аргумента программы.  
> ```c
> /* mq_send ( дескриптор файла, сообщение, длина сообщения, приоритет) */
> int mq_send_res = mq_send( mq_d, argv[1], strlen(argv[1]), 1 );
> ```

> 4. Закрытие дескриптора файла.  
> Так как был открыт дескриптор ```mq-файла``` , требуется его освободить.  
> ```c
> mq_close( mq_d );
> ```

> 5. Компиляция программы  
> ```bash
> gcc -Wall -Wextra -pedantic mq_sender.c -o mq_sender.out
> ```

> > *В старых версиях Linux (до 2022, glibc ≥ 2.34) нужно явно указывать библиотеку*  
> > ```-lrt``` *(rt - realtime)*  


> 6. Отправка сообщений  
> ```bash
> ./mq_sender.out "msg #0"
> ./mq_sender.out "msg #1"
> ./mq_sender.out "msg #2"
> ```

**Рассмотрим получателя** **```mq_receiver.c```** :  

> 1. Открытие ```mq-файла```  
> ```c
> mqd_t mq_d = mq_open("/my_queue", O_RDONLY);
> ```

> 2. Получение атрибутов из ```mq-файла```  
> ```c
> struct mq_attr msg_attr = { 0 };
> int mq_get_attr_res = mq_getattr( mq_d, &msg_attr );
> ```

> 3. Получение количества сообщений  
> ```c
> long queue_count = msg_attr.mq_curmsgs;
> ```

> 4. Вывод сообщений по очереди  
> ```c
> for( int i = 0; i < queue_count; i++ )
> {
>   ssize_t receive_res = mq_receive(mq_d, buffer, msg_size, NULL);
>   /* .... */
>   printf("%s\n", buffer );
>   memset( buffer, '\0',  msg_size);
> }
> ```


> 5. Очистка ресурсов, после использования  
> ```c
> /* очистк буфера сообщения */
> free(buffer);
>
> /* закрытие дескриптора */
> mq_close(mq_d);
>
> /* удаление mq-файла */
> mq_unlink("/my_queue");
> ```

> 5. Компиляция программы  
> ```bash
> gcc -Wall -Wextra -pedantic mq_receiver.c -o mq_receiver.out
> ```

> 6. Чтение сообщений
> ```bash
> ./mq_receiver.out 
>
> current messages count in queue: 3
>
> msg #0
> msg #1
> msg #2
> ```

#### Unix-сокеты ( Unix domain sockets, UDS )

Коммуникация между процессами, с помощью **unix domain socket**'ов (UDS) очень проста и удобна. ```UDS``` - это аналог **TCP/IP** сокетов, которые используются для обмена сообщениями через сеть.  

UDS отличается от tcp/ip сокетов тем, что работает только локально (внутри одного хоста), и представлен специальным **сокет-файлом** (socket file) , то есть имеется обычный путь к файлу UDS-сокета в файловой системе, тогда как в tcp/ip сокетах адресом является адрес интернет-протокола.  

Это **самый распространенный способ** создания коммуникации между процессами на одной машине. Однако UDS работает **только для UNIX-подобных систем**, если требуется кроссплатформенность, то стоит рассмотреть для межпроцессной связи - **tcp/ip сокеты** .  


**Программа**  

1. ```uds_server.c```:
> [Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/processes/ipc_samples/uds_server.c)  

2. ```uds_client.c```:
> [Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/processes/ipc_samples/uds_client.c)  

  

**Рассмотрим сервер** **```uds_server.c```** :  

> 1. Глобальное объявление имени файла для сокета
> ```c
> const char* sock_name = "/tmp/my_socket";
> ```

> 2. Глобальное объявление дескрипторов - сокета и файла для данных
> ```c
> int sock_fd = -1;
> int data_fd = -1;
> ```

> 3. Определение функций - обработчик сигнала и освобождение ресурсов
> ```c
> void on_signal(int signum);
> void dispose();
> ```

**```main()```**  

> 4. Инициализация обработчиков сигналов, с которыми будет вызвана функция ```on_signal(int)```
> ```c
> sig_action.sa_handler = on_signal;
> sigfillset(&sig_action.sa_mask);
> sig_action.sa_flags = SA_RESTART;
> sigaction(SIGTERM, &sig_action, NULL);
> sigaction(SIGINT, &sig_action, NULL);
> sigaction(SIGQUIT, &sig_action, NULL);
> sigaction(SIGABRT, &sig_action, NULL);
> sigaction(SIGPIPE, &sig_action, NULL);
> ```

> 5. Открытие uds-сокета 
> ```c
> sock_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
> ```

> 6. Параметры адреса для сокета
> ```c
> struct sockaddr_un sock_addr = { 0 };
> sock_addr.sun_family = AF_UNIX;
> strcpy(sock_addr.sun_path, sock_name);
> ```

> 7. Установка связи с сокетом функцией ```bind()```
> ```c
> int bind_res = bind(
>       sock_fd,
>       (const struct sockaddr*) &sock_addr,
>       sizeof(struct sockaddr_un)
>    );
> ```

> 8. Установка прослушивания сокета функцией ```listen()```
> ```c
> /* где 20 - максимальное количество клиентов. */
> int listen_res = listen(sock_fd, 20);
> ```

> 9. Установка подключения с клиентом функцией ```accept()```  
> В результате вернется новый дескриптор для обмена данными.  
> ```c
> data_fd = accept(sock_fd, NULL, NULL);
> ```

> 10. Объявление главного цикла **Main Loop**
> ```c
> while(1)
> {
>       /* Внутренний цикл чтения данных от клиента */
>       /* .... */
> }
> 
> /* в конце главного цикла отправляем сообщение клиенту, что их запрос обработан */
> char* back_msg = "message received";
> write(data_fd, back_msg, strlen(back_msg));
> ```

> 11. Объявление цикла для чтения данных клиента
> ```c
>   while(1)
>   {
>       read_res = read(data_fd, buffer, max_len );
>
>       if( read_res == -1 )
>       {
>           perror("read() failed. ");
>           dispose();
>        }
>       else if( read_res == 0 )
>       {
>           printf("> > client disconnected.\n");
>           dispose();
>       }
>       else
>       {
>           printf("> > > message from client:\n\t%s\n", buffer);
>           break;
>        }
>    }
> ```

> 12. Реализация функций-обработчиков
> ```c
> void on_signal(int signum)
> {
>    dispose();
> }
>
> void dispose()
> {
>    close(sock_fd);
>    close(data_fd);
>    unlink(sock_name);
>    exit(0);
> }
> ```  

Далее запускаем, и ждем сообщений клиента:  
```bash
./uds_server.out
```

**Рассмотрим клиента** **```uds_client.c```** :  

> 1. Клиент создает *свой* сокет  
> ```c
> int sock_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
> ```

> 2. Подключение к сокету, через общее имя файла **```"/tmp/my_socket"```**
> ```c
> /* ... */
> /* имя этого файла - точка связи, у сервера такая-же. */
> const char* sock_name = "/tmp/my_socket";
> 
> struct sockaddr_un sock_addr = { 0 };
> sock_addr.sun_family = AF_UNIX;
> strcpy(sock_addr.sun_path, sock_name);

> int conn_res = connect(
>    sock_fd,
>    (const struct sockaddr*) &sock_addr,
>    sizeof(struct sockaddr_un)
> );
> ```

> 3. Основной цикл
> ```c
> while(1)
> {
>   printf("enter message: ");
>   /* чтение сообщения от пользователя */
>   fgets( send_buffer, sizeof(send_buffer), stdin );
>
>   send_buffer[strcspn(send_buffer, "\n")] = '\0';
>
>   /* отправка сообщения серверу */
>   int write_res = write(sock_fd, send_buffer, strlen(send_buffer) + 1);
>
>   /* получение ответа от сервера */
>   int read_res = read(sock_fd, recv_buffer, max_len);
>   printf("%s\n", recv_buffer);
> }
> ```

Далее запускаем (в отдельном терминале, сервер должен быть запущен!), и пишем сообщения:  
```bash
./uds_client.out
enter message: goga
message received
enter message: boka
message received
enter message:
...
```

Смотрим что там с сервером:  
```bash
...
> > client connected.
> > > message from client:
    goga
> > > message from client:
    boka
```

Все работает!


**Итоги**:  

1. Клиент и сервер могут общаться друг с другом в **двух направлениях**, в отличии от ```fifo``` и ```pipe```
2. Коммуникация между сервером и клиентом происходит через **единый файл данных сокета**, кто-то туда пишет, кто-то от туда читает.
3. Сокеты работают через подключения, а не через жизненный цикл приложения.
4. Сокеты позволяют процессам быть проще и более независимыми, в отличии от других способов IPC (межпроцессной коммуникации).
5. Сокеты (uds или tcp/ip) - это самый распространенный способ IPC.

**[Следующая статья](linux-c-system-programming-essentials-p8-threads.md)**