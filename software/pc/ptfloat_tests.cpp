#include <iostream>
#include <stdint.h>
#include <float.h>

#include "ptfloat.h"

#define print_vec(name, v, n)\
do {\
  for(int i = 0; i < n; i++)\
    std::cout << name << "[" << i << "] = " << v[i] << std::endl;\
  std::cout << std::endl;\
} while(0)

inline void *memcpy(float *f_ptr, const double *d_ptr, size_t n) {
  n /= sizeof(double);
  while (n--) f_ptr[n] = (float)d_ptr[n];
  return (void *)f_ptr;
}

inline void *memcpy(double *d_ptr, const float *f_ptr, size_t n) {
  n /= sizeof(float);
  while (n--) d_ptr[n] = (double)f_ptr[n];
  return (void *)d_ptr;
}

int32_t rand32(void) {
  int32_t num = rand();
  num |= (int32_t)rand() << 15;
  num |= (int32_t)rand() << 30;
  return num;
}

int64_t rand64(void) {
  int64_t num = (int64_t)rand32() & 0x00000000ffffffff;
  num |= (int64_t)rand32() << 32;
  return num;
}

int32_t randn(int n) {
  int32_t num = rand32();
  num &= (1 << n) - 1;
  return num;
}

float randCommon(void) {
  union {
    float   f;
    int32_t i;
  } conv;
  int32_t exp_mask = (1 << FP_SP_EXP_W) - 1;
  int32_t r = randn(FP_SP_EXP_W*2) / (exp_mask + 1);
  int32_t exp = (r % (exp_mask - 1)) - (FP_SP_BIAS - 1);
  int32_t man = rand32();

  int32_t sign = 0;
  if (man < 0) {
    man = -man;
    sign = 1;
  }

  int32_t width = 19;
  man = (man >> (FP_SP_DATA_W - width)) & ((1 << width) - 1);
  exp += FP_SP_BIAS;

  conv.i = man << (FP_SP_F_W - width);
  conv.i |= exp << FP_SP_F_W;
  conv.i |= sign << (FP_SP_DATA_W - 1);

  return conv.f;
}

float randFloatMAC(void) {
  union {
    float   f;
    int32_t i;
  } conv;
  int32_t exp_mask = (1 << FP_SP_EXP_W) - 1;
  int32_t r = randn(FP_SP_EXP_W*2) / (exp_mask + 1);
  int32_t exp = (r % (exp_mask - 1)) - (FP_SP_BIAS - 1);
  exp = exp/2 - exp/64;
  int32_t man = rand32();

  int32_t sign = 0;
  if (man < 0) {
    man = -man;
    sign = 1;
  }

  int32_t width = FP_SP_F_W;
  man = (man >> (FP_SP_DATA_W - width)) & ((1 << width) - 1);
  exp += FP_SP_BIAS;

  conv.i = man;
  conv.i |= exp << FP_SP_F_W;
  conv.i |= sign << (FP_SP_DATA_W - 1);

  return conv.f;
}

float randFloat(void) {
  union {
    float   f;
    int32_t i;
  } conv;
  int32_t exp_mask = (1 << FP_SP_EXP_W) - 1;
  int32_t r = randn(FP_SP_EXP_W*2) / (exp_mask + 1);
  int32_t exp = (r % (exp_mask - 1)) - (FP_SP_BIAS - 1);
  int32_t man = rand32();

  int32_t sign = 0;
  if (man < 0) {
    man = -man;
    sign = 1;
  }

  int32_t width = FP_SP_F_W;
  man = (man >> (FP_SP_DATA_W - width)) & ((1 << width) - 1);
  exp += FP_SP_BIAS;

  conv.i = man;
  conv.i |= exp << FP_SP_F_W;
  conv.i |= sign << (FP_SP_DATA_W - 1);

  return conv.f;
}

double randDouble(void) {
  union {
    double  d;
    int64_t i;
  } conv;
  int64_t exp_mask = (1 << FP_DP_EXP_W) - 1;
  int64_t r = randn(FP_DP_EXP_W*2) / (exp_mask + 1);
  int64_t exp = (r % (exp_mask - 1)) - (FP_DP_BIAS - 1);
  int64_t man = rand64();

  int64_t sign = 0;
  if (man < 0) {
    man = -man;
    sign = 1;
  }

  int64_t width = FP_DP_F_W;
  man = (man >> (FP_DP_DATA_W - width)) & (((int64_t)1 << width) - 1);
  exp += FP_DP_BIAS;

  conv.i = man;
  conv.i |= exp << FP_DP_F_W;
  conv.i |= sign << (FP_DP_DATA_W - 1);

  return conv.d;
}

double* generate_random_input(unsigned int n) {

  // seed the generator
  //srand(time(0));

  double *vec = (double *) malloc(sizeof(double)*n);

  unsigned int i;
  for (i = 0; i < n; i++) {
    //vec[i] = randDouble();
    //vec[i] = randFloat();
    vec[i] = randFloatMAC();
  }

  return vec;
}

unsigned int read_input(char *inputFile, double **data) {

  FILE *fp = fopen(inputFile, "r");
  if (!fp) {
    fprintf(stderr, "unable to open file '%s'\n", inputFile);
    exit(1);
  }

  int n;
  fread(&n, sizeof(int), 1, fp);

  float *data_f = (float *) malloc(sizeof(float)*n);
  fread(data_f, sizeof(float), n, fp);

  *data = (double *) malloc(sizeof(double)*n);
  memcpy(*data, data_f, sizeof(float)*n);

  free(data_f);

  fclose(fp);

  return (unsigned int)n;
}

void conversion_tests() {
    std::cout << std::endl << "Conversion tests" << std::endl << std::endl;

    Ptfloat u = 37.48;
    std::cout << "u = " << u << std::endl;

    std::cout << "(double)u = " << (double)u << std::endl;
    std::cout << "(float)u = " << (float)u << std::endl;
    std::cout << "(int)u = " << (int)u << std::endl;
    std::cout << std::endl;

    double d = 54.35;
    std::cout << "d = " << d << std::endl;
    std::cout << "(Ptfloat)d = " << (Ptfloat)d << std::endl;
    std::cout << std::endl;

    float f = 78.34;
    std::cout << "f = " << f << std::endl;
    std::cout << "(Ptfloat)f = " << (Ptfloat)f << std::endl;
    std::cout << std::endl;

    int i = 400;
    std::cout << "i = " << i << std::endl;
    std::cout << "(Ptfloat)i = " << (Ptfloat)i << std::endl;
    std::cout << std::endl;

    return;
}

void arithmetic_tests() {
    std::cout << std::endl << "Arithmetic tests" << std::endl << std::endl;

    Ptfloat a = 1.0;
    Ptfloat b = 2.0F;
    Ptfloat c;
    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << std::endl;

    std::cout << "c++ = " << c++ << std::endl;
    std::cout << "++c = " << ++c << std::endl;
    c = a + b;
    std::cout << "c = a + b = " << c << std::endl;
    c = b + 4.0;
    std::cout << "c = b + 4.0 = " << c << std::endl;
    c = b + 3.0F;
    std::cout << "c = b + 3.0F = " << c << std::endl;
    c = b + 7;
    std::cout << "c = b + 7 = " << c << std::endl;
    c = 4.0 + b;
    std::cout << "c = 4.0 + b = " << c << std::endl;
    c = 3.0F + b;
    std::cout << "c = 3.0F + b = " << c << std::endl;
    c = 7 + b;
    std::cout << "c = 7 + b = " << c << std::endl;
    c += b;
    std::cout << "c += b = " << c << std::endl;
    std::cout << std::endl;

    std::cout << "c-- = " << c-- << std::endl;
    std::cout << "--c = " << --c << std::endl;
    c = a - b;
    std::cout << "-b = " << -b << std::endl;
    std::cout << "c = a - b = " << c << std::endl;
    c = b - 4.0;
    std::cout << "c = b - 4.0 = " << c << std::endl;
    c = b - 3.0F;
    std::cout << "c = b - 3.0F = " << c << std::endl;
    c = b - 7;
    std::cout << "c = b - 7 = " << c << std::endl;
    c = 4.0 - b;
    std::cout << "c = 4.0 - b = " << c << std::endl;
    c = 3.0F - b;
    std::cout << "c = 3.0F - b = " << c << std::endl;
    c = 7 - b;
    std::cout << "c = 7 - b = " << c << std::endl;
    c -= b;
    std::cout << "c -= b = " << c << std::endl;
    std::cout << std::endl;

    c = a * b;
    std::cout << "c = a * b = " << c << std::endl;
    c = b * 4.0;
    std::cout << "c = b * 4.0 = " << c << std::endl;
    c = b * 3.0F;
    std::cout << "c = b * 3.0F = " << c << std::endl;
    c = b * 7;
    std::cout << "c = b * 7 = " << c << std::endl;
    c = 4.0 * b;
    std::cout << "c = 4.0 * b = " << c << std::endl;
    c = 3.0F * b;
    std::cout << "c = 3.0F * b = " << c << std::endl;
    c = 7 * b;
    std::cout << "c = 7 * b = " << c << std::endl;
    c *= b;
    std::cout << "c *= b = " << c << std::endl;
    std::cout << std::endl;

    c = a / b;
    std::cout << "c = a / b = " << c << std::endl;
    c = b / 4.0;
    std::cout << "c = b / 4.0 = " << c << std::endl;
    c = b / 3.0F;
    std::cout << "c = b / 3.0F = " << c << std::endl;
    c = b / 7;
    std::cout << "c = b / 7 = " << c << std::endl;
    c = 4.0 / b;
    std::cout << "c = 4.0 / b = " << c << std::endl;
    c = 3.0F / b;
    std::cout << "c = 3.0F / b = " << c << std::endl;
    c = 7 / b;
    std::cout << "c = 7 / b = " << c << std::endl;
    c /= b;
    std::cout << "c /= b = " << c << std::endl;
    std::cout << std::endl;

    return;
}

void comparator_test(Ptfloat a, Ptfloat b) {

    std::cout << "a = " << a << std::endl;
    std::cout << "b = " << b << std::endl;
    std::cout << std::endl;

    if (a > b) {
      std::cout << "a > b" << std::endl;
    } else {
      std::cout << "a !> b" << std::endl;
    }
    if (a >= b) {
      std::cout << "a >= b" << std::endl;
    } else {
      std::cout << "a !>= b" << std::endl;
    }
    if (a < b) {
      std::cout << "a < b" << std::endl;
    } else {
      std::cout << "a !< b" << std::endl;
    }
    if (a <= b) {
      std::cout << "a <= b" << std::endl;
    } else {
      std::cout << "a !<= b" << std::endl;
    }
    if (a == b) {
      std::cout << "a == b" << std::endl;
    } else {
      std::cout << "a !== b" << std::endl;
    }
    if (a != b) {
      std::cout << "a != b" << std::endl;
    } else {
      std::cout << "a !!= b" << std::endl;
    }
    std::cout << std::endl;

    return;
}

void comparator_tests() {
    std::cout << std::endl << "Comparator tests" << std::endl << std::endl;

    Ptfloat a = 1;
    Ptfloat b = 2;
    comparator_test(a, b);

    a = 2;
    b = 1;
    comparator_test(a, b);

    a = 2;
    b = 2;
    comparator_test(a, b);

    return;
}

void memory_tests() {
    std::cout << std::endl << "Memory tests" << std::endl << std::endl;

    int n = 10;
    Ptfloat *vec_ptf;
    std::cout << std::endl << "Calloc test" << std::endl << std::endl;
    vec_ptf = (Ptfloat *)calloc_ptf(n, sizeof(Ptfloat));
    print_vec("vec_ptf", vec_ptf, n);

    double *vec_d;
    vec_d = (double *)calloc(n, sizeof(double));
    print_vec("vec_d", vec_d, n);

    float *vec_f;
    vec_f = (float *)calloc(n, sizeof(float));
    print_vec("vec_f", vec_f, n);

    int *vec_i;
    vec_i = (int *)calloc(n, sizeof(int));
    print_vec("vec_i", vec_i, n);

    std::cout << std::endl << "Memset test" << std::endl << std::endl;
    memset(vec_ptf, 3, n*sizeof(Ptfloat));
    print_vec("vec_ptf", vec_ptf, n);

    memset(vec_d, 5, n*sizeof(double));
    print_vec("vec_d", vec_d, n);

    memset(vec_f, 7, n*sizeof(float));
    print_vec("vec_f", vec_f, n);

    memset(vec_i,11, n*sizeof(int));
    print_vec("vec_i", vec_i, n);

    std::cout << std::endl << "Memcpy test" << std::endl << std::endl;
    memcpy(vec_ptf, vec_d, n*sizeof(double));
    print_vec("vec_ptf", vec_ptf, n);
    memcpy(vec_ptf, vec_f, n*sizeof(float));
    print_vec("vec_ptf", vec_ptf, n);
    memcpy(vec_ptf, vec_i, n*sizeof(int));
    print_vec("vec_ptf", vec_ptf, n);

    free(vec_ptf);

    return;
}

void mac_test(double *a_d, double *b_d, double *acc_d,
              float *a_f, float *b_f, float *acc_f,
              Ptfloat *a_ptf, Ptfloat *b_ptf, Ptfloat *acc_ptf,
              unsigned int n) {
  for (unsigned int i = 0; i < n; i++) {
    *acc_d += a_d[i] * b_d[i];
    *acc_f += a_f[i] * b_f[i];
    *acc_ptf += a_ptf[i] * b_ptf[i];
  }
  return;
}

void mac_tests(double *data, unsigned int n, double *weights, unsigned int m, FILE *fp_d, FILE *fp_f, FILE *fp_ptf) {
  double *a_d = (double *)calloc(m, sizeof(double));
  double *b_d = weights;
  float *a_f = (float *)calloc(m, sizeof(float));
  float *b_f = (float *)calloc(m, sizeof(float));
  Ptfloat *a_ptf = (Ptfloat *)calloc_ptf(m, sizeof(Ptfloat));
  Ptfloat *b_ptf = (Ptfloat *)calloc_ptf(m, sizeof(Ptfloat));

  memcpy(b_f, b_d, m*sizeof(double));
  memcpy(b_ptf, b_d, m*sizeof(double));

  double max_acc = 0.0;
  double min_acc = DBL_MAX;

  std::cout << std::endl << "MAC tests" << std::endl << std::endl;
  for (unsigned int i = 0; i <= n-m; i++) {
    memcpy(a_d, &data[i], m*sizeof(double));
    memcpy(a_f, a_d, m*sizeof(double));
    memcpy(a_ptf, a_d, m*sizeof(double));

    double c_d = 0.0;
    float c_f = 0.0;
    Ptfloat c_ptf = 0.0;
    mac_test(a_d, b_d, &c_d,
             a_f, b_f, &c_f,
             a_ptf, b_ptf, &c_ptf,
             m);

    if (abs(c_d) > max_acc) max_acc = abs(c_d);
    if (abs(c_d) < min_acc) min_acc = abs(c_d);

    fprintf(fp_d, "%.16E\n", c_d);
    fprintf(fp_f, "%.16E\n", c_f);
    fprintf(fp_ptf, "%.16e\n", c_ptf);
  }

  printf("max_acc = %.16E\n", max_acc);
  printf("min_acc = %.16E\n", min_acc);

  free(a_d);
  free(a_f);
  free(b_f);
  free(a_ptf);
  free(b_ptf);

  return;
}

void run_tests(double *data, unsigned int n, double *weights, unsigned int m, FILE *fp_d, FILE *fp_f, FILE *fp_ptf) {

    conversion_tests();

    arithmetic_tests();

    comparator_tests();

    memory_tests();

    mac_tests(data, n, weights, m, fp_d, fp_f, fp_ptf);

    return;
}

void usage(char *name, const char *message) {
  printf("usage: %s\n", message);
  printf("       %s -i <input file> -n <number of input data> -w <weight file> -m <number of weights> -d <double file> -f <float file> -p <PT-Float file>\n", name);
  exit(1);
}

int main(int argc, char **argv) {
    char *inputFile = 0;
    char *weightFile = 0;
    char *doubleFile = 0;
    char *floatFile = 0;
    char *ptfloatFile = 0;

    unsigned int n = 10240;
    unsigned int m = 1024;

    // Process input arguments
    int i;

    if (argc < 7)
      usage(argv[0], "not enough program arguments\n");

    for (i = 1; i < argc; i++) {
      if (argv[i][0] == '-' && !argv[i][2]) {
        if (argv[i][1] == 'i') {
          inputFile = argv[++i];
        } else if (argv[i][1] == 'n') {
          n = atoi(argv[++i]);
        } else if (argv[i][1] == 'w') {
          weightFile = argv[++i];
        } else if (argv[i][1] == 'm') {
          m = atoi(argv[++i]);
        } else if (argv[i][1] == 'd') {
          doubleFile = argv[++i];
        } else if (argv[i][1] == 'f') {
          floatFile = argv[++i];
        } else if (argv[i][1] == 'p') {
          ptfloatFile = argv[++i];
        } else usage(argv[0], "unexpected argument\n");
      } else usage(argv[0], "unexpected argument\n");
    }

    if (!doubleFile || !floatFile || !ptfloatFile)
      usage(argv[0], "not enough program arguments\n");

    // Load input data
    double *data;
    if (inputFile) n = read_input(inputFile, &data);
    else data = generate_random_input(n);

    // Load weights
    double *weights;
    if (weightFile) m = read_input(weightFile, &weights);
    else weights = generate_random_input(m);

    FILE *fp_d = fopen(doubleFile, "w");
    if (!fp_d) {
      fprintf(stderr, "unable to open file '%s'\n", doubleFile);
      exit(1);
    }

    FILE *fp_f = fopen(floatFile, "w");
    if (!fp_f) {
      fprintf(stderr, "unable to open file '%s'\n", floatFile);
      fclose(fp_d);
      exit(1);
    }

    FILE *fp_ptf = fopen(ptfloatFile, "w");
    if (!fp_ptf) {
      fprintf(stderr, "unable to open file '%s'\n", ptfloatFile);
      fclose(fp_d);
      fclose(fp_f);
      exit(1);
    }

    // Run tests
    run_tests(data, n, weights, m, fp_d, fp_f, fp_ptf);

    double max = 0.0;
    double min = DBL_MAX;
    for (unsigned int j = 0; j < n; j++) {
      if (abs(data[j]) > max) max = abs(data[j]);
      if (abs(data[j]) < min) min = abs(data[j]);
    }

    for (unsigned int j = 0; j < m; j++) {
      if (abs(weights[j]) > max) max = abs(weights[j]);
      if (abs(weights[j]) < min) min = abs(weights[j]);
    }
    printf("max = %.16E\n", max);
    printf("min = %.16E\n", min);

    free(data);
    free(weights);

    fclose(fp_d);
    fclose(fp_f);
    fclose(fp_ptf);

    return 0;
}
