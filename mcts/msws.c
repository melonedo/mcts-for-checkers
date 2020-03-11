#ifndef CHECKERS_MSWS_C_INCLUDED
#define CHECKERS_MSWS_C_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************\
*                                                                          *
*  Middle Square Weyl Sequence Random Number Generator                     *
*                                                                          *
*  msws() - returns a 32 bit unsigned int [0,0xffffffff]                   *
*                                                                          *
*  The state vector consists of three 64 bit words:  x, w, and s           *
*                                                                          *
*  x - random output [0,0xffffffff]                                        *
*  w - Weyl sequence (period 2^64)                                         *
*  s - an odd constant                                                     *
*                                                                          *
*  The Weyl sequence is added to the square of x.  The middle is extracted *
*  by shifting right 32 bits:                                              *
*                                                                          *
*  x *= x; x += (w += s); return x = (x>>32) | (x<<32);                    *
*                                                                          *
*  The constant s should be set to a random 64-bit pattern.  The utility   *
*  init_rand_digits in init.h may be used to initialize the constant.      *
*  This utility generates constants with different hexadecimal digits.     *
*  This assures sufficient change in the Weyl sequence on each iteration   *
*  of the RNG.                                                             *
*                                                                          *
*  Note:  This version of the RNG includes an idea proposed by             *
*  Richard P. Brent (creator of the xorgens RNG).  Brent suggested         *
*  adding the Weyl sequence after squaring instead of before squaring.     *
*  This provides a basis for uniformity in the output.                     *
*                                                                          *
*  Copyright (c) 2014-2019 Bernard Widynski                                *
*                                                                          *
*  This code can be used under the terms of the GNU General Public License *
*  as published by the Free Software Foundation, either version 3 of the   *
*  License, or any later version. See the GPL license at URL               *
*  http://www.gnu.org/licenses                                             *
*                                                                          *
\**************************************************************************/

uint32_t msws()
{
  static uint64_t x = 0x86ced140fe30d875, w = 0x86ced140fe30d875, s = 0x86ced140fe30d875;
  x *= x;
  x += (w += s);
  return x = (x>>32) | (x<<32);
}

#ifdef __cplusplus
}
#endif
#endif
