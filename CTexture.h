#ifndef CTEXTUREBASE_H
#define CTEXTUREBASE_H

// TODO:   REMOVE BEFORE RELEASE
// TODO:   REMOVE BEFORE RELEASE
// TODO:   REMOVE BEFORE RELEASE

// LICENSE: LoadTGA and SaveTGA based on loader from the web

#include <stdio.h>
#include <iostream>
#include "Engine.h"

using namespace std;

class CTexture{
	public:
		// loads a .tga file
		bool LoadTGA(const char* const strFileName);	  

		typedef enum { 
			   TGA_RGB = 2,   // This tells us it's a normal RGB (really BGR) file
			   TGA_A   = 3,   // This tells us it's a ALPHA file
			   TGA_RLE = 10   // length encoded
		} TGAFlags; 	 

		short int getSizeX(){ return sizeX; }
		short int getSizeY(){ return sizeY; }
    int getChannels(){ return channels; }

		void convertFrom24BitTo32Bit();
		void flipVertically();

		unsigned char *data;    // The image pixel data 


	protected:	  
		void init();

		int channels;           // The channels in the image (3 = RGB : 4 = RGBA)
		short int sizeX;              // The width of the image in pixels
		short int sizeY;              // The height of the image in pixels

		bool imageHasBeenFlipped;
};

#endif
