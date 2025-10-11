## 1.3 Продолжение функции ```InitWindow()```

Идёт вызов ```rlglInit()```, рассмотрим его внимательнее.

```c
rlglInit(CORE.Window.currentFbo.width, CORE.Window.currentFbo.height);
```

---

### 1.3.1 Функция ```rlglInit()```

Создается один пиксель формата RGBA.
Затем присваивается результат ```rlLoadTexute()``` как ```defaultTextureId```. То есть создается текстура ```1x1```, один белый пиксель.
```c
unsigned char pixels[4] = { 255, 255, 255, 255 };   
RLGL.State.defaultTextureId = rlLoadTexture(pixels, 1, 1, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
RLGL.State.currentTextureId = RLGL.State.defaultTextureId;
```

---

Внутри функции ```rlLoadTexture()``` происходит следующее:

- Идет **проверка доступного формата компрессии** (ASTC/ETC2/DXT), если формат не поддерживается - функция прерывается.
- Создается и привязывается **новый ID текстуры в OpenGL**.
- Через функцию OpenGL ```glTexImage2D()``` загружаются данные пикселей (у нас один пиксель) внутрь OpenGL.

Эта стандартная текстура-пустышка (1х1 белый) загружается, чтобы OpenGL всегда имел хотя-бы одну текстуру, даже если ничего не загружено. 

Это своего рода "FallBack" и защита от крашей, когда нет текстур вообще.

--- 

Вызов ```rlLoadShaderDefault()```, где происходит объявление и загрузка простых шейдеров по-умолчанию.

Эти шейдеры компилируются и загружаются в OpenGL.

После компиляции фрагментный и вершинный шейдер превращаются в одну - **шейдерную программу** , она загружается в OpenGL и возвращает ID.

```c
RLGL.State.currentShaderId = RLGL.State.defaultShaderId;
```

**Вершинный шейдер**:

```glsl
#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;
out vec2 fragTexCoord;
out vec4 fragColor;
```

**Фрагментный шейдер**: 
```glsl
#version 330     
in vec2 fragTexCoord;             
in vec4 fragColor;                
out vec4 finalColor;               
uniform sampler2D texture0;        
uniform vec4 colDiffuse;           
void main()                        
{                                 
    vec4 texelColor = texture(texture0, fragTexCoord);
    finalColor = texelColor*colDiffuse*fragColor;  
}              
```

---

Далее создается дефолтный **RenderBatch**.

> **RenderBatch**  
> Это *группа* примитивов (вершин/треугольников), которые можно  
> отрисовать *одним вызовом рендера* (glDrawArrays, glDrawElements). То есть батчингом называется - группировка данных для одного вызова отрисовки. 
>
> Это помогает: ускорению отрисовки, сократив переключение контекста.
>  
> Пример: 1000 спрайтов с одной текстурой = 1 батч, 1000 спрайтов с 1000 разных текстур = 1000 батчей, итог = лаги.
>

В RayLib объект RenderBatch состоит из следующего:

```c
typedef struct rlRenderBatch {
    int bufferCount; //Количество VBO в батче
    int currentBuffer; //Управление переключением VBO
    rlVertexBuffer *vertexBuffer; //Где хранятся вершины текущего батча
    rlDrawCall *draws; //Логические куски отрисовки, сгрупированные по текстурам
    int drawCounter; //Сколько кусков отрисовке в батче
    float currentDepth; //Z-Глубина следующего элемента
} rlRenderBatch;
```

Жизненный цикл ```RenderBatch```:

1. ```rlLoadRenderBatch()``` - создается VAO/VBO
2. **Накопление** - каждый вызов ```DrawTriangle()``` или ```DrawTexture``` накопляет вершины в ```vertexBuffer[currentBuffer]``` , если текстура изменилась - создается новый ```rlDrawCall```.
3. **Отрисовка** - когда буфер заполнен (или вызван ```rlDrawRenderBatchActive()```), идет проход по всем ```draws[]```и вызывается нужное количество ```glDrawElements()```.
4. **Сброс** - ```drawCounter = 0``` , ```currentDepth = 0```, ```currentBuffer``` переключается.

То есть, чтобы батчинг проходил эффективно, нужно иметь одинаковые параметры:
- Меш (vertex layout, шейдер input)
- Текстура (textureId)
- Шейдер (материал)
- OpenGL-состояния (blend mode, depth test, cull face)

```c
RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_NORMAL] = RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL;
RLGL.defaultBatch = rlLoadRenderBatch(RL_DEFAULT_BATCH_BUFFERS, RL_DEFAULT_BATCH_BUFFER_ELEMENTS);
RLGL.State.currentShaderLocs[RL_SHADER_LOC_VERTEX_NORMAL] = -1;
RLGL.currentBatch = &RLGL.defaultBatch;
```

---

Далее идет инициализация матриц, сперва единичной матрицой (пустышкой).

- ```modelview``` - отвечает за мир (камеру, и позицию объектов).
- ```projection``` - задает перспективу / ортографию.
- ```transform``` - внутренний множитель для RayLib, чтобы переключаться с 2D на 3D.

```c
RLGL.State.transform = rlMatrixIdentity();
RLGL.State.projection = rlMatrixIdentity();
RLGL.State.modelview = rlMatrixIdentity();
RLGL.State.currentMatrix = &RLGL.State.modelview;
```
---
Далее идет тест глубины (z-значения у пикселя).

```glDepthFunc(GL_LEQUAL)``` - задает правило *сравнения глубин*, то есть решает какой пиксель спереди. Значение ```GL_LEQUAL``` задает **стандартное поведение**, когда ближние объекты перекрывают дальние.

```glDisable(GL_DEPTH_TEST)``` - выключает сравнение глубины, на этапе старте приложения. Он включится дальше, как только пойдет 3D рендер.


```c
glDepthFunc(GL_LEQUAL); 
glDisable(GL_DEPTH_TEST);
```
---

Включается функция **смешивания цветов (blending)**.
Здесь задается использование альфа-канала у RGBA текстур, то есть работа с прозрачностью пикселя. Без включенного blend, прозрачные пиксели будут просто квадратами.

```c
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
glEnable(GL_BLEND);
```

---

Инициализация куллинга (Culling).

Здесь включается, так называемый, "Back-Face Culling" , на стороне GPU. Просто из рендера **уходят те полигоны, которые не видны** - обратные части полигона (внутренности модели) и те, которые повернуты против (Counter Clockwise) часовой стрелки. 

Это стандартный куллинг на стороне GPU.

> *Не путать с **Frustrum Culling**, который происходит на стороне CPU, чтобы не передавать GPU объекты вне камеры.*

```c
glCullFace(GL_BACK);
glFrontFace(GL_CCW);
glEnable(GL_CULL_FACE);   
```

---

Включается поддержка для "бесшовных" ```cubemap```, то есть кубических текстур, которые используются для:
- отражений
- скайбоксов
- окружений

С флагом ```GL_TEXTURE_CUBE_MAP_SEAMLESS``` переход между гранями куба, **становится линейным** и переход текстур становится гладким, без разрывов и других неприятных эффектов.

```c
glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); 
```

--- 

Далее идет установка ширины и высоты для framebuffer (буфер кадра) , того места, куда будет рендерить OpenGL. ```800x450``` соответственно.

```c
RLGL.State.framebufferWidth = width;
RLGL.State.framebufferHeight = height;
```

---

В конце идет очистка буферов рендеринга OpenGL.

- ```glClearColor()``` - очистка **Color Buffer** , каждого пикселя (в данном случае заполнение его черными пикселями RGBA - ```[0,0,0,1]```).

- ```glClearDepth()``` - очистка **Depth Buffer (Z-Buffer)**, насколько далеки пиксели от камеры, ```0.0``` - близко к камере, ```1.0``` - далеко от камеры..

- ```glClear()``` - очищает одновременно два буфера, применяются значения из предыдущих вызовов:
    - ```GL_COLOR_BUFFER_BIT``` - цветовой
    - ```GL_DEPTH_BUFFER_BIT``` - глубины
    

```c
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
glClearDepth(1.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

## 1.4 Продолжение функции ```InitWindow()```

После работы функции ```rlglInit()``` идет выставление поля что GPU готово к работе:

```c
isGpuReady = true;
```
---

Далее идет вызов ```SetupViewport()```, которая сбрасывает матрицы. Подробнее о ней смотрите выше, в разделе GLFW.

```c
SetupViewport(CORE.Window.currentFbo.width, CORE.Window.currentFbo.height);
```

---

Загружается стандартный шрифт для текста. Его реализация находится в: ```raylib\src\rtext.c```

```c
LoadFontDefault();
```

---

Выставляются глобальные переменные:
- Счетчик кадров
- Должно ли быть закрыто окно

То есть, это их изначальное состояние, как только окно прошло инициализацию.

```c
CORE.Time.frameCounter = 0;
CORE.Window.shouldClose = false;
```

--- 

Изменяется **seed** для псевдослучайных чисел. В качестве параметра передается результат функции ```time()```, который всегда разный.

Если **seed** всегда разный то:

- Генерация случайных чисел происходит разная (от запуска к запуску).

Если **seed** всегда одинаковый то:

- Генерация случайных чисел дает результаты одинаковые.

*Фиксированный* **seed** полезен, когда нужно повторить прошлую последовательность случайных чисел, таких как:

- Генерация карт (чтобы карта сгенерировалась снова также)
- Отладка, чтобы было меньше рандома, и повторить прошлое состояние.

*Нефиксированный* **seed** (как в примере) полезен, когда нужно всегда добавлять случайность, что в играх обязательно нужно.


```c
SetRandomSeed((unsigned int)time(NULL));
```

Работа функции ```InitWindow()``` закончена.

--- 
