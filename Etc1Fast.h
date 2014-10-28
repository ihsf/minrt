#ifndef _ETC1FAST_H
#define _ETC1FAST_H

#include <math.h>			
#include <stdio.h>	
#include <malloc.h>
#include "WindowsHelper.h"
#include "ProcessRGB.hpp"
#include "ForceInline.h"

#ifdef _MSC_VER
	#pragma warning( disable : 4996 ) // '*' was declared deprecated
#endif



using namespace std;

class Etc1Fast {
	public:
		Etc1Fast();
		~Etc1Fast();

    static void convertRGBAtoETC1(unsigned char* compressedData, unsigned char* data, int sizeX, int sizeY);


	private:
    finline static void ExtractBlock( const unsigned char* inPtr, int width, unsigned char* colorBlock ){
      for(int x = 0; x < 4; x++) {
        for(int y = 0; y < 4; y++){
          colorBlock[x*4*4 + y * 4    ] = *(inPtr + (x * 4 + y * width * 4) + 2);
          colorBlock[x*4*4 + y * 4 + 1] = *(inPtr + (x * 4 + y * width * 4) + 1);
          colorBlock[x*4*4 + y * 4 + 2] = *(inPtr + (x * 4 + y * width * 4) + 0);
          colorBlock[x*4*4 + y * 4 + 3] = *(inPtr + (x * 4 + y * width * 4) + 4);
        }
      }
    }

    finline static void setAlphaTo255(unsigned char* colorBlock ){
      for(int j = 0; j < 16; j++) {
        colorBlock[j * 4 + 3] = 255;
      }
    }

    static void etc1helperFunction(unsigned char* compressedDataPointer, unsigned char* inBuf, int sizeX, int j);

    struct ETC1Header {
      char tag[6];                   // "PKM 10"
      unsigned short format;         // Format == number of mips (== zero)
      unsigned short texWidth;       // Texture dimensions, multiple of 4 (big-endian)
      unsigned short texHeight;
      unsigned short origWidth;      // Original dimensions (big-endian)
      unsigned short origHeight;
    };

     finline static void Etc1Fast::pack_etc1_block(unsigned long long* compressedDataPointer, unsigned char* block){
      *compressedDataPointer = ProcessRGB(block);
    }    
};

#endif
