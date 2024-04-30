#ifndef IEEE754_H
#define IEEE754_H

#include <stdint.h>

#define FP_DP_DATA_W 64
#define FP_DP_EXP_W  11
#define FP_DP_BIAS   1023
#define FP_DP_F_W    52
#define FP_DP_MAN_W  (FP_DP_F_W + 1)

#define FP_SP_DATA_W 32
#define FP_SP_EXP_W  8
#define FP_SP_BIAS   127
#define FP_SP_F_W    23
#define FP_SP_MAN_W  (FP_SP_F_W + 1)

uint8_t double_is_sub(double val);
uint8_t double_is_nan(double val);
uint8_t double_is_inf(double val);
int8_t double_get_sign(double val);
int32_t double_get_exp(double val);
int64_t double_get_man(double val);

int fprintf_d(FILE *fp, const char *name, double val);
#define printf_d(name, val) fprintf_d(stdout, name, val)

char *base2(double num_d);

uint8_t float_is_sub(float val);
uint8_t float_is_nan(float val);
uint8_t float_is_inf(float val);
int8_t float_get_sign(float val);
int32_t float_get_exp(float val);
int32_t float_get_man(float val);

int fprintf_f(FILE *fp, const char *name, float val);
#define printf_f(name, val) fprintf_f(stdout, name, val)

#endif
