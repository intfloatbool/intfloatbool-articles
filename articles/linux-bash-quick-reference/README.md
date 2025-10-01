# Справочник по Linux и частых инструкций для Bash

## Файловая система
### Некоторые важные места файловой системы
- ```/etc``` - Конфигурационные файлы ОС и ПО
- ```/bin```,```/sbin``` - Системные исполняемые файлы
- ```/usr/bin```,```/usr/local/bin``` - установленное пользователем ПО и ваши исполняемые файлы.
- ```/var/log```,```/var/lib``` - var это часто изменяемые данные, логи приложений, библиотеки и так далее.
- ```/var/lib/systemd``` - конфигурация системы инициализации systemd.
- ```/etc/systemd/system``` - пользовательские файлы конфигурации systemd.
- ```/dev``` - аппаратные устройства.
- ```/proc``` - файлы процессов.

### Типы файлов 
- **Обычный файл** - текстовый или бинарный исполняемый файл. Начинается с дефиса, пример вывода ```ls -l <file>``` -> ```-rw-r--r--```

- **Каталог** - особый файл для структурирования других файлов (папка). Начинается с буквы ```d``` , пример: ```ls -l <dir>``` -> ```-drwxr-xr-x```

- **Блочное устройство** - специальный тип файлов, обеспечивает буферизированный доступ к оборудованию (hdd, ssd и так далее). Начинается с буквы ```b```.

- **Именованный канал (или конвейер)** - используются для коммуникации между процессами. Начинается с буквы ```p```.

- **Ссылки** - ссылки бывают двух видов *жесткие* и *символические* (мягкие, symlink). Символическая ссылка полезна как ярлык, чтобы иметь доступ к файлу из разных мест. Начинается с буквы ```l```.

- **Сокеты** - используются для коммуникации между процессами. Например через сокет коммуницирует ```nginx``` и ```PHP-FPM```. Сокет обозначается буквой ```s```. 

## Компиляторы

### C / C++

> #### Компиляция C-кода с заголовками и библиотеками 
> ```bash
> gcc myprogram.c -I ../include ../lib/libsome.a -o myprogram.out
> ```

## Программное обеспечение

### IDE
> #### Установка PhpStorm
> - Download the archive and extract it to /opt/
> - Then run:
> ```bash
> /opt/PhpStorm-xxx/bin/phpstorm
> ```

### Docker
> #### Запуск служб
> ```bash
> sudo systemctl enable --now docker # run the Docker service
> 
> sudo systemctl disable --now docker.service docker.socket # stop the engine
> 
> sudo docker run hello-world # verify the installation
> ```


## bash

> ### whereis - поиск бинарников
> ```bash
> whereis <binary_name>
> ```

> ### Перемещение в начало и конец текущей строки команды
> **CTRL** + **A** - в начало строки  
> **CTRL** + **E** - в конец строки  

> ### документация - ```man```
> ```bash
> man <program>
> man ls # выводит в less документацию к утилите ls
>
> man <page> <function>
> man 3 printf # выводит документацию к функции языка C - printf()
> man 2 unlink # выводит документацию к функции языка C - unlink() 
> ```

> ### rsync - (cp + ssh)
> Копирование файлов и каталогов между узлами
> ```bash
> # передача файла с локального узла на удаленный:
> rsync /path/to/local_file.txt remote_node:path/to/remote_dir 
> ##
> # передача файла с удаленного узла на локальный:
> rsync remote_node:path/to/remote_file.txt path/to/local_dir


> ### Исследовать тип файла
> ```bash
> file text.txt
> ```

> ### Создание символической ссылки
> ```bash
> ln -s file1.txt link1
> ```


> ### Working with archives
> ```bash
> zip -r <name>.zip <dir>/ #  archive a folder 
> ```

> ### Current working directory
> ```bash
> pwd 
> ```

> ### List files
> ```bash
> ls 
> ls /path
> ls -l # detailed info (permissions, owners, type, modified date)
> ls -h # human-readable sizes 
> ls -lh # -l & -h combination
> ```

> ### Change directory
> ```bash
> cd /path
> ```

> ### Find
> ```bash
> find /dir/path -name '*.ext' # найти файлы по расширению
> find /dir/path -type d -iname '*lib*' # найти каталоги по шаблону
> find / -t type d -name home # search entire FS for directory named "home"
> find . -exec echo {} \; # runs echo for each file in current dir
> find -iname foobar # case insensitive
> find -name "foobar*" #  names starting with foobar
> find -name "*foobar" #  names ending with foobar 
> ```

> ### less - view file page by page
> ```bash
> less <filename>
> ```
> - Useful for large files (logs).
> - Use arrow keys to navigate.
> - ```/text``` to search, ```q``` to quit. 

> ### tail - посмотреть последние строчки файла
> ```bash
> tail <filename>
> ```
> - Полезно для больших файлов, например логов, чтобы увидеть последние строки.

> ### touch - create or update file
> ```bash
> touch <filename>
> ```

> ### mkdir - create directory
> ```bash
> mkdir <path1> <path2> ...
> mkdir -p <path> # create nested directories if not exist
> ```

> ### rm - remove files/directories.
> ```bash
> rm <filename> 
> rm -rf <directory>
> ```


### Processes
#### Processes are represented as files inside ```/proc/...```
> ```bash
> ps aux # list processes
> ps aux | head -n 10 # first 10 lines
> ps -eLf # show threads
> ps -ejH # process tree
> ps -aux | grep <name> # legacy way to find process by name (use pgrep)
> pgrep <name> # find PID by name
> ```

> ##### Monitoring
> ```bash
> top # realtime process monitor
> iotop # I/O monitor
> nethogs # network monitor
> ```

> ##### Killing process
> ```bash
> kill <PID> # sends SIGTERM signal ( polite stop )
> kill -9 <PID> # sends SIGKILL signal ( force stop )
> ```

> ##### File descriptors
> *Everything is a file in UNIX.*
> ```bash
> lsof -p <PID> # list descriptors of process.
> lsof -p <PID> | less # read descriptors page by page. 
> ```


### Службы
**Службы** часто называются *демонами*, это продолжительные процессы которые выполняются в фоновом режиме, например - СУБД, веб-серверы, системные службы для ОС и так далее. Эти службы обычно запускает система инициализации, например ```systemd```.

> *Управлять службами systemd можно только с правами **root** (через sudo)*

##### Управление службами с помощью systemd
 
> **Запустить службу**
> ```bash
> systemctl start <service_name>
> ```

> **Остановить службу**
> ```bash
> systemctl stop <service_name>
> ```

> **Перезапустить службу**
> ```bash
> systemctl restart <service_name>
> ```

> **Вывести текущее состояние службы**
> ```bash
> systemctl status <service_name>
> ```

> **Включение / отключение автозапуска службы, при загрузке системы**
> ```bash
> systemctl enable <service_name> # включение
> systemctl disable <service_name> # отключение
> ```
 
