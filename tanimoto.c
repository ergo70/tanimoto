#include "postgres.h"
#include "fmgr.h"
#include <utils/varbit.h>

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

static const bits8 popcount_counts_byte[] =
{
    0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
    3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8
};

static inline float4 fast_tanimoto (const bits8 *fp1, const bits8 *fp2, unsigned int size)
{
    // Fast tanimoto code with 8 bit LUT by Andrew Dalke as published in http://www.dalkescientific.com/writings/diary/archive/2008/06/27/computing_tanimoto_scores.html.
    // Used with permission

    bits8 tmp;
    unsigned int and_count=0, or_count=0;

    while (size--)
    {
        tmp = (*fp2 | *fp1);
        or_count += popcount_counts_byte[tmp];

        tmp = (*fp2 & *fp1);
        and_count += popcount_counts_byte[tmp];

        fp2++;
        fp1++;
    }

    return (float4) and_count / (or_count * 1.0f);
}

PG_FUNCTION_INFO_V1(tanimoto);

Datum
tanimoto (PG_FUNCTION_ARGS)
{
    VarBit *arg_fp1 = PG_GETARG_VARBIT_P (0);
    VarBit *arg_fp2 = PG_GETARG_VARBIT_P (1);
    unsigned int size = VARBITLEN(arg_fp1);
    float4 retval = 0.0;

    if(size != (VARBITLEN(arg_fp2)))
    {
        elog (ERROR, "Fingerprints must have equal sizes.");
    }

    if(0 != (size % BITS_PER_BYTE)) {
        elog (ERROR, "Fingerprint sizes must be evenly divisible by 8.");
    }

    retval = (float4) fast_tanimoto(VARBITS(arg_fp1), VARBITS(arg_fp2), (size / BITS_PER_BYTE));

    PG_RETURN_FLOAT4 (retval);
}
