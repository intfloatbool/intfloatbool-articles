# 1. Функция ```InitWindow()```

## 1.1 Начало программы

Программа начинается в ```main()```

```c
int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "raylib [models] example - loading gltf");
    // .....
}
```

Сразу попадаем отладчиком в первую функцию - ```InitWindow()```. 

Сигнатура:

```c
void InitWindow(int width, int height, const char *title)
```

Все понятно, передаем ширину, высоту окна, и текст для заголовка окна.

Отладчик ведет нас к реализации этой функции в файле: ```raylib\src\rcore.c```

Выставляются параметры для структуры ```Window```, просто передача данных.

```c
CORE.Window.screen.width = width;
CORE.Window.screen.height = height;
CORE.Window.eventWaiting = false;
CORE.Window.screenScale = MatrixIdentity();  
if ((title != NULL) && (title[0] != 0)) CORE.Window.title = title;
```
Инициализация глобального ```Input State```:

Через функцию ```memset()``` структура ```Input``` сбрасывает все свои значения полей в 0 байт. То есть, другими словами, приводится к "исходному состоянию". 

> *```memset``` работает ТОЛЬКО с POD-структурами и классами.*

```c
memset(&CORE.Input, 0, sizeof(CORE.Input));     
CORE.Input.Keyboard.exitKey = KEY_ESCAPE;
CORE.Input.Mouse.scale = (Vector2){ 1.0f, 1.0f };
CORE.Input.Mouse.cursor = MOUSE_CURSOR_ARROW;
CORE.Input.Gamepad.lastButtonPressed = GAMEPAD_BUTTON_UNKNOWN;
```