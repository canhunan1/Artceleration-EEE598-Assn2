#ifndef NEON_INTRINSICS_H
#define NEON_INTRINSICS_H

#include <android/bitmap.h>

extern void fir_filter_neon_intrinsics(short *output, const short* input, const short* kernel, int width, int kernelSize);
extern void neonNeonEdgeLinearSum(AndroidBitmapInfo* infoOri, uint32_t * pixelsOri,AndroidBitmapInfo* infoProcessed, uint32_t * pixelsProcessed,float f1,float f2);

#endif /* NEON_INTRINSICS_H */