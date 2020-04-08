#ifndef BMPREADER_H
#define BMPREADER_H

typedef struct _IMAGE
{
	int width;
	int height;
	unsigned char* data;

} Image;

extern int LoadBMP(const char* path, Image* image);

#endif