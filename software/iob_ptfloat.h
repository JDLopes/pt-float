#ifndef IOB_PTFLOAT_H
#define IOB_PTFLOAT_H

#include <stdint.h>

#include "ieee754.h"

//
//   PT-Float<DATA_W, EW_W>
//
// |<--------DATA_W-------->|
// |_______ _______ ________|
// |___E___|___F___|___EW___|
// |       |       |        |
// |<-EW-->|<-F_W->|<-EW_W->|
//

//
// Defines
//

#define MSB(w) ((int64_t)1 << ((w) - 1))
#define MASK(width) (int64_t)(((uint128_t)1 << (width)) - 1)

// PT-Float
#define DATA_W 32
#define EW_W   4

#define EW_MAX  ((1 << EW_W) - 1)
#define F_W(ew) (DATA_W - (ew) - EW_W)
#define F_MAX_W F_W(0)
#define F_MIN_W F_W(EW_MAX)

#define EXP_W(ew) ((ew)? ((ew) + 1): 0)
#define EXP_MAX_W (EW_MAX + 1)
#define MAN_W(ew) (F_W(ew) + 1)
#define MAN_MAX_W (F_MAX_W + 1)
#define MAN_MIN_W (F_MIN_W + 1)

#define EXP_MAX ((1 << EW_MAX) - 1)
#define EXP_MIN (-EXP_MAX+1)
#define MAN_MAX (((int64_t)1 << F_MIN_W) - 1)
#define MAN_MIN ((int64_t)1 << F_MIN_W)

#define RES_MAX_W (MAN_MAX_W+3)

#define PTFLOAT_W 32
#define UPK_MAN_W 64

#define PTFLOAT_MAX   (((int64_t)1 << DATA_W) - 1)
#define PTFLOAT_MIN   ((EXP_MAX << (F_MIN_W + EW_W)) | EW_MAX)
#define PTFLOAT_ZERO  EW_MAX
#define PTFLOAT_HALF  (0x8 << (DATA_W - 4))
#define PTFLOAT_ONE   ((0xc << (DATA_W - 4)) | 1)
#define PTFLOAT_TWO   ((0xa << (DATA_W - 4)) | 2)
#define PTFLOAT_M_ONE 0

//
// Variables
//

extern FILE *ptfloat_fp;

//
// Typedefs
//

typedef struct {
  int32_t exp;
  int64_t man;
  int32_t ew;
  } ptfloatUnpacked;

typedef int32_t ptfloat;

typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;

//
// Functions
//

ptfloatUnpacked ptfloat_unpack(ptfloat input);
ptfloatUnpacked ptfloat_add_sub(ptfloatUnpacked a, ptfloatUnpacked b, uint8_t *overflow, uint8_t op);
ptfloatUnpacked ptfloat_mul_(ptfloatUnpacked a, ptfloatUnpacked b, uint8_t *overflow, uint8_t *underflow);
ptfloatUnpacked ptfloat_div_(ptfloatUnpacked a, ptfloatUnpacked b, uint8_t *overflow, uint8_t *underflow, uint8_t *div_by_zero);
ptfloat ptfloat_pack(ptfloatUnpacked o, uint8_t *overflow);

#define ptfloat_add(a, b, overflow) ptfloat_pack(ptfloat_add_sub(ptfloat_unpack(a), ptfloat_unpack(b), overflow, 0), overflow)
#define ptfloat_sub(a, b, overflow) ptfloat_pack(ptfloat_add_sub(ptfloat_unpack(a), ptfloat_unpack(b), overflow, 1), overflow)
#define ptfloat_mul(a, b, overflow, underflow) ptfloat_pack(ptfloat_mul_(ptfloat_unpack(a), ptfloat_unpack(b), overflow, underflow), overflow)
#define ptfloat_div(a, b, overflow, underflow, div_by_zero) ptfloat_pack(ptfloat_div_(ptfloat_unpack(a), ptfloat_unpack(b), overflow, underflow, div_by_zero), overflow)

#define ptfloat_neg(a, overflow) ptfloat_sub(PTFLOAT_ZERO, a, overflow)

ptfloat double2ptfloat(double input, uint8_t *failed);
#define float2ptfloat(input, failed) double2ptfloat((double)input, failed)
double ptfloat2double(ptfloat input);
#define ptfloat2float(input) (float)ptfloat2double(input)

int8_t ptfloat_compare(ptfloatUnpacked a, ptfloatUnpacked b);
#define ptfloat_lt(a, b) ((ptfloat_compare(ptfloat_unpack(a), ptfloat_unpack(b)) < 0)? 1: 0)
#define ptfloat_le(a, b) ((ptfloat_compare(ptfloat_unpack(a), ptfloat_unpack(b)) <= 0)? 1: 0)
#define ptfloat_gt(a, b) ((ptfloat_compare(ptfloat_unpack(a), ptfloat_unpack(b)) > 0)? 1: 0)
#define ptfloat_ge(a, b) ((ptfloat_compare(ptfloat_unpack(a), ptfloat_unpack(b)) >= 0)? 1: 0)
#define ptfloat_eq(a, b) ((ptfloat_compare(ptfloat_unpack(a), ptfloat_unpack(b)) == 0)? 1: 0)

#define ptfloat_is_sub(val) ({\
  int32_t ew = EW_MAX & val;\
  int32_t e = val >> (DATA_W - ew);\
\
  uint8_t ret = (e == 0 && ew == EW_MAX)? 1: 0;\
\
  ret;\
})

void print_128(uint128_t data);

#endif
