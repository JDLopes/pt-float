#!/usr/bin/python3
#
# i e e e 7 5 4 . p y
#

from ctypes import Union, c_double, c_int64

#
# IEEE-754
#

# Double precision
FP_DP_DATA_W = 64
FP_DP_EXP_W  = 11
FP_DP_BIAS   = 1023
FP_DP_F_W    = 52
FP_DP_MAN_W  = FP_DP_F_W + 1

# Single precision
FP_SP_DATA_W = 32
FP_SP_EXP_W  = 8
FP_SP_BIAS   = 127
FP_SP_F_W    = 23
FP_SP_MAN_W  = (FP_SP_F_W + 1)

class union_d(Union):
    _fields_ = ("d", c_double), ("i", c_int64)

def msb(width):
    return 1 << (width-1)

def mask(width):
    return (1 << width) - 1

def doubleIsSub(num_d):
    conv = union_d()
    conv.d = num_d
    man = conv.i & mask(FP_DP_F_W)
    exp = (conv.i >> FP_DP_F_W) & mask(FP_DP_EXP_W)
    if ((exp == 0) and man) : return 1
    else : return 0

def doubleIsNan(num_d):
    conv = union_d()
    conv.d = num_d
    man = conv.i & mask(FP_DP_F_W)
    exp = (conv.i >> FP_DP_F_W) & mask(FP_DP_EXP_W)
    if ((exp == mask(FP_DP_EXP_W)) and man) : return 1
    else : return 0

def doubleIsInf(num_d):
    conv = union_d()
    conv.d = num_d
    man = conv.i & mask(FP_DP_F_W)
    exp = (conv.i >> FP_DP_F_W) & mask(FP_DP_EXP_W)
    if ((exp == mask(FP_DP_EXP_W)) and man == 0) : return 1
    else : return 0

def getDoubleSign(num_d):
    conv = union_d()
    conv.d = num_d
    return (conv.i >> (FP_DP_DATA_W-1)) & 1

def getDoubleExp(num_d):
    conv = union_d()
    conv.d = num_d
    return ((conv.i >> FP_DP_F_W) & mask(FP_DP_EXP_W)) - FP_DP_BIAS

def getDoubleMan(num_d):
    conv = union_d()
    conv.d = num_d
    exp = (conv.i >> FP_DP_F_W) & mask(FP_DP_EXP_W)
    man = conv.i & mask(FP_DP_F_W)
    if (not doubleIsSub(num_d) and not doubleIsNan(num_d) and exp and exp != mask(FP_DP_EXP_W)) : man |= msb(FP_DP_MAN_W)
    return man
