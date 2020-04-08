# BMP_Loader

Используется, для загрузки 24-разрядных bmp рисунков.  

Image - структура текстуры, содержащая следующие поля: width - ширина текстуры, height - высота текстуры, data - массив размера width * height * 3, содержащий информацию о цвете пикселя в формате RGB

Пример использования в OpenGL:

```
#include "BMPReader.h"

.........................

Image* img = new Image();
LoadBMP("container.bmp", img);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->data);
```
