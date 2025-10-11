### 1.2.2 Расширения OpenGL

Вызов функции ```rlLoadExtensions()```, которая загружает *OpenGL Extensions* с обратным вызовом ```glfwGetProcAddress()```.

- ```glfwGetProcAddress()``` позволяет получить адрес любой функции OpenGL.

> **OpenGl Extensions**  
> Это дополнительные функции и возможности OpenGL, которые **не входят в стандарт ядра**, но **поддерживаются конкретным GPU или драйвером**.

```c
rlLoadExtensions(glfwGetProcAddress);

void rlLoadExtensions(void *loader)
{
    /* .... */
}
```
Я прошелся отладчиком внутри ```rlLoadExtensions()```, там очень много директив ```#if```, теперь посмотрим порядок выполнения на ```Win64```.

---

Загрузка количества доступных расширений, у меня 403.

```c
GLint numExt = 0;
glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
```
---

Выставление поддерживаемых расширений в OpenGL 3.3 по умолчанию.

> **VAO** (```vao```)   
> Vertex Array Object - хранит состояние атрибутов вершин (нормаль, позиция, цвет, текстурные координаты), работает с буфером вершин - VBO, что ускоряет работу, не требуя каждый кадр
заново настраивать атрибуты вершин через ```glVertexAttribPointer()```.

> **Instancing** (```instancing```)  
> Instanced Rendering - позволяет отрисовать много одинаковых объектов (один мэш VAO/VBO, одна шейдерная программа, одна привязанная текстура) за один вызов отрисовки (```drawcall```).  
> Это значительно ускоряет массовую отрисовку, например, травы, армии юнитов, частиц.

> **Non-Power-Of-Two Textures** (```texNPOT```)  
> Разрешает использование текстур произвольных размеров, не только ```2^n``` - ```128x128, 256x256, 512x512``` и так далее. Теперь можно, например, использовать текстуру размером ```257x251``` . 

> **Floating Point Textures 32bit** (```texFloat32```)  
> Позволяет использовать текстуры с компонентами ```float```, 32 бит на канал. Требуется для HDR, постэффектов и прочих сочных вещей.

> **Half-Float Textures 16bit** (```texFloat16```)  
> Позволяет использовать текстуры с компонентами ```half float```, 16 бит на канал. *Оптимальный баланс между точностью и **памятью***.

> **Depth Textures** (```texDepth```)  
> Позволяет использовать буфер глубины ```GL_DEPTH_COMPONENT``` в виде текстуры. Является основной для теней, имитации глобального освещения (SSAO).

> **Максимальная глубина Z-буфера** (```maxDepthBits```)    
> Чем больше поддерживает GPU (обычно 32), тем меньше артефактов ```z-fighting``` при отрисовке дальних объектов.

> **Anisotropic Filtering** (```texAnisoFilter```)  
> Повышает качество текстур при наклонных углах обзора.  Улучшает визуал без сильного падения FPS.

> **Texture Mirrored Clamp** (```texMirrorClamp```)  
> Позволяет "зеркально" повторять крайние пиксели, вместо обычного ```clamping```.  
> Полезно при постэффектах и текстурах окружения, чтобы избежать швов.

```c
RLGL.ExtSupported.vao = true;
RLGL.ExtSupported.instancing = true;
RLGL.ExtSupported.texNPOT = true;
RLGL.ExtSupported.texFloat32 = true;
RLGL.ExtSupported.texFloat16 = true;
RLGL.ExtSupported.texDepth = true;
RLGL.ExtSupported.maxDepthBits = 32;
RLGL.ExtSupported.texAnisoFilter = true;
RLGL.ExtSupported.texMirrorClamp = true;
```

---

Далее идет выставление глобальных данных по компрессии текстур.

У меня получились все ```true```, кроме ASTC.Вероятно, OpenGL не поддерживает ASTC на моей видеокарте NVIDIA GeForce RTX 3050  (6 GB).

В OpenGL можно хранить текстуры в сжатом виде- не в обычных RGBA пикселях, а форматах: ASTC, DXT, ETC2 и так далее.

Сжатые текстуры дают:
- Меньше потребления видеопамяти
- Быстрое чтение из VRAM, вместо долгого из CPU.
- Но иногда немного хуже качество.

Виды форматов компрессии текстур:

> **ASTC**  
> Adaptive Scalable Texture Compression
> - Работает на мобильных GPU, и на современных десктоп-картах.
> - Гибкая настройка сжатия - от очень качественного, до ультракомпактного.

> **DXT / S3TC**  
> DirectX Texture Compression
> - Поддерживается почти всеми GPU.
> - Используется в *.DDS текстурах.
> - DXT3 и DXT5 с альфой.

> **ETC2 / EAC**  
> Ericsson Texture Compression
> - Поддерживается почти всеми GPU.
> - Это кроссплатформенный формат. 
> - Стандарт для Android ```OpenGL ES 3.0``` .

```c
RLGL.ExtSupported.texCompASTC = GLAD_GL_KHR_texture_compression_astc_hdr && GLAD_GL_KHR_texture_compression_astc_ldr;
RLGL.ExtSupported.texCompDXT = GLAD_GL_EXT_texture_compression_s3tc;
RLGL.ExtSupported.texCompETC2 = GLAD_GL_ARB_ES3_compatibility;
```

---

Передача ссылки на функцию-загрузчик в глобальный объект.

```c
RLGL.loader = (rlglLoadProc)loader;
```

---

Получение доступного уровня анизотропной фильтрации, чем выше уровень, тем меньше мыла. У меня получилось значение ```16.0```.

```c
glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &RLGL.ExtSupported.maxAnisotropyLevel);
```

### 1.2.3 Выставление коллбэков для GLFW

После выхода из функции ```rlLoadExtensions()```, возвращаемся в функцию ```InitPlatform()```. 

Нас встречают далее следующие вызовы:

```c
glfwSetWindowSizeCallback(platform.handle, WindowSizeCallback); 
glfwSetWindowPosCallback(platform.handle, WindowPosCallback);
glfwSetWindowMaximizeCallback(platform.handle, WindowMaximizeCallback);
glfwSetWindowIconifyCallback(platform.handle, WindowIconifyCallback);
glfwSetWindowFocusCallback(platform.handle, WindowFocusCallback);
glfwSetDropCallback(platform.handle, WindowDropCallback);
```

Им передаются функции из RayLib, в виде обратных вызовов. Самое интересное здесь - ```WindowSizeCallback()``` , которая вызывает внутри себя ```SetupViewport()```. 

Функция ```SetupViewPort()``` сбрасывает матрицы ```PROJECTION``` и ```MODEL_VIEW```.

То есть, при смене размера окна, у нас идет сброс ```MVP``` матриц для одного кадра.

```c
void SetupViewport(int width, int height)
{
    CORE.Window.render.width = width;
    CORE.Window.render.height = height;

    rlViewport(CORE.Window.renderOffset.x/2, CORE.Window.renderOffset.y/2, CORE.Window.render.width, CORE.Window.render.height);

    rlMatrixMode(RL_PROJECTION);
    rlLoadIdentity();

    rlOrtho(0, CORE.Window.render.width, CORE.Window.render.height, 0, 0.0f, 1.0f);

    rlMatrixMode(RL_MODELVIEW);       
    rlLoadIdentity(); 
}
```
 
---

Далее идут коллбэки для ввода.

```c
 glfwSetKeyCallback(platform.handle, KeyCallback);
 glfwSetCharCallback(platform.handle, CharCallback);
 glfwSetMouseButtonCallback(platform.handle, MouseButtonCallback);
 glfwSetCursorPosCallback(platform.handle, MouseCursorPosCallback);   
 glfwSetScrollCallback(platform.handle, MouseScrollCallback);
 glfwSetCursorEnterCallback(platform.handle, CursorEnterCallback);
 glfwSetJoystickCallback(JoystickCallback);
```
---
### 1.2.4 Конец функции ```InitPlatform()```

Инициализация таймера, там выставляется текущее время в секундах, чтобы таймер начал работать.

```c
InitTimer();

void InitTimer(void)
{
    /* .... */
    CORE.Time.previous = GetTime();
}
```

---

Получение рабочей директории, откуда будут браться ресурсы, в процессе дебага это примерно такой путь:
```C:\MY_SRC\raylib\projects\VS2022\build\models_loading_gltf\bin\x64\Debug```

От туда будут браться ресурсы для игры (модельки, текстурки и так далее).

```c
CORE.Storage.basePath = GetWorkingDirectory();
```

Конец функции ```InitPlatform()```.

---
