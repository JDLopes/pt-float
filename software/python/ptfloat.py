#!/usr/bin/python3
#
# p t f l o a t . p y
#

#
# PT-Float
#

DATA_W  = 32
EW_W    = 4

PTFLOAT_W = 32

EW_MAX = 0
F_MAX_W = 0
F_MIN_W = 0
EXP_MAX_W = 0
MAN_MAX_W = 0
MAN_MIN_W = 0
EXP_MAX = 0
EXP_MIN = 0
MAN_MAX = 0
MAN_MIN = 0
RES_MAX_W = 0

PTFLOAT_MAX   = 0
PTFLOAT_MIN   = 0
PTFLOAT_ZERO  = 0
PTFLOAT_HALF  = 0
PTFLOAT_ONE   = 0
PTFLOAT_TWO   = 0
PTFLOAT_M_ONE = 0

def F_W(ew):
    return DATA_W - ew - EW_W

def EXP_W(ew):
    if (ew): return ew + 1
    else: return 0

def MAN_W(ew):
    return F_W(ew) + 1

def notation(data_w, ew_w, ptfloat_w=PTFLOAT_W):
    global DATA_W, EW_W
    global EW_MAX, F_MAX_W, F_MIN_W
    global EXP_MAX_W, MAN_MAX_W, MAN_MIN_W
    global EXP_MAX, EXP_MIN, MAN_MAX, MAN_MIN
    global RES_MAX_W
    global PTFLOAT_MAX, PTFLOAT_MIN, PTFLOAT_ZERO, PTFLOAT_HALF, PTFLOAT_ONE, PTFLOAT_TWO, PTFLOAT_M_ONE
    
    DATA_W  = data_w
    EW_W    = ew_w
    
    EW_MAX  = (1 << EW_W) - 1
    F_MAX_W = F_W(0)
    F_MIN_W = F_W(EW_MAX)
    
    EXP_MAX_W = EW_MAX + 1
    MAN_MAX_W = F_MAX_W + 1
    MAN_MIN_W = F_MIN_W + 1
    
    EXP_MAX = (1 << EW_MAX) - 1
    EXP_MIN = -EXP_MAX+1
    MAN_MAX = (1 << F_MIN_W) - 1
    MAN_MIN = 1 << F_MIN_W
    
    RES_MAX_W = MAN_MAX_W + 3
    
    PTFLOAT_MAX   = (1 << DATA_W) - 1
    PTFLOAT_MIN   = (EXP_MAX << (F_MIN_W + EW_W)) | EW_MAX
    PTFLOAT_ZERO  = EW_MAX
    PTFLOAT_HALF  = 0x8 << (DATA_W - 4)
    PTFLOAT_ONE   = (0xc << (DATA_W - 4)) | 1
    PTFLOAT_TWO   = (0xa << (DATA_W - 4)) | 2
    PTFLOAT_M_ONE = 0
    
    return

def msb(width):
    return 1 << (width-1)

def mask(width):
    return (1 << width) - 1

def cleadings(num, width):
    leadings = 0
    if (num <= 0) : num = num ^ -1
    for i in range(width) :
        if ((num << i) & msb(width-1)) : break
        leadings+=1
    return leadings

def cew(exp):
    ew = 0
    if (exp) :
        if (exp < 0) : exp-=1
        leadings = cleadings(exp, PTFLOAT_W)
        ew = PTFLOAT_W - leadings - 1
    return ew

def cmw(exp):
    return F_W(cew(exp))

def unpack(num_ptf):
  upk = {}
  ew = num_ptf & EW_MAX
  e = 0
  if (ew) :
      e = (num_ptf >> (DATA_W - ew)) & mask(ew)
      if (msb(ew) & e) : e ^= -1 ^ mask(ew)
  f = (num_ptf >> EW_W) & mask(F_W(ew))
  if (msb(F_W(ew)) & f) : f ^= -1 ^ mask(F_W(ew))

  upk['exp'] = e ^ (-1 ^ mask(ew))
  if (upk['exp'] < 0) : upk['exp'] += 1
  if (not ew) :
    upk['exp'] = 0
  elif (ew == EW_MAX and not e) :
    upk['exp'] += 1

  upk['man'] = f
  if (not(ew == EW_MAX and not e)) :
    upk['man'] ^= -1 ^ mask(F_W(ew))

  upk['ew'] = ew

  return upk

def cman(upk):
    return upk['man']*2**(-(DATA_W - EW_W - upk['ew']))

def double(num_ptf):
    upk = unpack(num_ptf)
    return cman(upk)*2**upk['exp']

def isSub(num_ptf):
    ew = num_ptf & EW_MAX
    e = (num_ptf >> (DATA_W - ew)) & mask(ew)
    if (e == 0 and ew == EW_MAX) : return 1
    else : return 0

def PACK(exp, man, ew):
    ew_ = ew & EW_MAX
    ret = ew_
    ret |= (man >> (PTFLOAT_W - DATA_W + ew_ + EW_W)) << EW_W
    if (ew_): ret |= (exp & ((1 << (EXP_W(ew_) - 1)) - 1)) << (DATA_W - ew_)
    return ret

def h(num, width=DATA_W):
    fmt = '0x%0'+str(width//4)+'x'
    return fmt % num

def b(num, width=DATA_W):
    ret = bin(num)
    if (len(ret) < width+2) : ret = ret[:2]+'0'*(width+2-len(ret))+ret[2:]
    return ret

notation(32, 4)
