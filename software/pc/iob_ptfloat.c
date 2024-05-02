#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>

#include "iob_ptfloat.h"

FILE *ptfloat_fp = 0;

#define LSHIFT(num, shift) ({\
  int64_t num_ = num;\
  if ((shift) < 64) num_ <<= (shift);\
  else num_ = 0;\
  num_;\
})

#define RSHIFT(num, shift) ({\
  int64_t num_ = num;\
  if ((shift) < 64) num_ >>= (shift);\
  else if (num_ < 0) num_ = -1;\
  else num_ = 0;\
  num_;\
})

#define cleadings(num, width) ({\
  int128_t num_ = num;\
  int32_t leadings_ = 0;\
\
  if (num_ <= 0) num_ = ~num_;\
\
  int32_t i;\
  for (i = 0; i < (width)-1; i++) {\
    if ((num_ << i) & MSB((width)-1)) break;\
    leadings_++;\
  }\
\
  leadings_;\
})

#define cew(exp) ({\
  int32_t ew_ = 0;\
  int32_t exp_ = exp;\
\
  if (exp_) {\
    if (exp_ < 0) exp_--;\
    int32_t leadings_ = cleadings(exp_, PTFLOAT_W);\
    ew_ = PTFLOAT_W - leadings_ - 1;\
  }\
\
  ew_;\
})

ptfloatUnpacked ptfloat_unpack(ptfloat input) {
  ptfloatUnpacked upk;
  int32_t ew = EW_MAX & input;
  int32_t e = (input << (PTFLOAT_W - DATA_W)) >> (DATA_W - ew + (PTFLOAT_W - DATA_W));
  int64_t f = ((int64_t)input << (ew + (UPK_MAN_W - DATA_W))) >> (ew + EW_W + (UPK_MAN_W - DATA_W));

  upk.exp = e ^ ((int32_t)MSB(PTFLOAT_W) >> (PTFLOAT_W - EXP_W(ew)));
  if (upk.exp < 0) upk.exp++;
  if (!ew) {
    upk.exp = 0;
  } else if (ew == EW_MAX && !e) {
    upk.exp++;
  }

  upk.man = f;
  if (!(ew == EW_MAX && !e)) {
    upk.man ^= MSB(UPK_MAN_W) >> (UPK_MAN_W - MAN_W(ew));
  }

  upk.ew = ew;

  return upk;
}

ptfloatUnpacked ptfloat_add_sub(ptfloatUnpacked a, ptfloatUnpacked b, uint8_t *overflow, uint8_t op) {
  ptfloatUnpacked o;

  *overflow = 0;

  int32_t diff = a.exp - b.exp;

  a.man <<= 3 + a.ew;
  b.man <<= 3 + b.ew;

  int8_t sticky;
  int32_t exp;
  int64_t man_a, man_b;
  if (diff >= 0) {
    exp = a.exp;
    man_a = a.man;
    sticky = (b.man & MASK(diff + 1))? 1 : 0;
    man_b = RSHIFT(b.man, diff);
    man_b |= sticky;
  } else {
    diff = -diff;
    exp = b.exp;
    sticky = (a.man & MASK(diff + 1))? 1 : 0;
    man_a = RSHIFT(a.man, diff);
    man_a |= sticky;
    man_b = b.man;
  }

  int32_t shift = 0;
  int64_t man;
  if (op) {
    man = man_a - man_b;

    if (((man_a & MSB(RES_MAX_W)) & (~man_b & MSB(RES_MAX_W)) & (~man & MSB(RES_MAX_W))) ||
        ((~man_a & MSB(RES_MAX_W)) & (man_b & MSB(RES_MAX_W)) & (man & MSB(RES_MAX_W)))) {
      shift = 1;
    }
  } else {
    man = man_a + man_b;

    if (((man_a & MSB(RES_MAX_W)) & (man_b & MSB(RES_MAX_W)) & (~man & MSB(RES_MAX_W))) ||
        ((~man_a & MSB(RES_MAX_W)) & (~man_b & MSB(RES_MAX_W)) & (man & MSB(RES_MAX_W)))) {
      shift = 1;
    }
  }

  exp += shift;
  sticky |= (man & MASK(shift + 1))? 1 : 0;
  man = (man >> shift) | sticky;

  if (exp <= EXP_MAX) {
    if (man) {
      int32_t leadings = cleadings(man, RES_MAX_W);
      if ((exp - leadings) < EXP_MIN) {
        leadings = exp - EXP_MIN;
      }
      o.exp = exp - leadings;
      o.man = man << leadings;
    } else {
      o.exp = EXP_MIN;
      o.man = 0;
    }
  } else {
    o.exp = exp;
    o.man = man;
  }

  if (o.exp > EXP_MAX) {
    *overflow = 1;
    o.exp = EXP_MAX;
    o.man = MAN_MAX << (EW_MAX + 3);
    if (man < 0) {
      o.man = MAN_MIN << (EW_MAX + 3);
    }
  }

  return o;
}

ptfloatUnpacked ptfloat_mul_(ptfloatUnpacked a, ptfloatUnpacked b, uint8_t *overflow, uint8_t *underflow) {
  ptfloatUnpacked o;

  *overflow = 0;
  *underflow = 0;

  int128_t man = (int128_t)(a.man << a.ew) * (int128_t)(b.man << b.ew);

  int32_t exp = man? a.exp + b.exp : EXP_MIN;

  int8_t sticky = man & 1;
  uint8_t man_msb  = (man >> ((MAN_MAX_W << 1) - 1)) & 1;
  uint8_t man_msb2 = (man >> ((MAN_MAX_W << 1) - 2)) & 1;
  if (man_msb != man_msb2) {
    exp++;
    man >>= 1;
  }

  int32_t shift = MAN_MAX_W - 3 - 1;
  int32_t lds = cleadings(man, (MAN_MAX_W << 1) - 1);
  if ((exp - lds) < EXP_MIN) {
    lds = exp - EXP_MIN;
  }
  exp -= lds;
  shift -= lds;

  sticky |= (man & MASK(shift + 1))? 1 : 0;
  man = (man >> shift) | sticky;

  if (exp <= EXP_MAX && exp > EXP_MIN) {
    if (man) {
      int32_t leadings = cleadings(man, RES_MAX_W);
      if ((exp - leadings) < EXP_MIN) {
        leadings = exp - EXP_MIN;
      }
      o.exp = exp - leadings;
      o.man = man << leadings;
    } else {
      o.exp = EXP_MIN;
      o.man = 0;
    }
  } else {
    o.exp = exp;
    o.man = man;
  }

  if (o.exp > EXP_MAX) {
    *overflow = 1;
    o.exp = EXP_MAX;
    o.man = MAN_MAX << (EW_MAX + 3);
    if (man < 0) {
      o.man = MAN_MIN << (EW_MAX + 3);
    }
  } else if (o.exp < EXP_MIN - MAN_MIN_W) {
    *underflow = 1;
    o.exp = EXP_MIN;
    o.man = 0;
  }

  if (o.exp < EXP_MIN) {
    shift = EXP_MIN - o.exp;
    o.exp = EXP_MIN;
    sticky = (o.man & MASK(shift + 1))? 1 : 0;
    o.man = RSHIFT(o.man, shift) | sticky;
  }

  return o;
}

ptfloatUnpacked ptfloat_div_(ptfloatUnpacked a, ptfloatUnpacked b, uint8_t *overflow, uint8_t *underflow, uint8_t *div_by_zero) {
  ptfloatUnpacked o;

  *overflow = 0;
  *underflow = 0;
  *div_by_zero = 0;

  uint128_t dividend = a.man << a.ew;
  int32_t shift = cleadings(dividend, MAN_MAX_W);
  dividend <<= shift;
  a.exp -= shift;
  if (a.man < 0) {
    dividend = -dividend;
  }
  dividend <<= RES_MAX_W - 1;

  uint128_t divisor = b.man << b.ew;
  shift = cleadings(divisor, MAN_MAX_W);
  divisor <<= shift;
  b.exp -= shift;
  if (b.man < 0) {
    divisor = -divisor;
  }

  int32_t exp = a.man? a.exp - b.exp : EXP_MIN;

  if (divisor == 0) {
    *div_by_zero = 1;
    o.exp = 0;
    o.man = 0;
    return o;
  }

  int32_t i, bit;
  uint128_t quotient = 0, remainder = 0;
  for (i = 0; i < (MAN_MAX_W << 1) + 3; i++) {
    if (dividend & MSB((MAN_MAX_W << 1) + 3)) {
      bit = 1;
    } else {
      bit = 0;
    }

    quotient <<= 1;
    remainder <<= 1;
    remainder |= bit;
    dividend <<= 1;

    if (remainder >= divisor) {
      quotient |= 1;
      remainder -= divisor;
    }
  }

  if (((a.man > 0 && b.man < 0) || (a.man < 0 && b.man > 0))) {
    quotient = -quotient;
  }

  int8_t sticky = quotient & 1;
  uint8_t man_msb = (quotient >> RES_MAX_W) & 1;
  uint8_t man_msb2 = (quotient >> (RES_MAX_W-1)) & 1;
  if (man_msb != man_msb2) {
    exp++;
    sticky |= (quotient & 3)? 1 : 0;
    quotient = (quotient >> 1) | sticky;
  }
  int64_t man = quotient;

  if (exp <= EXP_MAX && exp > EXP_MIN) {
    if (man) {
      int32_t leadings = cleadings(man, RES_MAX_W);
      if ((exp - leadings) < EXP_MIN) {
        leadings = exp - EXP_MIN;
      }
      o.exp = exp - leadings;
      o.man = man << leadings;
    } else {
      o.exp = EXP_MIN;
      o.man = 0;
    }
  } else {
    o.exp = exp;
    o.man = man;
  }

  if (o.exp > EXP_MAX) {
    *overflow = 1;
    o.exp = EXP_MAX;
    o.man = MAN_MAX << (EW_MAX + 3);
    if (man < 0) {
      o.man = MAN_MIN << (EW_MAX + 3);
    }
  } else if (exp < EXP_MIN - MAN_MIN_W) { // MAN_MAX_W -> MAN_MIN_W
    *underflow = 1;
    o.exp = EXP_MIN;
    o.man = 0;
  }

  if (o.exp < EXP_MIN) {
    shift = EXP_MIN - o.exp;
    o.exp = EXP_MIN;
    sticky = (o.man & MASK(shift + 1))? 1 : 0;
    o.man = RSHIFT(o.man, shift) | sticky;
  }

  return o;
}

ptfloat ptfloat_pack(ptfloatUnpacked o, uint8_t *overflow) {
  int32_t packed;

  int32_t exp = o.exp;
  int32_t ew = cew(exp);

  int32_t shift = ew + 3;
  int64_t man = o.man >> shift;

  uint8_t man_lsb    = (o.man & (1 << shift))? 1: 0;
  uint8_t guard_bit  = (o.man & (1 << (shift - 1)))? 1: 0;
  uint8_t round_bit  = (o.man & (1 << (shift - 2)))? 1: 0;
  uint8_t sticky_bit = (o.man & ((1 << (shift - 2)) - 1))? 1: 0;

  if (guard_bit & (man_lsb | round_bit | sticky_bit)) {
    int64_t res = man + 1;
    shift = 0;
    if ((~man & MSB(MAN_W(ew))) & (res & MSB(MAN_W(ew)))) {
      exp++;
      int32_t new_ew = cew(exp);
      res >>= new_ew - ew + 1;
      ew = new_ew;
    }
    man = res;

    if (exp <= EXP_MAX) {
      if (man) {
        int32_t leadings = cleadings(man, MAN_W(ew));
        if ((exp - leadings) < EXP_MIN) {
          leadings = exp - EXP_MIN;
        }
        exp -= leadings;
        int32_t new_ew = cew(exp);
        shift += new_ew - ew - leadings;
        ew = new_ew;
      }
    }

    if (shift < 0) {
      man <<= -shift;
    } else {
      man >>= shift;
    }

    if (exp > EXP_MAX) {
      *overflow = 1;
      ew = EW_MAX;
      exp = EXP_MAX;
      man = MAN_MAX;
      if (man < 0) {
        man = MAN_MIN;
      }
    }
  }

  uint8_t man_msb  = (man >> (MAN_W(ew) - 1)) & 1;
  uint8_t man_msb2 = (man >> (MAN_W(ew) - 2)) & 1;
  if (exp == EXP_MIN && man_msb == man_msb2) {
    exp = 0;
  }

  if (exp < 0) {
    exp--;
  }
  exp &= MASK(ew);

  man &= MASK(F_W(ew));

  packed = ew;
  packed |= man << EW_W;
  if (ew) {
    packed |= exp << (F_W(ew) + EW_W);
  }

  return packed;
}

ptfloat double2ptfloat(double input, uint8_t *failed) {
  union {
    double   d;
    uint64_t i;
  } conv = { .d = input };
  ptfloatUnpacked upk;
  int32_t shift = 0;
  uint8_t overflow = 0;

  *failed = 0;

  // Fields extraction
  int32_t sign = conv.i >> (FP_DP_DATA_W - 1);
  int32_t exp = (conv.i << 1) >> (FP_DP_DATA_W - FP_DP_EXP_W);
  int64_t man = (conv.i << (1 + FP_DP_EXP_W)) >> (FP_DP_DATA_W - FP_DP_F_W);

  if (!exp) { // Denormalized or Zero
    if (!man) {
      exp = EXP_MIN;
      sign = 0;
    } else {
      int32_t leadings = cleadings(man, FP_DP_MAN_W);
      shift = FP_DP_F_W - leadings - RES_MAX_W + 1;

      exp += -FP_DP_BIAS + shift - FP_DP_MAN_W - RES_MAX_W;
    }
  } else if (exp == MASK(FP_DP_EXP_W)) { // Infinity or NAN
    *failed = 1;
  } else { // Normalized
    man |= MSB(FP_DP_MAN_W);
    exp -= FP_DP_BIAS - 1;

    shift = FP_DP_MAN_W - RES_MAX_W + 1;
  }

  if (sign) {
    man = -man;
  }

  if (exp < EXP_MIN) {
    shift += EXP_MIN - exp;
    exp = EXP_MIN;
    if (shift > FP_DP_F_W) {
      shift = FP_DP_F_W + 1;
    }
    if (shift < -FP_DP_F_W) {
      shift = -64;
    }
  }

  int8_t sticky = 0;
  if (shift < 0) {
    man = LSHIFT(man, -shift);
  } else {
    sticky = (man & MASK(shift + 1))? 1 : 0;
    man = RSHIFT(man, shift) | sticky;
  }

  if (exp <= EXP_MAX && exp > EXP_MIN) {
    if (man) {
      int32_t leadings = cleadings(man, RES_MAX_W);
      upk.exp = exp - leadings;
      upk.man = man << leadings;
    } else {
      upk.exp = EXP_MIN;
      upk.man = 0;
    }
  } else {
    upk.exp = exp;
    upk.man = man;
  }

  if (*failed) {
    upk.man = 0;
    upk.exp = 0;
  }

  return ptfloat_pack(upk, &overflow);
}

double ptfloat2double(ptfloat input) {
  union {
    double  d;
    int64_t i;
  } conv;

  ptfloatUnpacked upk = ptfloat_unpack(input);

  int64_t sign = (upk.man >> (UPK_MAN_W-1)) & 1;
  int64_t exp = upk.exp;
  int64_t man = upk.man << (FP_DP_MAN_W - MAN_MAX_W + upk.ew);

  if (sign) {
    man = -man;
  }

  exp += FP_DP_BIAS;
  if (upk.exp == EXP_MIN && !man) { // Zero
    exp = 0;
  }

  if (exp > 0) {
    int32_t leadings = cleadings(man, FP_DP_MAN_W+1);
    if ((exp - leadings) < 0) {
      leadings = exp;
    }
    exp -= leadings;
    man <<= leadings;
  } else if (exp < 0) {
    man <<= 3;

    int32_t shift = -exp;
    int8_t sticky = (man & MASK(shift + 1))? 1 : 0;
    man = RSHIFT(man, shift) | sticky;
    exp = 0;

    uint8_t man_lsb    = (man & (1 << 3))? 1: 0;
    uint8_t guard_bit  = (man & (1 << 2))? 1: 0;
    uint8_t round_bit  = (man & (1 << 1))? 1: 0;
    uint8_t sticky_bit = (man & ((1 << 1) - 1))? 1: 0;
    man >>= 3;

    if (guard_bit & (man_lsb | round_bit | sticky_bit)) {
      man++;
    }
  }

  if (exp >= MASK(FP_DP_EXP_W)) { // Infinity
    exp = MASK(FP_DP_EXP_W);
    man = 0;
  }

  man &= MASK(FP_DP_F_W);

  conv.i = man;
  conv.i |= exp << FP_DP_F_W;
  conv.i |= sign << (FP_DP_DATA_W - 1);

  return conv.d;
}

int8_t ptfloat_compare(ptfloatUnpacked a, ptfloatUnpacked b) {
  int8_t res;

  a.man <<= a.ew;
  b.man <<= b.ew;

  if (a.exp > b.exp) {
    res = 1;
    if (a.man < 0) res = -1;
  } else if (a.exp < b.exp) {
    res = -1;
    if (b.man < 0) res = 1;
  } else {
    res = 0;
    if (a.man > b.man) res = 1;
    else if (a.man < b.man) res = -1;
  }

  return res;
}

void print_128(uint128_t data) {
  uint64_t low = data;
  uint64_t high = data >> 64;
  printf("0x%016lx%016lx\n", high, low);
  return;
}
