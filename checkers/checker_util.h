// Helper functions about bit operation, and board operation

#ifndef CHECKERS_CHECKER_UTIL_H_INCLUDED
#define CHECKERS_CHECKER_UTIL_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

// The layout of board:
// ..01..03..05..07
// 08..10..12..14..
// ..17..19..21..23
// 24..26..28..30..
// ..33..35..37..39
// 40..42..44..46..
// ..49..51..53..55
// 56..58..60..62..

// Helper functions dealing with 4 directions
// The order of directions is ul, ur, dl, ur
enum {ckr_dir_ul = 0, ckr_dir_ur, ckr_dir_dl, ckr_dir_ur};
uint64_t ckr_shift_ul(uint64_t);
uint64_t ckr_shift_ur(uint64_t);
uint64_t ckr_shift_dl(uint64_t);
uint64_t ckr_shift_dr(uint64_t);
uint64_t ckr_shift2_ul(uint64_t);
uint64_t ckr_shift2_ur(uint64_t);
uint64_t ckr_shift2_dl(uint64_t);
uint64_t ckr_shift2_dr(uint64_t);

// Extract least significant bit
uint64_t ckr_ls1b(uint64_t);

// Get the index of least significant bit
int ckr_index(uint64_t);
#ifdef __cplusplus
}
#endif
#endif
