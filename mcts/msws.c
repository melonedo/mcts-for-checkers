
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
0x8b5ad4ceb9c1fe73ull,
0x64d098b5c4f26d37ull,
0x45973acb0ad43b97ull,
0x6e9c5db170d261c9ull,
0x4fa75198bc653efbull,
0x3d4c562ea9451fedull,
0xd8b57104d2850b1bull,
0x2bdfe60a326fdab1ull,
0x86ced140fe30d875ull,
0x7c981fa6257d863dull,
0xe5f68a3c60d89be5ull,
0x8e46fc02dc40ef19ull,
0x02c34bf7bdef4593ull,
0xcebf7a1d87d2f0b5ull,
0x4bf18673ce3a69dfull,
0xcb5970f8cde89211ull,
0x5bf739adceaf657dull,
0x68cae093a37f0c4dull,
0xf1d465a8c08f3e93ull,
0xf713c8edd6e1bc75ull,
0x47890a62de4f2377ull,
0x86342a07413b8fd3ull,
0xf01527dccd9847f1ull,
0x762c03e1cf196381ull,
0x0578be1645839bf1ull,
0x24fb567a64bc5781ull,
0xa1b3cd0f91ba270full,
0xc98a21d45b2a9431ull,
0xdfb654c8e48fd765ull,
0xdb2065ecda3651c7ull,
0x836b054182405a1bull,
0xd0e762c56c28d4ebull,
0x5621ce7981a609efull,
0x012a385da13d9877ull,
0x62fac0712a7de945ull,
0x47a806b516a3c40full,
0x568c7b29209f1cb3ull,
0x593842cd1b3c45f3ull,
0x928c3ea0a27683edull,
0x8637cda4e932c467ull,
0x79e26ad8d269ea75ull,
0x15c2e63bba92f46dull,
0x0a5d93cf526ba9f7ull,
0x1b045792c480579bull,
0x902a7d858d0ac7b5ull,
0x4cd5f1a8c7832f6full,
0xec4d53fb9152ae4bull,
0xf281047e03e76c53ull,
0xd8eca931a5d6742full,
0x8325a0144dbc07ffull,
0xe61f4b27f5d231a5ull,
0xfce1746ade80c673ull,
0x90647bdc34a9218full,
0x5ab2617f208fec75ull,
0xc9b4f251230e4a51ull,
0xd821c75406dc17b9ull,
0xd12a47860ed64a85ull,
0xcd46a5e88bd27a3full,
0x63dce27ba9be6fd3ull,
0x9a586f3d1c8f5279ull,
0x791c862ff349d6b5ull,
0xe0f7cd51ae5917b3ull,
0x90d7c8a2d70893cbull,
0xc03a16244cf8615bull,
0x420187d684def529ull,
0x906ed7b875bc290full,
0x3b1205c9136a42b1ull,
0x78a6120ba429fc6full,
0xd2b59f7c47f305afull,
0x79bec51d86753bdbull,
0xc7196bef53e8adb1ull,
0x8da1e9f014e2b971ull,
0xade0542162adb087ull,
0x76ec9582ea5d2c97ull,
0x6a4eb513582fe311ull,
0x07c6b2d4069df413ull,
0x2a048ec5380cf6ebull,
0xd13748e580c1efb3ull,
0x0f92e13623ac9f57ull,
0xf8c015b7afe06187ull,
0x0b91ac676d093a25ull,
0xe361df48d4b85c9full,
0x69e2f0585e172f4dull,
0x5e7cdf98cfeb0387ull,
0xe4b6ad0895732e65ull,
0x3e1695a89f40e1b3ull,
0x4901e278d2f691adull,
0xf1904a781b56a043ull,
0x215490a8ae1bd289ull,
0x1d4eb50870c68a15ull,
0xdc5eb79815238eadull,
0xf02e4958673981edull,
0x1fa805474ea10683ull,
0x0e51f4670daef97dull,
0x83059fb6934d0ca3ull,
0x0af6c93535d20e1bull,
0x1b97a0e52cd9f1a3ull,
0x57b316c45b860d43ull,
0xce920ad3b8f347c3ull,
0x754a6d1247b18f59ull,
0xb736fd815d4c38ebull,
0x9e571c20b3ca8f13ull,
0x491028efe8d6f1c3ull,
0xca7143ed40acbf25ull,
0x9a36fd1c4ecd395bull,
0xd98f047b651bd84full,
0x851cea0915469ca9ull,
0x6be0adc8e5da321full,
0x20da4fb66741fdadull,
0xb109cfd48549e10dull,
0x903d6fe27ad52cf7ull,
0x57d806a1afb24c87ull,
0x9adb854f275014a7ull,
0x3a4b7e2d01c762ebull,
0x0691f53b736520cfull,
0x53920a7889c4db5full,
0x183a9de6739ba6e3ull,
0xca190f84f75a39c3ull,
0xeb82df51f5ca4297ull,
0xcb278d4f14867da3ull,
0x702ebc53ad8e93b1ull,
0x3176ceba7be381f7ull,
0x34609f52bc4d9fa7ull,
0x284a5e197c0fe21dull,
0x58369fb17ad124f7ull,
0x5abf763874e091dfull,
0x12e4ac8f63f7c805ull,
0xdf5ec0174e692851ull,
0x95a031cee7ad9c6full,
0xf92310b5fc12d79full,
0x84d69e2ca8903cd5ull,
0xcf72b913fde91649ull,
0xd3fa218961c05d23ull,
0xb6cf8e304873f1a1ull,
0xc238d907ef4357a3ull,
0x573a920dd9ef568bull,
0x85a1f094385eda6full,
0x9cf4b2eafce6ba43ull
};
void msws_srand(void)
{
  static int i = 0;
  x = w = s = msws_seeds[i++];
}
#ifdef __cplusplus
}
#endif
