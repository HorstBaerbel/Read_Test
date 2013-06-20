#include "Targa.h"

#include <stdio.h>


//If there's no pragma pack(1) available on the platform you're on,
//you have to define this as a union using consecutive 8/16 bit variables.
#pragma pack(1)
struct TargaHeader {
    unsigned char  idLenght;
    unsigned char  paletteType;
    unsigned char  imageType;
    unsigned short paletteStart;
    unsigned short paletteEntries;
    unsigned char  paletteBitSize;
    unsigned short imageX;
    unsigned short imageY;
    unsigned short imageWidth;
    unsigned short imageHeight;
    unsigned char  bitsPerPixel;
    unsigned char  flags;
    //now follow image id(name)
    //and palette entries
}; /*!TARGA File header structure. Size is 18 Bytes.*/

void dumpToTGA(const char * fileName, const unsigned char * buffer, const int width, const int height)
{
    //set up TARGA file header
    struct TargaHeader h;
    h.idLenght = 0;
    h.paletteType = 0;
    h.imageType = 2; //10 for RLE-RGB;
    h.paletteStart = 0;
    h.paletteEntries = 0;
    h.paletteBitSize = 0;
    h.imageX = 0;
    h.imageY = 0;
    h.imageWidth = width;
    h.imageHeight = height;
    h.bitsPerPixel = 32;
    h.flags = 0;
    //open snapshot file
    FILE * f = fopen(fileName, "wbS");
    if (f != NULL) {
        //write header to file
        fwrite(&h, sizeof(struct TargaHeader), 1, f);
        //save image top-down
        for (int j = height - 1; j >= 0; j--) {
            for (int i = 0; i < width; i++) {
                //swap r-b color values
                unsigned int tempc = ((unsigned int *)buffer)[j * width + i];
                //tempc = (tempc & 0xFF00FF00) | ((tempc & 0x00FF0000) >> 16) | ((tempc & 0x000000FF) << 16);
                //write to file
                fwrite(&tempc, sizeof(unsigned int), 1, f);
            }
        }
        fclose(f);
    }
}
