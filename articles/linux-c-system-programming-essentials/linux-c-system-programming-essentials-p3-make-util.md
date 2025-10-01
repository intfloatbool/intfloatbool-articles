## 8. Утилита make для автоматизации сборки

### Что делаем
Используем код из 6 части **Создание собственной библиотеки**.
Компилируем библиотеку, затем компилируем клиент для работы с ней.
  
Но на этот раз **выполнять операции сборки будет утилита ```make```**, в которой мы пропишем необходимые команды. 

**Make** позволяет автоматизировать процесс сборки, где мы работаем с командами (targets), которые сами укажем.  

Таким образом, вместо того, чтобы постоянно вручную писать команды сборки вроде:
```bash
gcc my_file.c -lxxx ....
```

Это будет делать за нас Make, и все ограничится простым вызовом, в месте, где находится файл ```Makefile```:
```bash
make
```

### Что внутри
- Makefile сборки библиотеки ```custom_logger```.
- Makefile сборки клиента ```client_custom_logger```.

### Исходный код
```public/code_sources/linux-c-system-programming-essentials/7-makefile/custom_logger/Makefile```  
[Ссылка](./code_sources/linux-c-system-programming-essentials/7-makefile/custom_logger/Makefile)

```public/code_sources/linux-c-system-programming-essentials/7-makefile/client_custom_logger/Makefile```  
[Ссылка](./code_sources/linux-c-system-programming-essentials/7-makefile/client_custom_logger/Makefile)

### Пояснения

#### Сборка библиотеки ```custom_logger/Makefile```

Создается файл ```Makefile``` внутри папки с проектом - ```custom_logger```:

```custom_logger``` это библиотека, которая будет использоваться программой ```client_custom_logger```, поэтому ```Makefile``` для нее заточен под сборку именно библиотеки.  

Основная особенность ```make``` это **поддержка переменных**, мы можем в начале файла указать наши переменные, которые затем будут заменять своим содержимым те места, где они используются.

**Объявление переменной идет так:**
```lang-makefile 
VARIABLE := value
```

**Обращение затем к этой переменной, с целью получить ее значение делается так:**
```lang-makefile
 $(VARIABLE)
```

Затем из этих переменных складываются команды (targets).

**Разберем поэтапно каждую переменную:**


> ```CC``` - указываем компилятор, который будет использоваться, в нашем случае это ```gcc```
> ```lang-makefile
> CC := gcc
> ``` 

> ```CFLAGS``` -  флаги компилятора, для конфигурации процесса компиляции
> ```lang-makefile
> CFLAGS := -O2 -Wall -Wextra -pedantic -fPIC
> ```
> > - ```-O2``` - оптимизация на уровне 2.
> > - ```-Wall``` - выводим все предупреждения, если они появятся.
> > - ```-Wextra``` - выводим дополнительные предупреждения, если они появятся.
> > - ```-pedantic``` - более строгий анализ кода.
> > - ```-fPIC``` - генерация позиционно-независимого кода, важно для сборки библиотек.

> ```SRCS``` - файлы исходного кода проекта
> ```lang-makefile
> SRCS := $(wildcard src/*.c)
> ``` 
> Ищутся все файлы с расширением ```.c``` в папке ```src``` и подставляются в эту переменную.

> ```BIN``` и ```LIB``` пути  
> Просто названия папок , где будут лежать результаты сборок.
> ```lang-makefile
> BIN := bin
> LIB := lib
> ```
  
> ```NAME``` - имя выходного файла
> ```lang-makefile
> NAME := customlogger
> ```

> ```LIB_PATH``` - итоговый путь где будет лежать библиотека ```.so```
> ```lang-makefile
> LIB_PATH := $(PWD)/$(LIB)
> ```
> Обратите внимание, что путь будет такой: **рабочая директория процесса оболочки ```PWD```** (т.е. откуда запускается ```make```) + ```lib```.

> ```OBJS``` - имена объектных файлов
> ```lang-makefile
> OBJS := $(SRCS:src/%.c=$(BIN)/%.o)
> ```
> Здесь просто идет копирование имен исходных файлов с форматом ```.c``` на формат ```.o```. Это просто имена, так как на каждый файл исходного кода будет свой объектный файл (1 к 1) , например, ```src/custom_logger.c``` станет ```bin/custom_logger.o``` и так далее.



**Теперь объявим цели сборки (targets)**

```lang-makefile
all: build_shared

build_object:
    $(CC) $(CFLAGS) -c $(SRCS) -o $(OBJS) 

build_shared: build_object
    $(CC) -shared -Wl,-soname,lib$(NAME).so -o $(LIB)/lib$(NAME).so $(OBJS)

setenv:
    @echo 'export LD_LIBRARY_PATH="$(LIB_PATH):$$LD_LIBRARY_PATH"'	

clean:
    rm -r $(BIN)/* && rm -r $(LIB)/*

.PHONY: all build_object build_shared setenv clean
```

Пойдем по пути простого вызова из терминала команды 

```bash
make
```

которая запустит дефолтный таргет. 

> **default target в make**
> > Это первый указанный таргет по списку, можно указать дефолтный таргет самому, задав специальную переменную:
> > ```lang-makefile
> > .DEFAULT-GOAL := all
> > ```

1. **```all```** - в нашем случае просто переправляет к таргету ```build_shared```.
2. **```build_shared```** - сборка библиотеки. Сперва вызывает таргет ```build_object```, и делает следующее, подставляя вместо переменных значения:  
```bash
gcc -shared -Wl,-soname,libcustomlogger.so -o lib/libcustomlogger.so bin/custom_logger.o
```
3. **```build_object```** - сборка объектных файлов:  
```bash
gcc -O2 -Wall -Wextra -pedantic -fPIC -c src/custom_logger.c -o bin/custom_logger.o
```

Итого порядок операций такой:  

1. **```build_object```**
2. **```build_shared```**

Сперва строим объектные файлы, а затем из них собираем библиотеку.

#### Сборка клиента ```client_custom_logger/Makefile```

> **Важно**
> > Копия библиотеки ```libcustomlogger.so```, собранная раньше, должна быть в папке ```client_custom_logger/lib/libcustomlogger.so``` !

**```Makefile```** выглядит просто, но есть интересные флаги для компоновщика:

```lang-makefile
OUT_NAME := client_custom_logger.out
LINKER_BUILD_TIME := -Llib -lcustomlogger
LINKER_RUNTIME_PARAMS := -Wl,-rpath,'$$ORIGIN/lib'


all:
    $(CC) $(SRCS) $(LINKER_BUILD_TIME) $(LINKER_RUNTIME_PARAMS) -o $(OUT_NAME)

.PHONY: all

```


1. **```LINKER_BUILD_TIME```** указывает флаги для компоновщика, которые используются во время *сборки*.
> ```-Llib```
> > указывает на папку где размещена библиотека, в нашем случае это папка ```lib```.

> ```-lcustomlogger```
> > указывает на имя библиотеки, в этой папке. Обратим внимание что в имени файла ```libcustomlogger.so```не учитывается префикс ```lib``` и расширение ```.so``` , поэтому имя для компоновщика из ```libcustomlogger.so``` превращается в ```customlogger```.

2. **```LINKER_RUNTIME_PARAMS```** передает опции времени выполнения программы компоновщику
> ```-Wl,<опции>```
> > Опция - ```-rpath,'$$ORIGIN/lib'``` прописывает ```RUNPATH``` в исполняемом ```ELF``` файле. 
> > Эта опция используется для передачи пути библиотекам, которые нужны приложению
> > В нашем случае это такой путь ```$$ORIGIN/lib```.
> > 
> > Переменная ```$ORIGIN``` это путь директории, где будет находится исполняемый файл.
> > Итого будет путь такой ```<путь_к_программе>\lib```
> > В директории ```lib``` размещена наша библиотека ```libcustomlogger.so``` .
> >

#### Резюме
Утилита ```make``` позволяет нам писать скрипты сборки в файлах ```Makefile```, настраивать связи этапов сборок, гарантируя порядок. Возможность использования переменных упрощает написание скриптов сборки. Для сборки проекта большего чем вывод ```Hello World !``` - это незаменимый инструмент.

**[Следующая статья](linux-c-system-programming-essentials-p4-syscalls.md)**