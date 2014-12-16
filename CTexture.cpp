#include "CTexture.h"

// TODO:   REMOVE BEFORE RELEASE
// TODO:   REMOVE BEFORE RELEASE
// TODO:   REMOVE BEFORE RELEASE


#ifdef WIN32
typedef unsigned __int64 ULONGLONG;
#else
typedef unsigned long long ULONGLONG;
#endif

void CTexture::init(){
	sizeX = sizeY = channels = 0; 
	data = NULL;
	imageHasBeenFlipped = false;
}

bool CTexture::LoadTGA(const char* const strFileName) {
	FILE *pfile;
	unsigned char tempColor;   // This will change the images from BGR to RGB.
	unsigned char uselessChar; // This will be used to hold char data we dont want.
	unsigned char imageTypeCode;
	unsigned char bitCount;
	short int	  uselessInt;	// This will be used to hold int data we dont want.
	
  if (!strFileName)
    return false;

  init();

	// Open the image and read in binary mode.
	pfile = fopen(strFileName, "rb");

	// check if the file opened.
	if (!pfile){
		cout << "Error opening " << strFileName << endl;
		return false;
	}

	// Read in the two first bytes
	unsigned char additionalOffset;
	fread(&additionalOffset, sizeof(unsigned char), 1, pfile);
	fread(&uselessChar, sizeof(unsigned char), 1, pfile);
	
	// Read the image type, 2 is color, 4 is is greyscale.
	fread(&imageTypeCode, sizeof(unsigned char), 1, pfile);

	// We only want to be able to read in color or greyscale .tga's.
	if ((imageTypeCode != 2) && (imageTypeCode != 3)  && (imageTypeCode != 10) && 
		(imageTypeCode != 1)){
		fclose(pfile);
		cout << "Error opening " << strFileName << " with TGAtype: " << (int)imageTypeCode << endl;
		cout << "Please convert this tga file manually to 24 bit. Exiting now." << endl;
		exit(1);
	    return false;
	}

	// Get rid of 13 bytes of useless data.
	fread(&uselessInt, sizeof(short int), 1, pfile);
	fread(&uselessInt, sizeof(short int), 1, pfile);
	fread(&uselessChar, sizeof(unsigned char), 1, pfile);
	fread(&uselessInt, sizeof(short int), 1, pfile);
	fread(&uselessInt, sizeof(short int), 1, pfile);

	// Get the image width and height.
	fread(&sizeX, sizeof(short int), 1, pfile);
	fread(&sizeY, sizeof(short int), 1, pfile);

	// Get the bit count.
	fread(&bitCount, sizeof(unsigned char), 1, pfile);

	// 32 indicates that the first pixel is upper left on the screen
	fread(&uselessChar, sizeof(unsigned char), 1, pfile);

	// some .tga files contain addition header information AFTER
	// the standard header like the fileName. the number of that
	// bytes can be read at the first unsigned char in the header.
	for(int k = 0; k < additionalOffset; k++)
		fread(&uselessChar, sizeof(unsigned char), 1, pfile);

	// If the image is RGB then colorMode should be 3 and RGBA would
	// make colorMode equal to 4.  This will help in our loop when
	// we must swap the BGR(A) to RGB(A).
	channels = bitCount / 8;

	// Determine the size of the tga image.
	ULONGLONG tgaSize = (ULONGLONG)sizeX * (ULONGLONG)sizeY * (ULONGLONG)channels;

	if(tgaSize > ULONGLONG(1024 * 1024 * 600)){
		cout << "Loading huge texture with " << sizeX << "x" << sizeY << " from " << strFileName << ". This takes a while!" << endl;
	}

	// Allocate memory for the tga image.
	delete [] data;
	
	data = new unsigned char[tgaSize];

	if(!data){
		cout << "LoadTGA: data = new unsigned char[tgaSize] returned NULL: " << strFileName << endl;
		return false;
	}

	if(imageTypeCode != 10){
		// Read the image into imageData.
		fread(data, sizeof(unsigned char), tgaSize, pfile);

		//  black and white TGA
		if(channels == 1){
      cout << "channels == 1 not supported" << endl;
      exit(1);
		}
	} else {
		// RLE compression
		unsigned char rleID = 0;
		int i = 0;
		while(i < tgaSize){
			// Read in the current color count + 1
			fread(&rleID, sizeof(unsigned char), 1, pfile);
			// Check if we don't have an encoded string of colors
			if(rleID < 128){
				// Increase the count by 1
				rleID++;

				// Go through and read all the unique colors found
				while(rleID){
					fread(&data[i], sizeof(unsigned char), 1, pfile);
					fread(&data[i+1], sizeof(unsigned char), 1, pfile);
					fread(&data[i+2], sizeof(unsigned char), 1, pfile);
					// If we have a 4 channel 32-bit image, assign one more for the alpha
					if(channels == 4)
						fread(&data[i+3], sizeof(unsigned char), 1, pfile);

					rleID--;
					i += channels;
				}
			}
			// Else, let's read in a string of the same character
			else{
				// Minus the 128 ID + 1 (127) to get the color count that needs to be read
				rleID -= 127;

				// Go and read as many pixels as are the same
				bool firstTime = true;
				unsigned char r, g, b, a;
				while(rleID){
					if(firstTime){
						fread(&r, sizeof(unsigned char), 1, pfile);
						fread(&g, sizeof(unsigned char), 1, pfile);
						fread(&b, sizeof(unsigned char), 1, pfile);
						if(channels == 4)
							fread(&a, sizeof(unsigned char), 1, pfile);
						firstTime = false;
					}
					data[i]   = r;
					data[i+1] = g;
					data[i+2] = b;
					if(channels == 4)
						data[i+3] = a;
					rleID--;
					i += channels;
				}
			}
			if(i > tgaSize)
				cout << "CTexture::LoadTGA: Possible overrun in type 10 (RLE)!" << endl;
		}
	}

	// This loop will swap the BGR(A) to RGB(A).
	for (ULONGLONG index = 0; index < tgaSize; index += channels){
		tempColor = data[index];
		data[index] = data[index + 2];
		data[index + 2] = tempColor;
	}

	// if the texture is vertically wrong, we flip it
	if(uselessChar == 32){
		flipVertically();
		imageHasBeenFlipped = true;
	}

	// Close the file where your done.
	fclose(pfile);

	// support only textures with at least 2x2 pixels
	Assert(sizeX > 1, "LoadTGA");
	Assert(sizeY > 1, "LoadTGA");

	return true;
}

void CTexture::flipVertically(){
	char* reverseTemp = (char*)malloc(sizeof(char)*sizeX * channels);
	ULONGLONG k;

	for(ULONGLONG i = 0; i < sizeY / 2; i++){
		for(k = 0; k < sizeX * channels; k++)
			reverseTemp[k] = data[k + sizeX * channels * i];

		for(k = 0; k < sizeX * channels; k++)
			data[k + sizeX * channels * i] = data[k + sizeX * channels * (sizeY - i - 1)];

		for(k = 0; k < sizeX * channels; k++)
			data[k + sizeX * channels * (sizeY - i - 1)] = reverseTemp[k];
	}

	free(reverseTemp);
}


void CTexture::convertFrom24BitTo32Bit(){
	if(channels != 3){
		cout << "convertFrom24BitTo32Bit: Image is not a 24 bit image." << endl;
		return;
	}
	if(!data){
		cout << "convertFrom24BitTo32Bit: No data found." << endl;
		return;
	}

	ULONGLONG size32Bit = (ULONGLONG)sizeX * (ULONGLONG)sizeY * (ULONGLONG)4;
	unsigned char *data32Bit = new unsigned char[size32Bit];
	ULONGLONG size24Bit = (ULONGLONG)sizeX * (ULONGLONG)sizeY * (ULONGLONG)3;

	const unsigned char OPAQUEVALUE = 255;
	//const unsigned char OPAQUEVALUE = 0;

	for(ULONGLONG i = 0, j = 0; i < size24Bit; i += 3, j += 4){
		data32Bit[j] = data[i];
		data32Bit[j+1] = data[i+1];
		data32Bit[j+2] = data[i+2];		
		
		data32Bit[j+3] = OPAQUEVALUE;	
	}

	channels = 4;
	
	delete [] data;
	data = data32Bit;
}
