#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "ieee754.h"

#define MSB(w) ((int64_t)1 << ((w) - 1))
#define MASK(width) (((int64_t)1 << (width)) - 1)

uint8_t double_is_sub(double val) {
  union {
    double  d;
    int64_t i;
  } conv = { .d = val };
  int64_t man = conv.i & MASK(FP_DP_F_W);
  int32_t exp = (conv.i >> FP_DP_F_W) & MASK(FP_DP_EXP_W);
  if (!exp && man) {
    return 1;
  } else {
    return 0;
  }
}

uint8_t double_is_nan(double val) {
  union {
    double  d;
    int64_t i;
  } conv = { .d = val };
  int64_t man = conv.i & MASK(FP_DP_F_W);
  int32_t exp = (conv.i >> FP_DP_F_W) & MASK(FP_DP_EXP_W);
  if (exp == MASK(FP_DP_EXP_W) && man) {
    return 1;
  } else {
    return 0;
  }
}

uint8_t double_is_inf(double val) {
  union {
    double  d;
    int64_t i;
  } conv = { .d = val };
  int64_t man = conv.i & MASK(FP_DP_F_W);
  int32_t exp = (conv.i >> FP_DP_F_W) & MASK(FP_DP_EXP_W);
  if (exp == MASK(FP_DP_EXP_W) && !man) {
    return 1;
  } else {
    return 0;
  }
}

int8_t double_get_sign(double val) {
  union {
    double  d;
    int64_t i;
  } conv = { .d = val };
  int8_t sign = (conv.i >> (FP_DP_DATA_W - 1)) & 1;
  return sign;
}

int32_t double_get_exp(double val) {
  union {
    double  d;
    int64_t i;
  } conv = { .d = val };
  int32_t exp = (conv.i >> FP_DP_F_W) & MASK(FP_DP_EXP_W);
  exp -= FP_DP_BIAS;
  return exp;
}

int64_t double_get_man(double val) {
  union {
    double  d;
    int64_t i;
  } conv = { .d = val };
  int64_t man = conv.i & MASK(FP_DP_F_W);
  int32_t exp = (conv.i >> FP_DP_F_W) & MASK(FP_DP_EXP_W);
  if (!double_is_sub(val) && !double_is_nan(val) && exp && exp != MASK(FP_DP_EXP_W)) man |= MSB(FP_DP_MAN_W);
  return man;
}

int fprintf_d(FILE *fp, const char *name, double val) {
  union {
    double  d;
    int64_t i;
  } conv = { .d = val };
  int32_t exp = double_get_exp(val);
  int64_t man = double_get_man(val);
  int ret = fprintf(fp, "%s = %.3e (0x%016lx), man = 0x%014lx, exp = %d (0x%04hx), subnormal = %d, %s = %.16e\n", name, val, conv.i, man, exp, exp, double_is_sub(val), name, val);
  return ret;
}

char base2_str[29];

char *base2(double num_d) {
  if (num_d == 0.0) return "0.0";
  if (double_is_inf(num_d)) return "Inf";
  if (double_is_nan(num_d)) return "NaN";
  int32_t exp = double_get_exp(num_d);
  int64_t man = double_get_man(num_d);
  double man_b10 = man * pow(2, -FP_DP_F_W);
  if (double_get_sign(num_d)) man_b10 *= -1;
  sprintf(base2_str, "%.16f*2**%d", man_b10, exp);
  return base2_str;
}

uint8_t float_is_sub(float val) {
  union {
    float   f;
    int32_t i;
  } conv = { .f = val };
  int32_t man = conv.i & MASK(FP_SP_F_W);
  int32_t exp = (conv.i >> FP_SP_F_W) & MASK(FP_SP_EXP_W);
  if (exp == 0 && man) {
    return 1;
  } else {
    return 0;
  }
}

uint8_t float_is_nan(float val) {
  union {
    float   f;
    int32_t i;
  } conv = { .f = val };
  int32_t man = conv.i & MASK(FP_SP_F_W);
  int32_t exp = (conv.i >> FP_SP_F_W) & MASK(FP_SP_EXP_W);
  if (exp == MASK(FP_SP_EXP_W) && man) {
    return 1;
  } else {
    return 0;
  }
}

uint8_t float_is_inf(float val) {
  union {
    float   f;
    int32_t i;
  } conv = { .f = val };
  int32_t man = conv.i & MASK(FP_SP_F_W);
  int32_t exp = (conv.i >> FP_SP_F_W) & MASK(FP_SP_EXP_W);
  if (exp == MASK(FP_SP_EXP_W) && !man) {
    return 1;
  } else {
    return 0;
  }
}

int8_t float_get_sign(float val) {
  union {
    float   f;
    int32_t i;
  } conv = { .f = val };
  int8_t sign = (conv.i >> (FP_SP_DATA_W - 1)) & 1;
  return sign;
}

int32_t float_get_exp(float val) {
  union {
    float   f;
    int32_t i;
  } conv = { .f = val };
  int32_t exp = (conv.i >> FP_SP_F_W) & MASK(FP_SP_EXP_W);
  exp -= FP_SP_BIAS;
  return exp;
}

int32_t float_get_man(float val) {
  union {
    float  f;
    int32_t i;
  } conv = { .f = val };
  int32_t man = conv.i & MASK(FP_SP_F_W);
  int32_t exp = (conv.i >> FP_SP_F_W) & MASK(FP_SP_EXP_W);
  if (!float_is_sub(val) && !float_is_nan(val) && exp && exp != MASK(FP_SP_EXP_W)) man |= MSB(FP_SP_MAN_W);
  return man;
}

int fprintf_f(FILE *fp, const char *name, float val) {
  union {
    float   f;
    int32_t i;
  } conv = { .f = val };
  int32_t exp = float_get_exp(val);
  int32_t man = float_get_man(val);
  int ret = fprintf(fp, "%s = %.3e (0x%08x), man = 0x%06x, exp = %d (0x%04hx), subnormal = %d, %s = %.16e\n", name, val, conv.i, man, exp, exp, float_is_sub(val), name, val);
  return ret;
}
