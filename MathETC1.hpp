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

#ifndef __DARKRL__MATH_HPP__
#define __DARKRL__MATH_HPP__

#include <algorithm>
#include <cmath>

template<typename T>
inline T AlignPOT( T val )
{
    if( val == 0 ) return 1;
    val--;
    for( unsigned int i=1; i<sizeof( T ) * 8; i <<= 1 )
    {
        val |= val >> i;
    }
    return val + 1;
}

inline int CountSetBits( unsigned int val )
{
    val -= ( val >> 1 ) & 0x55555555;
    val = ( ( val >> 2 ) & 0x33333333 ) + ( val & 0x33333333 );
    val = ( ( val >> 4 ) + val ) & 0x0f0f0f0f;
    val += val >> 8;
    val += val >> 16;
    return val & 0x0000003f;
}

inline int CountLeadingZeros( unsigned int val )
{
    val |= val >> 1;
    val |= val >> 2;
    val |= val >> 4;
    val |= val >> 8;
    val |= val >> 16;
    return 32 - CountSetBits( val );
}

inline float sRGB2linear( float v )
{
    const float a = 0.055f;
    if( v <= 0.04045f )
    {
        return v / 12.92f;
    }
    else
    {
        return pow( ( v + a ) / ( 1 + a ), 2.4f );
    }
}

inline float linear2sRGB( float v )
{
    const float a = 0.055f;
    if( v <= 0.0031308f )
    {
        return 12.92f * v;
    }
    else
    {
        return ( 1 + a ) * pow( v, 1/2.4f ) - a;
    }
}

template<class T>
inline T SmoothStep( T x )
{
    return x*x*(3-2*x);
}

inline unsigned char clampu8( int val )
{
    return std::min( std::max( 0, val ), 255 );
}

template<class T>
inline T sq( T val )
{
    return val * val;
}

static inline int mul8bit( int a, int b )
{
    int t = a*b + 128;
    return ( t + ( t >> 8 ) ) >> 8;
}

#endif
