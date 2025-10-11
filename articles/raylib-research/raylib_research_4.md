
# 2. Продолжение работы функции ```main()```

Выставляются значения камеры, чтобы обозначить ее позицию и направление взгляда в нужную точку.

```c
Camera camera = { 0 };
camera.position = (Vector3){ 6.0f, 6.0f, 6.0f }; // позиция
camera.target = (Vector3){ 0.0f, 2.0f, 0.0f }; // точка куда смотрит камера
camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; // Указание где у камеры вверх, когда она смотрит на таргет (дефолт 1 по Y).
camera.fovy = 45.0f; // FOV по Y
camera.projection = CAMERA_PERSPECTIVE; // Вид проекции
```

--- 

## 2.1 Загрузка модели

### 2.1.1 Функция ```LoadModel()```
Происходит вызов ```LoadModel()```, стоит в него углубиться.

```c
Model model = LoadModel("resources/models/gltf/robot.glb");
```
---

Внутри создается объект ```Model```

```c
Model model = { 0 };
```

```Model``` имеет следующую структуру:

- ```Matrix transform``` - локальная матрица модели (позиция, поворот, масштаб)
- ```Mesh* meshes``` / ```int meshCount``` - массив мэшей и их количество.
- ```Material* materials``` / ```int materialsCount``` - массив материалов и их количество.
- ```int* meshMaterial``` - номер материала, для каждого меша.
- ```BoneInfo* bones``` / ```int boneCount``` - массив костей и их количество.
- ```Transform* bindPose``` - базовая поза костей.

---

Дальше идут функции загрузки, по расширениям модели, в примере это ```*.GLB```:

```c
 if (IsFileExtension(fileName, ".gltf") || IsFileExtension(fileName, ".glb")) model = LoadGLTF(fileName);
```

Соответственно вызывается функция ```LoadGLTF()```.

Там происходит распарсинг файла ```robot.glb```, собираются все данные для ```Model```:
- мэши
- материалы (+ текстуры)
- анимации (кости)
- локальная матрица модели

---

# 3. Продолжение работы функции ```main()```

Далее загружаются конкретные анимации из ```*.GLB``` файла.

Там заполняются анимации:

- берутся кости для анимации
- данные перемещения костей
- продолжительность

```c
 ModelAnimation *modelAnimations = LoadModelAnimations("resources/models/gltf/robot.glb", &animsCount);
```

--- 

Выставляется целевая частота кадров.

```c
SetTargetFPS(60); 
```

---

## 3.1 Игровой цикл

Создается цикл ```while```, пока окно не закрыто:

```c
while (!WindowShouldClose())
{
/* .... */
}

```

### 3.1.1 Обновление состояния

В этом простом примере от RayLib, обновление состояния (ввод, позиций, анимации) довольно простое:

- ```UpdateCamera()``` Камера крутится вокруг цели автоматически.
- ```IsMouseButtonPressed()``` Проверки нажатия кнопок мыши, чтобы переключать анимации.
- ```UpdateModelAnimation()``` Обновление текущей анимации.

```c
UpdateCamera(&camera, CAMERA_ORBITAL);

if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) animIndex = (animIndex + 1)%animsCount;
else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) animIndex = (animIndex + animsCount - 1)%animsCount;

ModelAnimation anim = modelAnimations[animIndex];
animCurrentFrame = (animCurrentFrame + 1)%anim.frameCount;
UpdateModelAnimation(model, anim, animCurrentFrame);
```

---

### 3.1.2 Отрисовка

Все инициализации с рендером OpenGL проделаны, далее вызываются функции в декларативном стиле, для отрисовки.

- Блок отрисовки должен быть между функциями ```BeginDrawing()``` и ```EndDrawing()```.

- Вначале весь буфер кадра очищается до белых пикселей ```ClearBackground()```.

- Работа с 3D идет между функциями ```BeginMode3D()``` и ```EndMode3D()```.

- В блоке модели, создается модель в позиций, и с множителем белого цвета ```tint```
    - ```tint``` (оттенок) - это **цветовой множитель**, который применяется к цвету пикселя,  после того как модель уже отрендерена со своей текстурой и материалом. Таким образом, можно немного разбавлять краской модель, для разных нужд. 


```tint = WHITE```:

![](/assets/images/tint_white.png)

```tint = BLACK```:

![](/assets/images/tint_black.png)

```tint = RED```:

![](/assets/images/tint_red.png)


- Идет отрисовка сетки, через функцию ```DrawGrid()```, ```10х10``` с расстоянием ```1.0```.

- Отрисовка 2D происходит вне блока ```Mode3D```, идет рендер текста, через ```DrawText()```.

```c
BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
        DrawModel(model, position, 1.0f, WHITE);
        DrawGrid(10, 1.0f);
    EndMode3D();

    DrawText("Use the LEFT/RIGHT mouse buttons to switch animation", 10, 10, 20, GRAY);
    DrawText(TextFormat("Animation: %s", anim.name), 10, GetScreenHeight() - 20, 10, DARKGRAY);

EndDrawing();
```
---
### 3.1.3 Конец игрового цикла

Когда окно закрыто, идет стандартное высвобождение ресурсов.

```c
UnloadModel(model);
CloseWindow();
```