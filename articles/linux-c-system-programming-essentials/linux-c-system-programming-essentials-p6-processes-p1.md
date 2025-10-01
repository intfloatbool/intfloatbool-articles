### Функция ```system()```

Это более высокоуровневый способ работы с процессами, более удобный, чем вручную использовать ```execl()``` и ```fork()```.  

Функция ```system()``` принимает параметром команду, которую нужно выполнить, также как мы пишем ее в терминале.  

Внутри себя она использует:  
1. ```fork()``` для создания копии текущего процесса.
2. ```execl()``` для замещения этой копии процесса на другую программу.

Находится в заголовке ```<stdlib.h>```. 

#### Пример программы
```process_system.c```:  
[Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/processes/process_system.c)  

Эта простая программа выполняет ```touch zika.txt``` в отдельном дочернем процессе, используя функцию ```system()```.

Простой вызов ```system()``` выполняет внутри себя ```fork()``` а потом ```execl()```. Посмотрите еще раз, как с ними работать в предыдущих разделах, и вы поймете ее лаконичность, ограничиваясь только одним вызовом ```system()``` он внутри себя делает всю *неприятную* работу.  

```c
int system_result = system("touch zika.txt");

if( system_result == -1)
{
    perror("some error in command.");
    exit(1);
}

exit(0);
```

При вызове этой программы, произойдет следующее:
1. Создется новый, дочерний процесс.
2. В этом дочернем **отдельном процессе** выполнится ```touch zika.txt```
3. В итоге мы имеем файл ```zika.txt``` , там, где вызвали нашу программу.

 
### Создание фоновых процессов

Фоновый процесс, также называемый **демоном** (daemon).  Он запускается в ОС и выполняет какие-то задачи.  
Примеры демонов:  

- **веб-серверы** - nginx, apache.
- **ssh** - сервис, который помогает удаленно управлять ОС, в безопасном режиме.

> **Демоны уже не делают вручную (!!!)** 
> > Далее будет описан примерный план создания своего демона, однако
> > в большинстве систем есть **менеджер сервисов**, например **```systemd```** .  
> > С этим менеджером можно создавать сервисы и управлять ими. Не нужно возиться с ```fork'ами``` ,
> > можно делать перезапуск, есть логи и так далее, все удобства.   
> >  
> > То есть мы только берем программу, создаем из нее сервис c помощью ```systemd``` и все, фоновый сервис готов!  


Демон создается через ```fork()``` , но "ритуал" создания демона отличается от просто создания дочернего процесса, выглядит он так:

- С помощью функции ```setsid()```, разрывается связь с управляющим терминалом (```tty```), чтобы его нельзя было отменить через ```CTRL+D```.  
- Делается двойной (double-forking) ```fork()```, дочерний процесс создает еще один процесс, чтобы демон точно не был лидером сессии.
- Все унаследованные дескрипторы закрываются или перенаправляются (STDIN, STDOUT, STDERR и так далее).
- Меняется рабочая директория на основную - ```"/"```, функцией ```chdir()```, чтобы демон не держал точку монтирования.
- Сбрасывается маска прав через функцию ```umask(0)``` , чтобы не зависеть от родителя.

Итого, чтобы демон стал полностью независимым и автономным, нужно оторвать его от наследований. Процесс становится демоном, когда он вручную "оторван" от:  

1. Терминала
2. Сессии
3. Унаследованных дескрипторов

**НО!** используйте вместо этого **```systemd```** !!!  

### Работа с менеджером-сервисов systemd

Тот самый главный процесс с идентификатором ```1``` это **```systemd```** !

- Демоны обрабатываются c помощью специальных файлов **unit files**.
- Логирование обрабатывается с помощью **journald**

Чтобы увидеть все активные фоновые юниты (*units*):  
```bash
systemctl
```

**Все сервисы (юнит-файлы)**  размещаются в ```/lib/systemd/system``` .  

Чтобы увидеть **журнал логов** от сервисов:  
```bash
sudo journalctl
```

#### 1. Создание программы

```process_system_daemon```:  
[Ссылка](./code_sources/linux-c-system-programming-essentials/8-system-calls/processes/process_system_daemon.c)

Программа каждые 5 секунд пишет в ```stderr``` сообщение. Если ее запустить из терминала, то сообщения будут видны в терминале.

Это самый банальный пример процесса-демона, который каждые N секунд что-то делает.

Когда эта программа будет работать как сервис через ```systemd``` она будет выводит свои сообщения в файлы логов. И работать в фоне, не завися от терминала.  

Такие программы, должны обрабатывать сигналы и ОС, чтобы понять что делать при получении сигнала. 

```c
/* .... */
static void on_signal(int sig) 
{
    /* to supress unsused warning */
    (void)sig; 
    g_stop = 1;
}
/* .... */
```


```c
/* .... */
struct sigaction sa = {0};
sa.sa_handler = on_signal;
sigemptyset(&sa.sa_mask);
sigaction(SIGINT, &sa, NULL);
sigaction(SIGTERM, &sa, NULL);
/* .... */
```

В нашем случае, при любом из сигналов ```SIGINT``` (прерывание) или ```SIGTERM``` (прекращение) , происходит изменение атомарной переменной ```g_stop```, которая сигнализирует о том, что **цикл программы** (в котором она каждые 5 секунд делает вывод сообщений) нужно остановить:  

```c
while(!g_stop) 
{
    log_line("[my_daemon] XXX tick=%lu", tick);
    tick++;

    sleep_monotonic(interval);
}
```

Затем программу нужно собрать:  
```bash
cc -O2 -Wall -Wextra -pedantic process_system_daemon.c -o process_system_daemon
```

И отправить в ```/usr/local/bin``` , чтобы запускать ее из ```systemd```:  
```bash
cp process_system_daemon /usr/local/bin/process_system_daemon
```

#### 2. Создание юнит-файла

Чтобы ```systemd``` понимал что делать с программой, создается юнит-файл, где описывается информация о программе.

```bash
touch process_system_daemon.service
```

Внутри должны быть минимальные параметры для создания сервиса:  

> **Описание сервиса**    
> ```ini
> [Unit]
> Description=Simple XXX test daemon
> ```

> **Конфигурация сервиса**  
> 1. ```Type``` - тип сервиса.  
> 2. ```ExecStart``` - команда для запуска (путь к исполняемому и аргументы).  
> 3. ```Restart``` - событие для перезапуска.
> 4. ```RestartSec``` - время перезапуска.
> 5. ```StandardOutput``` - стрим для стандартного вывода (идет в журнал логов!).
> 6. ```StandardError``` - стрим для вывода ошибок (идет в журнал логов!).
> ```ini
> [Service]
> Type=simple
> ExecStart=/usr/local/bin/process_system_daemon 3
> 
> Restart=on-failure
> RestartSec=2s
> 
> StandardOutput=journal
> StandardError=journal
>
> ```

> **Поведение при установке сервиса**  
> При запуске ```systemctl enable ...``` будет создан симлинк для работы сервиса.      
> ```ini
> [Install]
> WantedBy=multi-user.target
> ```

Теперь это файл ```process_system_daemon.service``` нужно скопировать в папку с сервисами ```systemd```:   
```bash
sudo cp process_system_daemon.service /etc/systemd/system/process_system_daemon.service
```

#### 3. Запуск сервиса

Теперь, когда необходимые файлы готовы по путям:
1. Исполняемый файл программы - ```/usr/local/bin/process_system_daemon```
2. Юнит-файл - ```/etc/systemd/system/process_system_daemon.service```

Можно запустить сервис:  

```bash
sudo systemctl daemon-reload

sudo systemctl enable --now process_system_daemon.service
```

Проверяем статус:
```bash
sudo systemctl status process_system_daemon.service
```

Вывод о статусе должен содержать ```active``` и ```enabled```, означающие что сервис запущен и включен.
Примерно так:    

```bash
● process_system_daemon.service - Simple XXX test daemon
     Loaded: loaded (/etc/systemd/system/process_system_daemon.service; enabled; preset: disabled)
    Drop-In: /usr/lib/systemd/system/service.d
             └─10-timeout-abort.conf
     Active: active (running) since Thu 2025-09-25 17:32:51 MSK; 40s ago

```

Сервис запущен и работает, можно посмотреть на его логи из журнала:  

```bash
journalctl -u process_system_daemon.service
```

Видно те самые логи, которые мы отправляем из демона:  

```bash
Sep 25 17:32:51 192-168-0-116.local process_system_daemon[7901]: [my_daemon] XXX start pid=7901, interval=3.000 sec
Sep 25 17:32:51 192-168-0-116.local process_system_daemon[7901]: [my_daemon] XXX tick=1
Sep 25 17:32:54 192-168-0-116.local process_system_daemon[7901]: [my_daemon] XXX tick=2
Sep 25 17:32:57 192-168-0-116.local process_system_daemon[7901]: [my_daemon] XXX tick=3
...
```

#### Остановка, перезапуск и удаление сервиса

> **Перезапуск**  
> ```bash
> sudo systemctl restart process_system_daemon.service
> ```

> **Остановка**  
> ```bash
> sudo systemctl stop process_system_daemon.service
> ```

> **Удаление**  
> ```bash
> sudo systemctl disable --now process_system_daemon.service 
> sudo systemctl stop process_system_daemon.service
> 
> sudo rm /usr/local/bin/process_system_daemon
> sudo rm /etc/systemd/system/process_system_daemon.service
> 
> sudo systemctl reset-failed
> sudo systemctl daemon-reload
> ```

**[Следующая статья](linux-c-system-programming-essentials-p7-processes-p2.md)**