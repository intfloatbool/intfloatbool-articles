
## 1.2 Функция ```InitPlatform()```

Далее вызывается функция ```InitPlatform()```

```c
/* .... */
int result = InitPlatform();
/* .... */
```

> *Так как текущая версия делается для Windows, реализация функции лежит в ```src/platforms/rcore_desktop_glfw.c```, для Android'а например, реализация в файле: ```src/platforms/rcore_android.c```*.

### 1.2.1 GLFW

Реализация функции находится в ```src/platforms/rcore_desktop_glfw.c```. Что мы видим - для десктопа используется **GLFW**.

> **GLFW** - библиотека для упрощения использования OpenGL.
> Имеет следующие возможности:
> - Создание окон.
> - Управление контекстами, которые нужны для передачи графическому API.
> - Обработка пользовательского ввода.

Рассмотрим ```InitPlatform()```  
```c
int InitPlatform(void)
{
    glfwSetErrorCallback(ErrorCallback);

    const GLFWallocator allocator = {
        .allocate = AllocateWrapper,
        .deallocate = DeallocateWrapper,
        .reallocate = ReallocateWrapper,
        .user = NULL, 
    };
    /* .... */
}
```

---

Вызывается функция ```glfwSetErrorCallback()```, название которой говорит само за себя - функция обработчик ошибок glfw.

Передается аргументом функция ```ErrorCallback```, которая переправляет ошибку собственному логгеру RayLib (```TRACELOG()```).

```c
static void ErrorCallback(int error, const char *description)
{
    TRACELOG(LOG_WARNING, "GLFW: Error: %i Description: %s", error, description);
}
```

---

Дальше создаётся структура ```GLFWallocator```. Она позволяет переопределить функции работы с памятью для клиента. RayLib делает такие определения работы с памятью у ```glfw```: 

- ```GLFWallocator.allocate = AllocateWrapper``` - ```AllocateWrapper``` это просто перенаправление на функцию ```calloc()``` из стандартной библиотеки, которая выделяет память и инициализирует ее нулевыми значениями, в отличие от ```malloc()```, которая не инициализирует память, а создает мусор, который нужно заполнить.
- ```GLFWallocator.deallocate = DeallocateWrapper``` - ```DeallocateWrapper``` перенаправление на стандартный ```free()```.
- ```GLFWallocator.reallocate = ReallocateWrapper``` - перенаправление на стандартный ```realloc()```, который меняет размер уже выделенного блока памяти.

Эта структура передается ```glfw``` через функцию ```glfwInitAllocator(&allocator)``` , таким образом glfw выставлен пользовательский аллокатор памяти.

```c
const GLFWallocator allocator = {
    .allocate = AllocateWrapper,
    .deallocate = DeallocateWrapper,
    .reallocate = ReallocateWrapper,
    .user = NULL, 
};
```

---

После этого вызывается ```glfwInit()```, которая инициализирует библиотеку ```glfw``` перед дальнейшим использованием ее функций.
```c
int result = glfwInit();
```

---

Затем идет вызов ```glfwDefaultWindowHints()```. Эта функция сбрасывает все ```hint'ы``` до уровня -  **по умолчанию**. 

```hints``` - это набор параметров, которые задаются, чтобы описать желаемые характеристики будущего окна и графического контекста. 

```c
glfwDefaultWindowHints();
```

---

Выставляется хинт - ```GLFW_AUTO_ICONIFY``` на ```0``` значение (```GLFW_FALSE```). Этот хинт влияет на поведение **полноэкранного окна** (full-screen), **когда окно теряет фокус** (кнопки - alt-tab, win), должно ли оно автоматически сворачиваться. По дефолту стоит = 1 , а тут выставляется 0, чтобы окно не сворачивалось. 

> *На оконные режимы (window-mode) не влияет этот параметр*.

```c
glfwWindowHint(GLFW_AUTO_ICONIFY, 0);
```

---

Затем идет получение флагов окна, они не выставлялись нигде ранее в ```CORE.Window.flags```, поэтому там значение 0. Так как флагов нету, то все условия с флагами, куда не попадаем, пропускаем.

```c
unsigned int requestedWindowFlags = CORE.Window.flags;
```

---

Так как изначально окно от ```glfw``` имеет скрытое состояние, то явно выставляется чтобы оно было видимым, через хинт:

```c
glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
```

---

Выставление хинта - ```GLFW_DECORATED```. По умолчанию он и так ```GLFW_TRUE```, но тут выставление идет явное. Если хинт ```GLFW_DECORATED``` выставлен на ```GLFW_TRUE```, то у окна будет рамка и кнопки управления (крестик, свернуть). Если стоит на ```GLFW_FALSE```, то рамок нет и кнопок управления тоже.

```c
glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
```

---

Дальше идет хинт, который позволяет или запрещает изменять размер окна, выставляется в запрет.

```c
glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
```

---

Хинт **фокусировки окна при его создании**. Если стоит ```GLFW_FALSE``` то при создании окна, оно не будет сразу же активным (т.е. не принимает ввод пользователя), пока мы нажмем в его область мышкой. А с флагом ```GLFW_TRUE``` окно активно сразу же, при создании.

```c
glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
```

---

Хинт- ```GLFW_FLOATING```, если он ```true```,то окно  **всегда поверх отображается поверх остальных окон**. Чтобы окно вело себя как все остальные, и мы могли нормально им манипулировать, выставляется ```false```.

```c
glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
```

---

Далее уже идет хинт для рендера - ```GLFW_TRANSPARENT_FRAMEBUFFER```, может ли окно быть прозрачным и через него будет виден рабочий стол (если используешь шейдеры с прозрачностью) ? Очевидно нет, для примера этой программы это не нужно.

```c
glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);
```

---

Еще один хинт для рендера - ```GLFW_SCALE_FRAMEBUFFER```.

Будет ли **framebuffer** масштабироваться вместе с **DPI** или нет (размер в пикселях остается нетронутым). 

Например, экран Retina с масштабом 200%, а наш размер окна - 800x600 , если включен этот хинт, то наше окно станет размером 1600x1200.

> **framebuffer** (буфер кадра) - *то, куда отрисовывает в пикселях графический API (например OpenGL).*

> **DPI** - *количество точек на дюйм. Например, экран Retina с масштабом 200%*.

```c
glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE);
```

---

Хинт ```GLFW_SCALE_TO_MONITOR``` - похожая история с ```GLFW_SCALE_FRAMEBUFFER```, но влияет на **размер окна** в зависимости от DPI. 

Применяется в комбинировании с ```GLFW_SCALE_FRAMEBUFFER``` , чтобы буфер кадра и размер окна масштабировались четко по DPI.

```c
glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_FALSE);
```

---

Хинт ```GLFW_MOUSE_PASSTHROUGH``` делает окно прозрачным для кликов, как будто окна нет. Например, если у вас за окном есть другое окно - то фокус мышки направится в него. Конечно же это не тот флаг, что нужен в игровой программе, ибо нам нужен ввод из мыши, поэтому он ```GLFW_FALSE```.

```c
glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_FALSE);
```

---

Далее попадаю в блок кода, который определяет мою версию OpenGL ```RL_OPENGL_33```.

```c
/* .... */
else if (rlGetVersion() == RL_OPENGL_33)
/* .... */
```

Выставляются соответствующие хинты для этой версии: 

```c
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
```

---

Затем выставляется **профиль OpenGL** с набором доступных функций.

```GLFW_OPENGL_CORE_PROFILE``` - это современный OpenGL без legacy-функций , только версия 3.3 и выше!

```c
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
```

И отключается профиль с legacy-фукнциями для совместимости. То есть приложение работает только при наличии OpenGL 3.3 и выше.

```c
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_FALSE);
```

---

Функция ```glfwSetJoystickCallback()```, регистрирует обратный вызов (callback), который GLFW вызывает, когда устройство ввода (геймпад/джойстик) подключается или отключается. Тут он не нужен и ему явно указывается ```NULL```.

```c
glfwSetJoystickCallback(NULL);
```
---

Далее идет условное разделение на инициализацию структуры ```GLFWmonitor```, которая представляет *физический* монитор устройства. 

```c
GLFWmonitor *monitor = NULL;
if (CORE.Window.fullscreen)
{
    /* .... */
}
else 
{
    /* попадаем сюда */
    /* .... */
}
```

Так как приложение работает в оконном режиме, то попадаем в блок ```else```.

---
Создается bool-переменная для проверки full-screen это или нет, очевидно, она будет иметь значение ```false```.

```c
bool requestWindowedFullscreen = (CORE.Window.screen.height == 0) && (CORE.Window.screen.width == 0);
```
---

Размер окна выводится в переменные, со значениями ```800``` и ```450``` соответственно.

```c
int creationWidth = CORE.Window.screen.width != 0 ? CORE.Window.screen.width : 1;
int creationHeight = CORE.Window.screen.height != 0 ? CORE.Window.screen.height : 1;
```
--- 
Далее создается окно GLFW.

```c
 platform.handle = glfwCreateWindow(creationWidth, creationHeight, (CORE.Window.title != 0)? CORE.Window.title : " ", NULL, NULL);
```
--- 

Идет получение текущего активного монитора.

```c
int monitorCount = 0;
int monitorIndex = GetCurrentMonitor();
GLFWmonitor **monitors = glfwGetMonitors(&monitorCount);

if (monitorIndex < monitorCount)
{
    monitor = monitors[monitorIndex];
    SetDimensionsFromMonitor(monitor);
    /* ... */
}
```

---

Выставляются значения ширины и высоты для рендера ```800``` и ```450```.

```c
CORE.Window.render.width = CORE.Window.screen.width;
CORE.Window.render.height = CORE.Window.screen.height;
```

---

Выставляется текущий объект контекста для OpenGL. Передается ```platform.handle``` типа ```GLFWindow*```.  Контекст для OpenGL - это место, где хранится **всё состояние OpenGL**. Соответственно, каждое окно имеет свой контекст.

```c
glfwMakeContextCurrent(platform.handle);
```
---
Выставляется флаг для состояния ```RayLib``` , что ```ready = true```.


```c
CORE.Window.ready = true;
```

---

Отключается и включается **Vsync**, через функцию ```glfwSwapInterval()```.

> Включение **Vsync** - это принудительное ограничение FPS, ограниченная герцовкой монитора. Предотвращает появления **разрывов при рендере**  (screen tearing). Разрыв проявляется когда кадр, делится на какие-то области, которые смещаются относительно всей картинки, например одна часть головы на месте, вторая часть головы немного сдвинута.

> Отключение **Vsync** - FPS не ограничен монитором (более высокая производительность), но есть возможность словить разрыв кадра.

> Функция ```glfwSwapInterval()``` устанавливает сколько кадров OpenGL должен ждать, чтобы выполнить ```glSwapBuffers()```.


```c
/* Значение 0 - выключен VSync, 1 - включен */
glfwSwapInterval(0); 
```

*То есть, буквально, Vsync = отложение рендера на 1 кадр.*

---

Далее идет создание переменных для ширины и высоты **буфера кадров** , ```800``` и ```450``` соответственно.

```c
int fbWidth = CORE.Window.screen.width;
int fbHeight = CORE.Window.screen.height;
```

и передача в глобальный объект для ```RayLib```

```c
CORE.Window.render.width = fbWidth;
CORE.Window.render.height = fbHeight;
CORE.Window.currentFbo.width = fbWidth;
CORE.Window.currentFbo.height = fbHeight;
```

---

Централизация окна по центру экрана.

```c
int monitorX = 0;
int monitorY = 0;
int monitorWidth = 0;
int monitorHeight = 0;
glfwGetMonitorWorkarea(monitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);

int posX = monitorX + (monitorWidth - (int)CORE.Window.render.width)/2;
int posY = monitorY + (monitorHeight - (int)CORE.Window.render.height)/2;

SetWindowPosition(posX, posY);

void SetWindowPosition(int x, int y)
{
    CORE.Window.position.x = x;
    CORE.Window.position.y = y;
    glfwSetWindowPos(platform.handle, x, y);
}
```

---

Обработка флагов, для вызова соответствующих функций GLFW. Флагов нет, поэтому метод ничего не делает.

```c
SetWindowState(requestedWindowFlags);
```
---
