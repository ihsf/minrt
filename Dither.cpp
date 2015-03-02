// https://bitbucket.org/wolfpld/etcpak
// Copyright(c) 2013, Bartosz Taudul <wolf.pld@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and / or other materials provided with the distribution.
// * Neither the name of the <organization> nor the
// names of its contributors may be used to endorse or promote products
// derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <algorithm>
#include <string.h>

#include "Dither.hpp"
#include "MathETC1.hpp"

static uint8_t e5[32];
static uint8_t e6[64];
static uint8_t qrb[256+16];
static uint8_t qg[256+16];

void InitDither()
{
    for( int i=0; i<32; i++ )
    {
        e5[i] = (i<<3) | (i>>2);
    }
    for( int i=0; i<64; i++ )
    {
        e6[i] = (i<<2) | (i>>4);
    }
    for( int i=0; i<256+16; i++ )
    {
        int v = std::min( std::max( 0, i-8 ), 255 );
        qrb[i] = e5[mul8bit( v, 31 )];
        qg[i] = e6[mul8bit( v, 63 )];
    }
}

void Dither( uint8_t* data )
{
    int err[8];
    int* ep1 = err;
    int* ep2 = err+4;

    for( int ch=0; ch<3; ch++ )
    {
        uint8_t* ptr = data + ch;
        uint8_t* quant = (ch == 1) ? qg + 8 : qrb + 8;
        memset( err, 0, sizeof( err ) );

        for( int y=0; y<4; y++ )
        {
            uint8_t tmp;
            tmp = quant[ptr[0] + ( ( 3 * ep2[1] + 5 * ep2[0] ) >> 4 )];
            ep1[0] = ptr[0] - tmp;
            ptr[0] = tmp;
            tmp = quant[ptr[4] + ( ( 7 * ep1[0] + 3 * ep2[2] + 5 * ep2[1] + ep2[0] ) >> 4 )];
            ep1[1] = ptr[4] - tmp;
            ptr[4] = tmp;
            tmp = quant[ptr[8] + ( ( 7 * ep1[1] + 3 * ep2[3] + 5 * ep2[2] + ep2[1] ) >> 4 )];
            ep1[2] = ptr[8] - tmp;
            ptr[8] = tmp;
            tmp = quant[ptr[12] + ( ( 7 * ep1[2] + 5 * ep2[3] + ep2[2] ) >> 4 )];
            ep1[3] = ptr[12] - tmp;
            ptr[12] = tmp;
            ptr += 16;
            std::swap( ep1, ep2 );
        }
    }
}
