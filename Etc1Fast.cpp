#include "Etc1Fast.h"

//#include <omp.h> 
#ifdef __INTEL_COMPILER
  #include <cilk/cilk.h>
#endif

#ifndef ALIGN16
  #define   ALIGN16( x ) __declspec(align(16)) x
#endif

#ifndef ByteSwap16
  #define ByteSwap16(n) ( ((((unsigned int) n) << 8) & 0xFF00) | ((((unsigned int) n) >> 8) & 0x00FF) )
#endif


void Etc1Fast::convertRGBAtoETC1(unsigned char* compressedData, unsigned char* data, int sizeX, int sizeY){
  if(sizeX < 8 || sizeY < 8){
    cout << "Etc1Fast::convertRGBAtoETC1: Invalid image size: " << sizeX << " x " << sizeY << endl;
    return;
  }

  if((sizeX % 8 != 0) || ((sizeY % 8) != 0)){
    cout << "Etc1Fast::convertRGBAtoETC1: Invalid image size. Needs to be dividable by 8: " << sizeX << " x " << sizeY << endl;
    return;
  }

  if(!data || !compressedData){
    cout << "Etc1Fast::convertRGBAtoETC1: Invalid data or compressedData" << endl;
    return;
  }
 
  unsigned char* compressedDataPointer = compressedData;
  unsigned char* inBuf = data;


    for ( int j = 0; j < sizeY; j += 4) {
#ifdef __INTEL_COMPILER
      cilk_spawn(etc1helperFunction(compressedDataPointer, inBuf, sizeX, j));
#else
      cout << "convertRGBAtoETC1: No cilk found. Using single thread" << endl;
      etc1helperFunction(compressedDataPointer, inBuf, sizeX, j);
#endif
    }
}


void Etc1Fast::etc1helperFunction(unsigned char* compressedDataPointer, unsigned char* inBuf, int sizeX, int j) {
  unsigned char*  currentInBuf = inBuf + sizeX * 4 * 4 * (j / 4);

  for ( int i = 0; i < sizeX; i += 4) {
    ALIGN16( unsigned char block[64] );
    // copies color values into a block of 4x4 (unsigned char rgba)
	  ExtractBlock( currentInBuf + i * 4, sizeX, block );

    unsigned char* currentCompressedDataPointer = compressedDataPointer + 8 * (i / 4) + (8 * sizeX / 4) * (j / 4);
    pack_etc1_block((unsigned long long*)currentCompressedDataPointer, block);
  }
}





