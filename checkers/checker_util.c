
#ifdef __cplusplus
extern "C" {
#endif

#include "checker_util.h"

uint64_t ckr_shift_ul(uint64_t x)
{
  return x >> 9;
}
uint64_t ckr_shift_ur(uint64_t x)
{
  return x >> 7;
}
uint64_t ckr_shift_dl(uint64_t x)
{
  return x << 7;
}
uint64_t ckr_shift_dr(uint64_t x)
{
  return x << 9;
}
uint64_t ckr_shift2_ul(uint64_t x)
{
  return x >> 18;
}
uint64_t ckr_shift2_ur(uint64_t x)
{
  return x >> 14;
}
uint64_t ckr_shift2_dl(uint64_t x)
{
  return x << 14;
}
uint64_t ckr_shift2_dr(uint64_t x)
{
  return x << 18;
}

uint64_t ckr_radius2(uint64_t x)
{
  return ckr_shift2_dl(x) | ckr_shift2_dr(x) |
    ckr_shift2_ul(x) | ckr_shift2_ur(x);
}

uint64_t ckr_ls1b(uint64_t x)
{
  return x & (-x);
}
#ifdef __cplusplus
}
#endif
