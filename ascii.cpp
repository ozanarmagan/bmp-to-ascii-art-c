#include <iostream>
#include <fstream>
#include <string>
#include<Windows.h>
using namespace std;

string scale = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
int numScl = scale.length() - 1;



typedef struct
{
    size_t width;
    size_t height;
    unsigned char * data;
} image;


void load(image ** img,string path)
{
	ifstream fst(path,ios::binary);
	fst.seekg(0,fst.end);
	int size = fst.tellg();
	fst.seekg(0,fst.beg);
	unsigned char* buffer = new unsigned char[size];
	int i = 0;
	fst.read((char*)&buffer[0],size);
	fst.close();
	size_t pdOffset = buffer[10] | buffer[11] << 8 | buffer[12] << 16   | buffer[13] << 24;
	unsigned long width = buffer[18] | buffer[19] << 8 | buffer[20] << 16   | buffer[21] << 24;
	unsigned long height = buffer[22] | buffer[23] << 8 | buffer[24] << 16 | buffer[25] << 24;
	unsigned long bpp = buffer[28] | buffer[29] << 8;
	int noCompression = buffer[30] == 0 && buffer[31] == 0 && buffer[32] == 0   && buffer[33] == 0;

	if(bpp != 24 || !noCompression || width < 1 || height < 1 || width > 64000 || height > 64000)
	{
		cout << "Dosya hatali...";
	}

	int bytesPerPixel = (int) (bpp / 8);
	size_t rowBytes = (width * bytesPerPixel + 3) / 4 * 4;
	
	size_t usedRowBytes = width * bytesPerPixel;
  	size_t imageBytes = rowBytes * height;

	*img = (image*) malloc(sizeof(image));
	(*img)->height = height;
	(*img)->width = width;

	/* resimin boyutunu hesapla ve dinamik olarak o kadar boşluk ayır */
	size_t img_size = width * height;
	(*img)->data = (unsigned char *) malloc(img_size);

	unsigned char * ptr = (*img)->data;
  	unsigned char * srcPtr = &buffer[pdOffset];
	
	for(size_t i = 0;i < img_size;++i)
	{
		unsigned char r = *srcPtr;
		unsigned char g = *(srcPtr + 1);
		unsigned char b = *(srcPtr + 2);

		*ptr = (0.299 * r + 0.587 * g + 0.114 * b);
		ptr++;
		srcPtr += bytesPerPixel;
		if (i % width == 0)
			srcPtr += rowBytes - usedRowBytes;
	}

	free(buffer);
}

void make_ascii(image * img,string path)
{
	ofstream ofst;
	ofst.open(path);
	cout << "Dosyaya yaziliyor...";
	for (size_t y = img->height - 1; y > 0; --y)
	{
		for (size_t x = 0; x < img->width; ++x)
		{
			unsigned char c = *(img->data + x + img->width * y);
			int rescaled = c * numScl / 256;
			ofst << scale[numScl - rescaled];
		}
		ofst << endl;
	}
	cout << "Tamamlandi";
	ofst.close();
}

void release(image * img)
{
  if (img)
  {
    if (img->data)
      free(img->data);
 
    free(img);
  }
}

int main(){
	image *img = NULL;
	load(&img,"mc2.bmp");
	make_ascii(img,"deneme.txt");
	release(img);
}