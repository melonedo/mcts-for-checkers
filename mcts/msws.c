
#ifdef __cplusplus
extern "C" {
#endif

#include "msws.h"

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

static uint64_t x, w, s;
uint32_t msws()
{
  x *= x;
  x += (w += s);
  return x = (x>>32) | (x<<32);
}

uint64_t msws_seeds[] = {
0x8b5ad4ceb9c1fe73,
0x64d098b5c4f26d37,
0x45973acb0ad43b97,
0x6e9c5db170d261c9,
0x4fa75198bc653efb,
0x3d4c562ea9451fed,
0xd8b57104d2850b1b,
0x2bdfe60a326fdab1,
0x86ced140fe30d875,
0x7c981fa6257d863d,
0xe5f68a3c60d89be5,
0x8e46fc02dc40ef19,
0x02c34bf7bdef4593,
0xcebf7a1d87d2f0b5,
0x4bf18673ce3a69df,
0xcb5970f8cde89211,
0x5bf739adceaf657d,
0x68cae093a37f0c4d,
0xf1d465a8c08f3e93,
0xf713c8edd6e1bc75,
0x47890a62de4f2377,
0x86342a07413b8fd3,
0xf01527dccd9847f1,
0x762c03e1cf196381,
0x0578be1645839bf1,
0x24fb567a64bc5781,
0xa1b3cd0f91ba270f,
0xc98a21d45b2a9431,
0xdfb654c8e48fd765,
0xdb2065ecda3651c7,
0x836b054182405a1b,
0xd0e762c56c28d4eb,
0x5621ce7981a609ef,
0x012a385da13d9877,
0x62fac0712a7de945,
0x47a806b516a3c40f,
0x568c7b29209f1cb3,
0x593842cd1b3c45f3,
0x928c3ea0a27683ed,
0x8637cda4e932c467,
0x79e26ad8d269ea75,
0x15c2e63bba92f46d,
0x0a5d93cf526ba9f7,
0x1b045792c480579b,
0x902a7d858d0ac7b5,
0x4cd5f1a8c7832f6f,
0xec4d53fb9152ae4b,
0xf281047e03e76c53,
0xd8eca931a5d6742f,
0x8325a0144dbc07ff,
0xe61f4b27f5d231a5,
0xfce1746ade80c673,
0x90647bdc34a9218f,
0x5ab2617f208fec75,
0xc9b4f251230e4a51,
0xd821c75406dc17b9,
0xd12a47860ed64a85,
0xcd46a5e88bd27a3f,
0x63dce27ba9be6fd3,
0x9a586f3d1c8f5279,
0x791c862ff349d6b5,
0xe0f7cd51ae5917b3,
0x90d7c8a2d70893cb,
0xc03a16244cf8615b,
0x420187d684def529,
0x906ed7b875bc290f,
0x3b1205c9136a42b1,
0x78a6120ba429fc6f,
0xd2b59f7c47f305af,
0x79bec51d86753bdb,
0xc7196bef53e8adb1,
0x8da1e9f014e2b971,
0xade0542162adb087,
0x76ec9582ea5d2c97,
0x6a4eb513582fe311,
0x07c6b2d4069df413,
0x2a048ec5380cf6eb,
0xd13748e580c1efb3,
0x0f92e13623ac9f57,
0xf8c015b7afe06187,
0x0b91ac676d093a25,
0xe361df48d4b85c9f,
0x69e2f0585e172f4d,
0x5e7cdf98cfeb0387,
0xe4b6ad0895732e65,
0x3e1695a89f40e1b3,
0x4901e278d2f691ad,
0xf1904a781b56a043,
0x215490a8ae1bd289,
0x1d4eb50870c68a15,
0xdc5eb79815238ead,
0xf02e4958673981ed,
0x1fa805474ea10683,
0x0e51f4670daef97d,
0x83059fb6934d0ca3,
0x0af6c93535d20e1b,
0x1b97a0e52cd9f1a3,
0x57b316c45b860d43,
0xce920ad3b8f347c3,
0x754a6d1247b18f59,
0xb736fd815d4c38eb,
0x9e571c20b3ca8f13,
0x491028efe8d6f1c3,
0xca7143ed40acbf25,
0x9a36fd1c4ecd395b,
0xd98f047b651bd84f,
0x851cea0915469ca9,
0x6be0adc8e5da321f,
0x20da4fb66741fdad,
0xb109cfd48549e10d,
0x903d6fe27ad52cf7,
0x57d806a1afb24c87,
0x9adb854f275014a7,
0x3a4b7e2d01c762eb,
0x0691f53b736520cf,
0x53920a7889c4db5f,
0x183a9de6739ba6e3,
0xca190f84f75a39c3,
0xeb82df51f5ca4297,
0xcb278d4f14867da3,
0x702ebc53ad8e93b1,
0x3176ceba7be381f7,
0x34609f52bc4d9fa7,
0x284a5e197c0fe21d,
0x58369fb17ad124f7,
0x5abf763874e091df,
0x12e4ac8f63f7c805,
0xdf5ec0174e692851,
0x95a031cee7ad9c6f,
0xf92310b5fc12d79f,
0x84d69e2ca8903cd5,
0xcf72b913fde91649,
0xd3fa218961c05d23,
0xb6cf8e304873f1a1,
0xc238d907ef4357a3,
0x573a920dd9ef568b,
0x85a1f094385eda6f,
0x9cf4b2eafce6ba43
};
void msws_srand(void)
{
  static int i = 0;
  x = w = s = msws_seeds[i++];
}
#ifdef __cplusplus
}
#endif
