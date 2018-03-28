//
// Copyright (c) 2017 Native Instruments GmbH, Berlin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

/***************************************************************\
*   IEEE80.h							*
*   Convert between "double" and IEEE 80 bit format  		*
*   in machine independent manner.				*
*   Assumes array of char is a continuous region of 8 bit frames*
*   Assumes (unsigned long) has 32 useable bits			*
*   billg, dpwe @media.mit.edu					*
*   01aug91							*
*   19aug91  aldel/dpwe  workaround top bit problem in Ultrix   *
*                        cc's double->ulong cast		*
*   05feb92  dpwe/billg  workaround top bit problem in 		*
*                        THINKC4 + 68881 casting		*
\***************************************************************/
/* Copyright (c) 1991-1992 above authors
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:
The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <math.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>

constexpr unsigned long ulpow2to31 = 0x80000000L;
constexpr double        dpow2to31  = 2147483648.0; /* 2^31 */

/* have to deal with ulong's 32nd bit conditionally as double<->ulong casts
   don't work in some C compilers */

inline double        myUlongToDouble( unsigned long ul );
inline unsigned long myDoubleToUlong( double val );

inline double myUlongToDouble( unsigned long ul )
{
    double val;

    /* in THINK_C, ulong -> double apparently goes via long, so can only
       apply to 31 bit numbers.  If 32nd bit is set, explicitly add on its
       value */
    if ( ul & ulpow2to31 )
        val = dpow2to31 + ( ul & ( ~ulpow2to31 ) );
    else
        val = ul;
    return val;
}

inline unsigned long myDoubleToUlong( double val )
{
    unsigned long ul;

    /* cannot cast negative numbers into unsigned longs */
    if ( val < 0 )
    {
        throw std::runtime_error( "IEEE80:DoubleToUlong: val < 0" );
    }

    /* in ultrix 4.1's cc, double -> unsigned long loses the top bit,
       so we do the conversion only on the bottom 31 bits and set the
       last one by hand, if val is truly that big */
    /* should maybe test for val > (double)(unsigned long)0xFFFFFFFF ? */
    if ( val < dpow2to31 )
        ul = (unsigned long) val;
    else
        ul = ulpow2to31 | (unsigned long) ( val - dpow2to31 );
    return ul;
}


/*
 * Convert IEEE 80 bit floating point to double.
 * Should be portable to all C compilers.
 */
inline double ieee_80_to_double( unsigned char* p )
{
    char          sign;
    short         exp   = 0;
    unsigned long mant1 = 0;
    unsigned long mant0 = 0;
    double        val;

    exp = *p++;
    exp <<= 8;
    exp |= *p++;
    sign = ( exp & 0x8000 ) ? 1 : 0;
    exp &= 0x7FFF;

    mant1 = *p++;
    mant1 <<= 8;
    mant1 |= *p++;
    mant1 <<= 8;
    mant1 |= *p++;
    mant1 <<= 8;
    mant1 |= *p++;

    mant0 = *p++;
    mant0 <<= 8;
    mant0 |= *p++;
    mant0 <<= 8;
    mant0 |= *p++;
    mant0 <<= 8;
    mant0 |= *p++;

    /* special test for all bits zero meaning zero
       - else pow(2,-16383) bombs */
    if ( mant1 == 0 && mant0 == 0 && exp == 0 && sign == 0 )
        return 0.0;
    else
    {
        val = myUlongToDouble( mant0 ) * pow( 2.0, -63.0 );
        val += myUlongToDouble( mant1 ) * pow( 2.0, -31.0 );
        val *= pow( 2.0, ( (double) exp ) - 16383.0 );
        return sign ? -val : val;
    }
}

/*
 * Convert double to IEEE 80 bit floating point
 * Should be portable to all C compilers.
 * 19aug91 aldel/dpwe  covered for MSB bug in Ultrix 'cc'
 */

inline void double_to_ieee_80( double val, unsigned char* p )
{
    char          sign  = 0;
    short         exp   = 0;
    unsigned long mant1 = 0;
    unsigned long mant0 = 0;

    if ( val < 0.0 )
    {
        sign = 1;
        val  = -val;
    }

    if ( val != 0.0 ) /* val identically zero -> all elements zero */
    {
        exp = (short) ( log( val ) / log( 2.0 ) + 16383.0 );
        val *= pow( 2.0, 31.0 + 16383.0 - (double) exp );
        mant1 = myDoubleToUlong( val );
        val -= myUlongToDouble( mant1 );
        val *= pow( 2.0, 32.0 );
        mant0 = myDoubleToUlong( val );
    }

    *p++ = ( ( sign << 7 ) | ( exp >> 8 ) );
    *p++ = 0xFF & exp;
    *p++ = (char) ( 0xFF & ( mant1 >> 24 ) );
    *p++ = (char) ( 0xFF & ( mant1 >> 16 ) );
    *p++ = (char) ( 0xFF & ( mant1 >> 8 ) );
    *p++ = (char) ( 0xFF & ( mant1 ) );
    *p++ = (char) ( 0xFF & ( mant0 >> 24 ) );
    *p++ = (char) ( 0xFF & ( mant0 >> 16 ) );
    *p++ = (char) ( 0xFF & ( mant0 >> 8 ) );
    *p++ = (char) ( 0xFF & ( mant0 ) );
}