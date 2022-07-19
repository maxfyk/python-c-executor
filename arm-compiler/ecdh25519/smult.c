#include "group.h"
#include "smult.h"

#define SMUL_BASELINE
#define BMUL_BASELINE

void cond_copy(group_ge *dest, group_ge *src0, group_ge *src1, uint32_t cond)
{
  cond = -cond;

  uint32_t *d = (uint32_t*)dest;
  uint32_t *s0 = (uint32_t*)src0;
  uint32_t *s1 = (uint32_t*)src1;

  for (unsigned int i = 0; i < sizeof(group_ge) / sizeof(uint32_t); i++)
    d[i] = (s0[i] & (~cond)) | (s1[i] & cond);
}


#ifdef SMUL_BASELINE
int crypto_scalarmult(unsigned char *ss, const unsigned char *sk, const unsigned char *pk)
{
  group_ge p, k;
  int i,j;

  if(group_ge_unpack(&p, pk)) return -1;

  k = group_ge_neutral;

  for(i=31;i>=0;i--)
  {
    for(j=7;j>=0;j--)
    {
      group_ge_double(&k, &k);
      if((sk[i] >> j) & 1) {
        group_ge_add(&k, &k, &p);
      }
    }
  }

  group_ge_pack(ss, &k);

  return 0;
}
#endif

#ifdef SMUL_CONST_TIME
int crypto_scalarmult(unsigned char *ss, const unsigned char *sk, const unsigned char *pk)
{
  group_ge p, k, tmp;
  int i,j;

  if(group_ge_unpack(&p, pk)) return -1;

  k = group_ge_neutral;

  for(i=31;i>=0;i--)
  {
    for(j=7;j>=0;j--)
    {
      group_ge_double(&k, &k);

      // always add and coditionally copy the result of the addition
      group_ge_add(&tmp, &k, &p);
      cond_copy(&k, &k, &tmp, (sk[i] >> j) & 1);
    }
  }

  group_ge_pack(ss, &k);

  return 0;
}
#endif

#ifdef SMUL_FWIN
int crypto_scalarmult(unsigned char *ss, const unsigned char *sk, const unsigned char *pk)
{
  group_ge p;

  if(group_ge_unpack(&p, pk)) return -1;

#define WWIDTH 5
#define SPRE (1 << WWIDTH)

  group_ge table[SPRE];

  // precompute table with 0*P, 1*P, 2*P, 3*P, 4*P, .... (2^WWIDTH - 1) * P
  table[0] = group_ge_neutral;

  for (int i = 1; i < SPRE; i++)
    group_ge_add(&table[i], &table[i-1], &p);


  unsigned char bk[128] = {0};

  int bp = 0;
  int wp = 0;

  // get scalar factor in groups of WWIDTH bits
  for (int i = 0; i < 32; i++)
    for(int j = 0; j < 8; j++)
    {
      bk[wp] = bk[wp] | (((sk[i] >> j) & 1) << bp);
      bp++;

      if (bp == WWIDTH)
      {
        wp++;
        bp = 0;
      }
    }


  // start with the highes WWIDTH-bit word
  group_ge k = table[bk[wp]];


  // ieterate through the scalar in WWIDTH-bit words
  for(int i = wp-1; i >= 0; i--)
  {
    for(int j = WWIDTH-1; j >= 0; j--)
      group_ge_double(&k, &k);

    group_ge_add(&k, &k, &table[bk[i]]);
  }


  group_ge_pack(ss, &k);

  return 0;
}
#endif

#ifdef SMUL_FWIN_CONST_TIME

unsigned char int_eq(int a, int b)
{
  unsigned long long t = a ^ b;
  t = (-t) >> 63;
  return 1-t;
}

void load_const_time(group_ge *ret, group_ge *table, int index, int size)
{
  // always touch all elements in the table; copy the requested index
  for (int i = 0; i < size; i++)
    cond_copy(ret, ret, &table[i], int_eq(i, index));
}

int crypto_scalarmult(unsigned char *ss, const unsigned char *sk, const unsigned char *pk)
{
  group_ge p;

  if(group_ge_unpack(&p, pk)) return -1;

#define WWIDTH 5
#define SPRE (1 << WWIDTH)

  group_ge table[SPRE];

  table[0] = group_ge_neutral;

  for (int i = 1; i < SPRE; i++)
    group_ge_add(&table[i], &table[i-1], &p);


  unsigned char bk[128] = {0};

  int bp = 0;
  int wp = 0;

  for (int i = 0; i < 32; i++)
    for(int j = 0; j < 8; j++)
    {
      bk[wp] = bk[wp] | (((sk[i] >> j) & 1) << bp);
      bp++;

      if (bp == WWIDTH)
      {
        wp++;
        bp = 0;
      }
    }


  group_ge k = table[bk[wp]];
  group_ge tmp;

  for(int i = wp-1; i >= 0; i--)
  {
    for(int j = WWIDTH-1; j >= 0; j--)
      group_ge_double(&k, &k);

    // load constant time
    load_const_time(&tmp, table, bk[i], SPRE);
    group_ge_add(&k, &k, &tmp);
  }


  group_ge_pack(ss, &k);

  return 0;
}
#endif

#ifdef SMUL_SWIN
int crypto_scalarmult(unsigned char *ss, const unsigned char *sk, const unsigned char *pk)
{
  group_ge p;

  if(group_ge_unpack(&p, pk)) return -1;

#define WWIDTH 5
#define SPRE (1 << WWIDTH)

  group_ge table[SPRE];

  table[0] = p;

  // we alwats have the highes bit set; for WWIDTH=5, we need the following precomputed points:
  // 100000_b * P
  // 100001_b * P
  // 100010_b * P
  // 100011_b * P
  // 100100_b * P
  // ...
  // 111111_b * P

  // push the point up (compute 100000_b * P)
  for (int i = 0; i < WWIDTH; i++)
    group_ge_double(&table[0], &table[0]);

  // fill in the gaps
  for (int i = 1; i < SPRE; i++)
    group_ge_add(&table[i], &table[i-1], &p);


  // lets get easy access to each single bit in the scalar
  unsigned char bk[256] = {0};

  int bp = 0;

  for (int i = 0; i < 32; i++)
    for(int j = 0; j < 8; j++)
      bk[bp++] = (sk[i] >> j) & 1;


  group_ge k = group_ge_neutral;

  int i = 255;

  while (i >= 0)
  {
    // as long as there are leading zero-bits in the scalar, just double
    while ((bk[i] == 0) && (i >= 0))
    {
      group_ge_double(&k, &k);
      i--;
    }

    // exit if not enough bits are left for a complete window size
    if (i < WWIDTH)
      break;

    uint32_t idx = 0;

    // get the correct index (i.e., the (WWIDTH+1)-bits long index)
    for (int j = 0; j <= WWIDTH; j++)
    {
      idx = (idx << 1) | bk[i];
      i--;
    }

    // double up to make 'space' for window
    for (int j = 0; j <= WWIDTH; j++)
      group_ge_double(&k, &k);
 
    // add in window; we need to subtract SPRE
    // since the high bit of the factors in the table is always '1'
    group_ge_add(&k, &k, &table[idx - SPRE]);
  }

  // handle the remaining bits that did not fit into the last window
  for (; i >= 0; i--)
  {
    group_ge_double(&k, &k);

    if (bk[i] == 1)
      group_ge_add(&k, &k, &p);
  }

  group_ge_pack(ss, &k);

  return 0;
}
#endif



#ifdef BMUL_BASELINE
int crypto_scalarmult_base(unsigned char *pk, const unsigned char *sk)
{
  group_ge p, k;
  int i,j;
  
  p = group_ge_base;

  k = group_ge_neutral;

  for(i=31;i>=0;i--)
  {
    for(j=7;j>=0;j--)
    {
      group_ge_double(&k, &k);
      if((sk[i] >> j) & 1) {
        group_ge_add(&k, &k, &p);
      }
    }
  }

  group_ge_pack(pk, &k);

  return 0;
}
#endif

#ifdef BMUL_BASELINE_CONST_TIME
int crypto_scalarmult_base(unsigned char *pk, const unsigned char *sk)
{
  group_ge p, k, tmp;
  int i,j;
  
  p = group_ge_base;

  k = group_ge_neutral;

  for(i=31;i>=0;i--)
  {
    for(j=7;j>=0;j--)
    {
      group_ge_double(&k, &k);

      // always add and coditionally copy the result of the addition
      group_ge_add(&tmp, &k, &p);
      cond_copy(&k, &k, &tmp, (sk[i] >> j) & 1);
    }
  }

  group_ge_pack(pk, &k);

  return 0;
}
#endif


#if defined(BMUL_PRE) || defined(BMUL_PRE_CONST_TIME)

#define PRE 240  // Must be multiple of 8 and smaller than or equal to 240.
#define TOT 256

group_ge table[PRE];
#endif

#ifdef BMUL_FWIN
#define PRE 128

group_ge table[PRE];
#endif


void crypto_scalarmult_base_pre()
{
#if defined(BMUL_PRE) || defined(BMUL_PRE_CONST_TIME)
  table[0] = group_ge_base;

  for (int i = 1; i < PRE; i++)
    group_ge_double(&table[i], &table[i-1]);
#endif

#ifdef BMUL_FWIN
  table[0] = group_ge_neutral;

  for (int i = 1; i < PRE; i++)
    group_ge_add(&table[i], &table[i-1], &group_ge_base);
#endif
}

#ifdef BMUL_PRE
int crypto_scalarmult_base(unsigned char *pk, const unsigned char *sk)
{
  group_ge k = group_ge_neutral;

  // Compute the top PRE bits.
  for(int i = 31; i >= (TOT-PRE)>>3; i--)
    for(int j = 7; j >= 0; j--)
      if ((sk[i] >> j) & 1)
        group_ge_add(&k, &k, &table[(i*8 + j) - (TOT - PRE)]);

  // Shift for the LSBs.
  for (int i = 0; i < TOT-PRE; i++)
    group_ge_double(&k, &k);

  // Compute the bottom TOT-PRE bits.
  for(int i = ((TOT-PRE) >> 3) - 1; i >= 0; i--)
    for(int j = 7; j >= 0; j--)
      if ((sk[i] >> j) & 1)
        group_ge_add(&k, &k, &table[i*8 + j]);


  group_ge_pack(pk, &k);

  return 0;
}
#endif

#ifdef BMUL_PRE_CONST_TIME
int crypto_scalarmult_base(unsigned char *pk, const unsigned char *sk)
{
  group_ge k = group_ge_neutral;
  group_ge tmp;

  // Compute the top PRE bits.
  for(int i = 31; i >= (TOT-PRE)>>3; i--)
    for(int j = 7; j >= 0; j--)
    {
      // always add and coditionally copy the result of the addition
      group_ge_add(&tmp, &k, &table[(i*8 + j) - (TOT - PRE)]);
      cond_copy(&k, &k, &tmp, (sk[i] >> j) & 1);
    }

  // Shift for the LSBs.
  for (int i = 0; i < TOT-PRE; i++)
    group_ge_double(&k, &k);

  // Compute the bottom TOT-PRE bits.
  for(int i = ((TOT-PRE) >> 3) - 1; i >= 0; i--)
    for(int j = 7; j >= 0; j--)
    {
      // always add and coditionally copy the result of the addition
      group_ge_add(&tmp, &k, &table[i*8 + j]);
      cond_copy(&k, &k, &tmp, (sk[i] >> j) & 1);
    }


  group_ge_pack(pk, &k);

  return 0;
}
#endif

#ifdef BMUL_FWIN
int crypto_scalarmult_base(unsigned char *pk, const unsigned char *sk)
{
  unsigned char bk[37] = {0};

  int bp = 0;
  int wp = 0;

  // extract words of seven bits from sk
  for (int i = 0; i < 32; i++)
    for(int j = 0; j < 8; j++)
    {
      bk[wp] = bk[wp] | (((sk[i] >> j) & 1) << bp);
      bp++;

      if (bp == 7)
      {
        wp++;
        bp = 0;
      }
    }

  // start with the first 7-bit word
  group_ge k = table[bk[36]];

  // process the remaining bits in the scalar
  for(int i = 35; i >= 0; i--)
  {
    for(int j = 6; j >= 0; j--)
      group_ge_double(&k, &k);

    group_ge_add(&k, &k, &table[bk[i]]);
  }


  group_ge_pack(pk, &k);

  return 0;
}
#endif


