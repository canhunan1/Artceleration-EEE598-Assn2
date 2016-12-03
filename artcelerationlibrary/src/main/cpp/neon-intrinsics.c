#include "neon-intrinsics.h"
#include <arm_neon.h>
#include <malloc.h>
#include <string.h>
#include <android/bitmap.h>
#include <android/log.h>

#define  LOG_TAG    "DEBUG"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
/* this source file should only be compiled by Android.mk /CMake when targeting
 * the armeabi-v7a ABI, and should be built in NEON mode
 */

void fir_filter_neon_intrinsics(short *output, const short* input, const short* kernel, int width, int kernelSize)
{
#if 1
    int nn, offset = -kernelSize/2;

    for (nn = 0; nn < width; nn++)
    {
        int mm, sum = 0;
        int32x4_t sum_vec = vdupq_n_s32(0);
        for(mm = 0; mm < kernelSize/4; mm++)
        {
            int16x4_t  kernel_vec = vld1_s16(kernel + mm*4);
            int16x4_t  input_vec = vld1_s16(input + (nn+offset+mm*4));
            sum_vec = vmlal_s16(sum_vec, kernel_vec, input_vec);
        }

        sum += vgetq_lane_s32(sum_vec, 0);
        sum += vgetq_lane_s32(sum_vec, 1);
        sum += vgetq_lane_s32(sum_vec, 2);
        sum += vgetq_lane_s32(sum_vec, 3);

        if(kernelSize & 3)
        {
            for(mm = kernelSize - (kernelSize & 3); mm < kernelSize; mm++)
                sum += kernel[mm] * input[nn+offset+mm];
        }

        output[nn] = (short)((sum + 0x8000) >> 16);
    }
#else /* for comparison purposes only */
    int nn, offset = -kernelSize/2;
    for (nn = 0; nn < width; nn++) {
        int sum = 0;
        int mm;
        for (mm = 0; mm < kernelSize; mm++) {
            sum += kernel[mm]*input[nn+offset+mm];
        }
        output[n] = (short)((sum + 0x8000) >> 16);
    }
#endif
}


/*
 * This method defines the linear add mathematical calculation which performed in neon.
 * @param   infoOri input is used to find the information of original bitmap image.
 * @param   pixelsOri is the bitmap header pointer which points to the address of first byte of original bitmap image.
 * @param   infoProcessed input is used to find the information of bitmap image after SobelEdge and GaussianBlur transform.
 * @param   pixelsProcessed is the bitmap  pointer which points to the address of first byte of bitmap imageafter SobelEdge and GaussianBlur transform.
 * */
void neonNeonEdgeLinearSum(AndroidBitmapInfo* infoOri, uint32_t * pixelsOri,AndroidBitmapInfo* infoProcessed, uint32_t * pixelsProcessed,float f1,float f2){
    uint32_t  width = infoOri->width;
    uint32_t  height = infoOri->height;
    uint32x4_t alphaFactor = vdupq_n_u32(0xFF000000);
    uint32x4_t redFactor = vdupq_n_u32(0x00FF0000);
    uint32x4_t greenFactor = vdupq_n_u32(0x0000FF00);
    uint32x4_t blueFactor = vdupq_n_u32(0x000000FF);
    uint32x4_t redShift = vdupq_n_u32(16);
    uint32x4_t greenShift = vdupq_n_u32(8);

    float32x4_t f1V =  vmovq_n_f32((float32_t)f1);
    float32x4_t f2V =  vmovq_n_f32((float32_t)f2);
    uint32_t* linePro;
    uint32_t* lineOri;


    for(int i = 0; i<height; i++) {
        linePro = (uint32_t*) pixelsProcessed;
        lineOri = (uint32_t*) pixelsOri;
        for(int j = 0; j<width/4;j += 4) {

            uint32x4_t ori = vld1q_u32(&lineOri[j]);
            uint32x4_t redOri = vqshlq_u32(vandq_u32(ori, redFactor),redShift);
            uint32x4_t greenOri = vqshlq_u32(vandq_u32(ori, greenFactor),greenShift);
            uint32x4_t blueOri = vandq_u32(ori, blueFactor);

            uint32x4_t pre = vld1q_u32(&linePro[j]);
            uint32x4_t redPre = vqshlq_u32(vandq_u32(pre, redFactor),redShift);
            uint32x4_t greenPre = vqshlq_u32(vandq_u32(pre, greenFactor),greenShift);
            uint32x4_t bluePre = vandq_u32(pre, blueFactor);

            uint32x4_t red = vqshlq_u32(vcvtq_u32_f32(vmlaq_f32(vmulq_f32(redPre , f1V), redOri, f2V)),redShift);
            uint32x4_t green = vqshlq_u32(vcvtq_u32_f32(vmlaq_f32(vmulq_f32(greenPre , f1V), greenOri, f2V)), greenShift);
            uint32x4_t blue = vcvtq_u32_f32(vmlaq_f32(vmulq_f32(bluePre , f1V), blueOri, f2V));
            uint32x4_t pixel = vorrq_u32(vorrq_u32(alphaFactor,red),vorrq_u32(green,blue));
            vst1q_u32(&linePro[j],pixel);
            //uint8x8_t a =  vdup_n_u8(0);
        }
        pixelsProcessed = (uint32_t*)((char*)pixelsProcessed + infoProcessed->stride);
        pixelsOri = (uint32_t*)((char*)pixelsOri + infoOri->stride);
    }

}

