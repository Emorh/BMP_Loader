// Этот загрузчик работает только на ОС windows и не учитывает 
// 12-байтные заголовки, которые есть, например в OS/2.

// Так же я предполагаю, что ему передаются незакодированные картинки,
// поэтому я возваращаю ошибку, если кодирование есть.

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "BMPReader.h"

#define BMP_SIZE_FILE_HEADER 14
#define BMP_SIZE_INFO_HEADER 40

#define BMP_COLOR_BITS24 24

unsigned int convertUInt16(unsigned char buf[2])
{
	return (buf[1] << 8) | buf[0];
}

unsigned int convertUInt32(unsigned char buf[4])
{
	unsigned res = buf[3];
	res = (res << 8) | buf[2];
	res = (res << 8) | buf[1];
	res = (res << 8) | buf[0];
	return res;
}

// Возвращает ненулевое значение, если что-то пошло не так
int readFileHeader(FILE* file, unsigned int* bitmap_offset)
{
	unsigned char header[BMP_SIZE_FILE_HEADER];


	if (fseek(file, 0, SEEK_SET))
	{
		return 1;
	}

	if (fread(header, BMP_SIZE_FILE_HEADER, 1, file) != 1)
	{
		return 1;
	}

	if (header[0] != 'B' || header[1] != 'M')
	{
		return 1;
	}

	unsigned int offset = convertUInt32(header + 10);

	if (fread(header, 4, 1, file) != 1)
	{
		return 1;
	}

	// На ОС Windows размер этого заголовка не менее 40
	if (convertUInt32(header) < BMP_SIZE_INFO_HEADER)
	{
		return 1;
	}

	*bitmap_offset = offset;
	return 0;
}

int LoadBMP(const char* path, Image* out_img)
{
	FILE* file;
	unsigned int bitmap_offset;

	file = fopen(path, "rb");

	if (!file)
		return 0;

	if (readFileHeader(file, &bitmap_offset))
	{
		fclose(file);
		return 0;
	}



	if (fseek(file, BMP_SIZE_FILE_HEADER, SEEK_SET))
	{
		fclose(file);
		return 0;
	}

	unsigned char buf[16];
	fread(buf, 16, 1, file);

	int x_res = (int)convertUInt32(buf + 4);
	int y_res = (int)convertUInt32(buf + 8);
	int n_bits = (int)convertUInt16(buf + 14);

	if (n_bits == BMP_COLOR_BITS24)
	{

		if (fseek(file, (int)bitmap_offset, SEEK_SET))
		{
			fclose(file);
			return 0;
		}


		int rgb_size = 3 * x_res;
		if (rgb_size % 4 != 0)
		{
			rgb_size += 4 - (rgb_size % 4);
		}

		out_img->width = x_res;
		out_img->height = y_res;
		out_img->data = (unsigned char*)malloc(x_res * y_res * 3);

		if (out_img->data == NULL)
		{
			fclose(file);
			return 0;
		}

		unsigned char* rgb = (unsigned char*)malloc(rgb_size);


		if (rgb == NULL)
		{
			fclose(file);
			return 0;
		}

		unsigned char* line;

		for (int y = 0; y < y_res; ++y)
		{
			size_t numb = 0;

			if (fread(rgb, rgb_size, 1, file) != 1)
			{
				fclose(file);
				free(rgb);
				return 0;
			}

			fread(rgb, rgb_size, 1, file);

			line = out_img->data + x_res * 3 * y;

			for (int x = 0; x < x_res; ++x)
			{
				// Индексация такая, поскольку в 24 битном формате каждый пиксель описывается в формате BGR
				out_img->data[x_res * 3 * y + x * 3 + 2] = rgb[numb++];
				out_img->data[x_res * 3 * y + x * 3 + 1] = rgb[numb++];
				out_img->data[x_res * 3 * y + x * 3] = rgb[numb++];
			}
		}
		fclose(file);
		free(rgb);
	}
	else
	{
		return 0;
	}

	return 1;
}
