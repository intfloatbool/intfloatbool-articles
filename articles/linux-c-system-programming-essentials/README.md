# Быстрое введение в системное программирование на языке C в Linux

## Требования

### Операционная система
Примеры протестированы на следующих ОС: 
- Linux Fedora 42 (Adams)
- WSL Ubuntu 24.04 LTS

### Инструменты

#### Редактор кода
Можете использовать любой редактор кода, который вам нравится- блокнот, Visual Studio Code и так далее, подойдет любой. 

> #### Компилятор
> Для примеров требуется компилятор **GCC** и сборщик **Make**
> - Установка в Fedora:
> ```bash
> sudo dnf group install development-tools
> ```
> - Установка в Ubuntu:
> ```bash
> sudo apt install build-essential
> ```

#### Отладочные инструменты (опционально)
Для поиска багов и неисправностей, не обойтись без специальных инструментов.
1. **GDB** - отладчик, с которым мы можем пройтись по исполнению программы шаг-за-шагом, увидеть значения переменных и даже их изменять, таким образом можно ускорить и упростить процесс нахождения причин неисправностей в нашем ПО.

2. **Valgrind** - инструмент для помощи в нахождении *утечек памяти*, так как мы работаем с памятью вручную, это одна из самых распространенных проблем и найти утечки без специального инструмента может быть очень сложно.

> **Установка в Fedora**
> ```bash
> sudo dnf install gdb valgrind
> ```

> **Установка в Ubuntu**
> ```bash
> sudo apt install gdb valgrind
> ```

## Содержание
1. [Ввод/вывод](linux-c-system-programming-essentials-p0-io.md)
2. [Переменные окружения](linux-c-system-programming-essentials-p1-env-vars.md)
2. [Компиляция и библиотеки](linux-c-system-programming-essentials-p2-compilation.md)
3. [Утилита make](linux-c-system-programming-essentials-p3-make-util.md)
4. [Системные вызовы](linux-c-system-programming-essentials-p4-syscalls)
5. Процессы
    1. [fork() и execl()](linux-c-system-programming-essentials-p5-processes-p0.md)
    2. [system(), Фоновые процессы и службы](linux-c-system-programming-essentials-p6-processes-p1.md)
    3. [Коммуникация между процессами (IPC)](linux-c-system-programming-essentials-p7-processes-p2.md)
6. [Потоки](linux-c-system-programming-essentials-p8-threads.md)
