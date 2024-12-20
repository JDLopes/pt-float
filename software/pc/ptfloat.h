#ifndef PTFLOAT_H
#define PTFLOAT_H

#include <iostream>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "iob_ptfloat.h"

extern uint8_t failed, overflow, underflow, div_by_zero;

class Ptfloat {
    private:
        ptfloat val;
    public:

        //
        // Declarations
        //

        Ptfloat(): val(float2ptfloat(0.0, &failed)) {};
        Ptfloat(float f): val(float2ptfloat(f, &failed)) {};
        Ptfloat(double d): val(double2ptfloat(d, &failed)) {};
        Ptfloat(long int i): val(double2ptfloat((double)i, &failed)) {};
        Ptfloat(long unsigned int i): val(double2ptfloat((double)i, &failed)) {};
        Ptfloat(int i): val(double2ptfloat((double)i, &failed)) {};
        Ptfloat(short unsigned int i): val(double2ptfloat((double)i, &failed)) {};
        Ptfloat(unsigned char i): val(double2ptfloat((double)i, &failed)) {};

        //
        // Conversion operators
        //

        operator float() { return ptfloat2float(val); }
        operator double() { return ptfloat2double(val); }
        operator long int() { return (long int)ptfloat2double(val); }
        operator long unsigned int() { return (long unsigned int)ptfloat2double(val); }
        operator int() { return (int)ptfloat2double(val); }
        operator int() const { return (int)ptfloat2double(val); }
        operator short unsigned int() { return (short unsigned int)ptfloat2double(val); }
        operator unsigned char() { return (unsigned char)ptfloat2double(val); }
        operator bool() { return (bool)ptfloat2double(val); }

        //
        // Arithmetic operators
        //

        // Addition
        Ptfloat operator++() {
            Ptfloat res = 1.0;
            res.val = ptfloat_add(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator++( int ) {
            Ptfloat res;
            res.val = val;
            val = ptfloat_add(val, ((Ptfloat)1.0).val, &overflow);
            return res;
        }

        Ptfloat operator+(Ptfloat const& other) const {
            Ptfloat res;
            res.val = ptfloat_add(val, other.val, &overflow);
            return res;
        }

        Ptfloat operator+(Ptfloat const& other) {
            Ptfloat res;
            res.val = ptfloat_add(val, other.val, &overflow);
            return res;
        }

        Ptfloat operator+(double const other) {
            Ptfloat res = other;
            res.val = ptfloat_add(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator+(float const other) {
            Ptfloat res = other;
            res.val = ptfloat_add(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator+(long unsigned int const other) {
            Ptfloat res = other;
            res.val = ptfloat_add(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator+(int const other) {
            Ptfloat res = other;
            res.val = ptfloat_add(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator+(unsigned char const other) {
            Ptfloat res = other;
            res.val = ptfloat_add(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator+=(Ptfloat const& other) {
            return val = ptfloat_add(val, other.val, &overflow);
        }

        // Subtraction
        Ptfloat operator--() {
            Ptfloat res = 1.0;
            res.val = ptfloat_sub(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator--( int ) {
            Ptfloat res;
            res.val = val;
            val = ptfloat_sub(val, ((Ptfloat)1.0).val, &overflow);
            return res;
        }

        Ptfloat operator-() const {
            Ptfloat res;
            res.val = ptfloat_neg(val, &overflow);
            return res;
        }

        Ptfloat operator-() {
            Ptfloat res;
            res.val = ptfloat_neg(val, &overflow);
            return res;
        }

        Ptfloat operator-(Ptfloat const& other) const {
            Ptfloat res;
            res.val = ptfloat_sub(val, other.val, &overflow);
            return res;
        }

        Ptfloat operator-(Ptfloat const& other) {
            Ptfloat res;
            res.val = ptfloat_sub(val, other.val, &overflow);
            return res;
        }

        Ptfloat operator-(double const other) {
            Ptfloat res = other;
            res.val = ptfloat_sub(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator-(float const other) {
            Ptfloat res = other;
            res.val = ptfloat_sub(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator-(long unsigned int const other) {
            Ptfloat res = other;
            res.val = ptfloat_sub(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator-(int const other) {
            Ptfloat res = other;
            res.val = ptfloat_sub(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator-(unsigned char const other) {
            Ptfloat res = other;
            res.val = ptfloat_sub(val, res.val, &overflow);
            return res;
        }

        Ptfloat operator-=(Ptfloat const& other) {
            return val = ptfloat_sub(val, other.val, &overflow);
        }

        // Multiplication
        Ptfloat operator*(Ptfloat const& other) const {
            Ptfloat res;
            res.val = ptfloat_mul(val, other.val, &overflow, &underflow);
            return res;
        }

        Ptfloat operator*(Ptfloat const& other) {
            Ptfloat res;
            res.val = ptfloat_mul(val, other.val, &overflow, &underflow);
            return res;
        }

        Ptfloat operator*(double const other) {
            Ptfloat res = other;
            res.val = ptfloat_mul(val, res.val, &overflow, &underflow);
            return res;
        }

        Ptfloat operator*(float const other) {
            Ptfloat res = other;
            res.val = ptfloat_mul(val, res.val, &overflow, &underflow);
            return res;
        }

        Ptfloat operator*(long unsigned int const other) {
            Ptfloat res = other;
            res.val = ptfloat_mul(val, res.val, &overflow, &underflow);
            return res;
        }

        Ptfloat operator*(int const other) {
            Ptfloat res = other;
            res.val = ptfloat_mul(val, res.val, &overflow, &underflow);
            return res;
        }

        Ptfloat operator*(unsigned char const other) {
            Ptfloat res = other;
            res.val = ptfloat_mul(val, res.val, &overflow, &underflow);
            return res;
        }

        Ptfloat operator*=(Ptfloat const& other) {
            return val = ptfloat_mul(val, other.val, &overflow, &underflow);
        }

        // Division
        Ptfloat operator/(Ptfloat const& other) const {
            Ptfloat res;
            res.val = ptfloat_div(val, other.val, &overflow, &underflow, &div_by_zero);
            return res;
        }

        Ptfloat operator/(Ptfloat const& other) {
            Ptfloat res;
            res.val = ptfloat_div(val, other.val, &overflow, &underflow, &div_by_zero);
            return res;
        }

        Ptfloat operator/(double const other) {
            Ptfloat res = other;
            res.val = ptfloat_div(val, res.val, &overflow, &underflow, &div_by_zero);
            return res;
        }

        Ptfloat operator/(float const other) {
            Ptfloat res = other;
            res.val = ptfloat_div(val, res.val, &overflow, &underflow, &div_by_zero);
            return res;
        }

        Ptfloat operator/(long unsigned int const other) {
            Ptfloat res = other;
            res.val = ptfloat_div(val, res.val, &overflow, &underflow, &div_by_zero);
            return res;
        }

        Ptfloat operator/(int const other) {
            Ptfloat res = other;
            res.val = ptfloat_div(val, res.val, &overflow, &underflow, &div_by_zero);
            return res;
        }

        Ptfloat operator/(unsigned char const other) {
            Ptfloat res = other;
            res.val = ptfloat_div(val, res.val, &overflow, &underflow, &div_by_zero);
            return res;
        }

        Ptfloat operator/=(Ptfloat const& other) {
            return val = ptfloat_div(val, other.val, &overflow, &underflow, &div_by_zero);
        }

        //
        // Comparator operators
        //

        bool operator<(Ptfloat const& other) { return (bool)ptfloat_lt(val, other.val); }
        bool operator<=(Ptfloat const& other) { return (bool)ptfloat_le(val, other.val); }
        bool operator>(Ptfloat const& other) { return (bool)ptfloat_gt(val, other.val); }
        bool operator>=(Ptfloat const& other) { return (bool)ptfloat_ge(val, other.val); }
        bool operator==(Ptfloat const& other) { return (bool)ptfloat_eq(val, other.val); }
        bool operator!=(Ptfloat const& other) { return (bool)!ptfloat_eq(val, other.val); }

        //
        // Input/Output
        //

        friend std::ostream &operator<<(std::ostream &output, const Ptfloat &other ) {
            output << ptfloat2double(other.val);
         return output;
        }

        friend std::istream &operator>>(std::istream  &input, Ptfloat &other ) {
            double value;
            input >> value;
            other.val = double2ptfloat(value, &failed);
            return input;
        }
};

//
// More arithmetic operators
//

inline Ptfloat operator+(double d, Ptfloat ptf) { return (Ptfloat)d + ptf; }
inline Ptfloat operator+(float f, Ptfloat ptf) { return (Ptfloat)f + ptf; }
inline Ptfloat operator+(long unsigned int i, Ptfloat ptf) { return (Ptfloat)i + ptf; }
inline Ptfloat operator+(int i, Ptfloat ptf) { return (Ptfloat)i + ptf; }
inline Ptfloat operator+(unsigned char i, Ptfloat ptf) { return (Ptfloat)i + ptf; }

inline Ptfloat operator-(double d, Ptfloat ptf) { return (Ptfloat)d - ptf; }
inline Ptfloat operator-(float f, Ptfloat ptf) { return (Ptfloat)f - ptf; }
inline Ptfloat operator-(long unsigned int i, Ptfloat ptf) { return (Ptfloat)i - ptf; }
inline Ptfloat operator-(int i, Ptfloat ptf) { return (Ptfloat)i - ptf; }
inline Ptfloat operator-(unsigned char i, Ptfloat ptf) { return (Ptfloat)i - ptf; }

inline Ptfloat operator*(double d, Ptfloat ptf) { return (Ptfloat)d * ptf; }
inline Ptfloat operator*(float f, Ptfloat ptf) { return (Ptfloat)f * ptf; }
inline Ptfloat operator*(long unsigned int i, Ptfloat ptf) { return (Ptfloat)i * ptf; }
inline Ptfloat operator*(int i, Ptfloat ptf) { return (Ptfloat)i * ptf; }
inline Ptfloat operator*(unsigned char i, Ptfloat ptf) { return (Ptfloat)i * ptf; }

inline Ptfloat operator/(double d, Ptfloat ptf) { return (Ptfloat)d / ptf; }
inline Ptfloat operator/(float f, Ptfloat ptf) { return (Ptfloat)f / ptf; }
inline Ptfloat operator/(long unsigned int i, Ptfloat ptf) { return (Ptfloat)i / ptf; }
inline Ptfloat operator/(int i, Ptfloat ptf) { return (Ptfloat)i / ptf; }
inline Ptfloat operator/(unsigned char i, Ptfloat ptf) { return (Ptfloat)i / ptf; }

//
// More comparator operators
//

inline bool operator<(double d, Ptfloat ptf) { return (Ptfloat)d < ptf; }
inline bool operator<(float f, Ptfloat ptf) { return (Ptfloat)f < ptf; }
inline bool operator<(int i, Ptfloat ptf) { return (Ptfloat)i < ptf; }
inline bool operator<(Ptfloat ptf, double d) { return ptf < (Ptfloat)d; }
inline bool operator<(Ptfloat ptf, float f) { return ptf < (Ptfloat)f; }
inline bool operator<(Ptfloat ptf, int i) { return ptf < (Ptfloat)i; }

inline bool operator<=(double d, Ptfloat ptf) { return (Ptfloat)d <= ptf; }
inline bool operator<=(float f, Ptfloat ptf) { return (Ptfloat)f <= ptf; }
inline bool operator<=(int i, Ptfloat ptf) { return (Ptfloat)i <= ptf; }
inline bool operator<=(Ptfloat ptf, double d) { return ptf <= (Ptfloat)d; }
inline bool operator<=(Ptfloat ptf, float f) { return ptf <= (Ptfloat)f; }
inline bool operator<=(Ptfloat ptf, int i) { return ptf <= (Ptfloat)i; }

inline bool operator>(double d, Ptfloat ptf) { return (Ptfloat)d > ptf; }
inline bool operator>(float f, Ptfloat ptf) { return (Ptfloat)f > ptf; }
inline bool operator>(int i, Ptfloat ptf) { return (Ptfloat)i > ptf; }
inline bool operator>(Ptfloat ptf, double d) { return ptf > (Ptfloat)d; }
inline bool operator>(Ptfloat ptf, float f) { return ptf > (Ptfloat)f; }
inline bool operator>(Ptfloat ptf, int i) { return ptf > (Ptfloat)i; }

inline bool operator>=(double d, Ptfloat ptf) { return (Ptfloat)d >= ptf; }
inline bool operator>=(float f, Ptfloat ptf) { return (Ptfloat)f >= ptf; }
inline bool operator>=(int i, Ptfloat ptf) { return (Ptfloat)i >= ptf; }
inline bool operator>=(Ptfloat ptf, double d) { return ptf >= (Ptfloat)d; }
inline bool operator>=(Ptfloat ptf, float f) { return ptf >= (Ptfloat)f; }
inline bool operator>=(Ptfloat ptf, int i) { return ptf >= (Ptfloat)i; }

inline bool operator==(double d, Ptfloat ptf) { return (Ptfloat)d == ptf; }
inline bool operator==(float f, Ptfloat ptf) { return (Ptfloat)f == ptf; }
inline bool operator==(int i, Ptfloat ptf) { return (Ptfloat)i == ptf; }
inline bool operator==(Ptfloat ptf, double d) { return ptf == (Ptfloat)d; }
inline bool operator==(Ptfloat ptf, float f) { return ptf == (Ptfloat)f; }
inline bool operator==(Ptfloat ptf, int i) { return ptf == (Ptfloat)i; }

inline bool operator!=(double d, Ptfloat ptf) { return (Ptfloat)d != ptf; }
inline bool operator!=(float f, Ptfloat ptf) { return (Ptfloat)f != ptf; }
inline bool operator!=(int i, Ptfloat ptf) { return (Ptfloat)i != ptf; }
inline bool operator!=(Ptfloat ptf, double d) { return ptf != (Ptfloat)d; }
inline bool operator!=(Ptfloat ptf, float f) { return ptf != (Ptfloat)f; }
inline bool operator!=(Ptfloat ptf, int i) { return ptf != (Ptfloat)i; }

//
// Math functions
//

inline Ptfloat cos(Ptfloat x) { return (Ptfloat)cos((double)x); }
inline Ptfloat sin(Ptfloat x) { return (Ptfloat)sin((double)x); }
inline Ptfloat acos(Ptfloat x) { return (Ptfloat)acos((double)x); }
inline Ptfloat exp(Ptfloat x) { return (Ptfloat)exp((double)x); }
inline Ptfloat frexp(Ptfloat x, int *e) { return (Ptfloat)frexp((double)x, e); }
inline Ptfloat log(Ptfloat x) { return (Ptfloat)log((double)x); }
inline Ptfloat log2(Ptfloat x) { return (Ptfloat)log2((double)x); }
inline Ptfloat pow(Ptfloat x, Ptfloat y) { return (Ptfloat)pow((double)x, (double)y); }
inline Ptfloat pow(Ptfloat x, double y) { return (Ptfloat)pow((double)x, y); }
inline Ptfloat pow(double x, Ptfloat y) { return (Ptfloat)pow(x, (double)y); }
inline Ptfloat pow(float x, Ptfloat y) { return (Ptfloat)pow(x, (double)y); }
inline Ptfloat pow(int x, Ptfloat y) { return (Ptfloat)pow(x, (double)y); }
inline Ptfloat sqrt(Ptfloat x) { return (Ptfloat)sqrt((double)x); }
inline Ptfloat ceil(Ptfloat x) { return (Ptfloat)ceil((double)x); }
inline Ptfloat fabs(Ptfloat x) { return (Ptfloat)fabs((double)x); }
inline Ptfloat floor(Ptfloat x) { return (Ptfloat)floor((double)x); }

//
// Standard functions
//

inline Ptfloat *memset(Ptfloat *ptr, int val, size_t n) {
  n /= sizeof(Ptfloat);
  while (n) ptr[--n] = val;
  return ptr;
}

inline void *memcpy(Ptfloat *ptf_ptr, const double *fp_ptr, size_t n) {
  n /= sizeof(double);
  while (n--) ptf_ptr[n] = (Ptfloat)fp_ptr[n];
  return (void *)ptf_ptr;
}

inline void *memcpy(Ptfloat *ptf_ptr, const float *fp_ptr, size_t n) {
  n /= sizeof(float);
  while (n--) ptf_ptr[n] = (Ptfloat)fp_ptr[n];
  return (void *)ptf_ptr;
}

inline void *memcpy(Ptfloat *ptf_ptr, const int *i_ptr, size_t n) {
  n /= sizeof(int);
  while (n--) ptf_ptr[n] = (Ptfloat)i_ptr[n];
  return (void *)ptf_ptr;
}

inline void *calloc_ptf(size_t nitems, size_t size) {
  Ptfloat *ptr = (Ptfloat *)malloc(size*nitems);
  memset(ptr, 0, size*nitems);
  return (void *)ptr;
}

#define printf printf_ptf
#define fprintf fprintf_ptf

#define scanf scanf_ptf
#define fscanf fscanf_ptf
#define sscanf sscanf_ptf

inline size_t getlen_(const char * str) {
  size_t len = 0;
  if (!str) return len;
  while (str[len] != '%' && str[len]) len++;
  while (str[len] != 'c' &&
         str[len] != 's' &&
         str[len] != 'd' && str[len] != 'i' &&
         str[len] != 'e' && str[len] != 'E' &&
         str[len] != 'f' && str[len] != 'F' &&
         str[len] != 'g' && str[len] != 'G' &&
         str[len] != 'o' &&
         str[len] != 'u' &&
         str[len] != 'x' && str[len] != 'X' &&
         str[len] != 'p' &&
         str[len] != 'n' &&
         str[len])
    len++;
  if (str[len]) len++;
  return len;
}

inline int vsnprintf_ptf(char *s, size_t n, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int ret = vsnprintf(s, n, fmt, args);
  va_end(args);
  return ret;
}

inline int vfprintf_ptf(FILE *fp, const char *fmt, va_list args) {
  size_t len = getlen_(fmt);
  if (!len) return 0;
  char *fmt_ptf = (char *) malloc(sizeof(char) * (len+1));
  memcpy(fmt_ptf, fmt, len);
  fmt_ptf[len] = 0;

  int ret;
  if (fmt_ptf[len-1] == 'f' ||
      fmt_ptf[len-1] == 'e' ||
      fmt_ptf[len-1] == 'g') {
    union {
      Ptfloat ptf;
      int   i;
    } conv = { .i = va_arg(args, int)};
    char buf[1024];
    ret = vsnprintf_ptf(buf, 1023, fmt_ptf, (double)conv.ptf);
    fputs(buf, fp);
  } else {
    ret = vfprintf(fp, fmt_ptf, args);
  }
  free(fmt_ptf);
  return ret + vfprintf_ptf(fp, fmt+len, args);
}

inline int fprintf_ptf(FILE *fp, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int ret = vfprintf_ptf(fp, fmt, args);
  va_end(args);
  return ret;
}

inline int printf_ptf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int ret = vfprintf_ptf(stdout, fmt, args);
  va_end(args);
  return ret;
}

inline char* sgetnw(char *buf, int n, char *str) {
  int i = 0;
  while (*str++ == ' ');
  if (!*str--) return 0;
  do buf[i++] = *str++;
  while (buf[i-1] != ' ' && buf[i-1] != '\n' && *str && n--);
  buf[i-1] = 0;
  return str;
}

inline int vsscanf_ptf(char *str, const char *fmt, va_list args) {
  size_t len = getlen_(fmt);
  if (!len) return 0;
  char *fmt_ptf = (char *) malloc(sizeof(char) * (len+1));
  memcpy(fmt_ptf, fmt, len);
  fmt_ptf[len] = 0;

  int ret;
  if (fmt_ptf[len-1] == 'f' ||
      fmt_ptf[len-1] == 'e' ||
      fmt_ptf[len-1] == 'g') {
    Ptfloat *ptf_ptr = (Ptfloat *)va_arg(args, int*);
    char buf[1024];
    sgetnw(buf, 1024, (char *)str);
    *ptf_ptr = (Ptfloat) atof(buf);
    ret = 1;
  } else {
    ret = vsscanf(str, fmt_ptf, args);
  }
  free(fmt_ptf);
  char *str_nxt = strchr(str, ' ');
  return ret + vsscanf_ptf(str_nxt, fmt+len, args);
}

inline int vfscanf_ptf(FILE *fp, const char *fmt, va_list args) {
  char buf[1024];
  char *str = buf;
  int ret = 0;
  while (fgets(str, 1024-(str-buf), fp)) {
    char *lspace = 0;
    if (strlen(buf) == 1023) {
      lspace = strrchr(buf, ' ');
      *lspace++ = 0;
    }
    int ret_int = vsscanf_ptf(buf, fmt, args);
    ret += ret_int;
    if (lspace) strcpy(buf, lspace);
    str = buf + strlen(lspace);
    while (ret_int--) {
      while (*fmt++ != '%');
      while (*fmt++ != ' ');
    }
  }
  return ret;
}

inline int fscanf_ptf(FILE *fp, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int ret = vfscanf_ptf(fp, fmt, args);
  va_end(args);
  return ret;
}

inline int scanf_ptf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int ret = vfscanf_ptf(stdin, fmt, args);
  va_end(args);
  return ret;
}

inline int sscanf_ptf(char *str, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int ret = vsscanf_ptf(str, fmt, args);
  va_end(args);
  return ret;
}

#endif
