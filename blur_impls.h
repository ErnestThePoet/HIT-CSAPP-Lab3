#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <algorithm>

#include <immintrin.h>

#include "macros.h"

#ifdef ARM
#include <arm_neon.h>
#endif

#include <omp.h>

#include "bitmap.h"


#define SINGLE_UNROLL_LOCALS(N) \
constexpr int32_t kByteOffset##N = bytes_per_px * N; \
int32_t current_total_offset_##N = current_row_offset + current_pixel_offset + kByteOffset##N; \
int32_t l_index_##N = current_pixel_offset - bytes_per_px + kByteOffset##N; \
int32_t r_index_##N = current_row_offset + current_pixel_offset + bytes_per_px + kByteOffset##N; \
int32_t lower_index_##N = lower_row_offset + current_pixel_offset + kByteOffset##N


#define SINGLE_UNROLL(N) \
pixel_data[current_total_offset_##N] = \
((uint32_t)upper_row_pixels[current_pixel_offset+kByteOffset##N])\
+ (current_row_pixels[l_index_##N])\
+ (pixel_data[r_index_##N])\
+ (pixel_data[lower_index_##N]) >> 2;\
\
pixel_data[current_total_offset_##N + 1] = \
((uint32_t)upper_row_pixels[current_pixel_offset+kByteOffset##N + 1])\
+ (current_row_pixels[l_index_##N + 1])\
+ (pixel_data[r_index_##N + 1])\
+ (pixel_data[lower_index_##N + 1]) >> 2;\
\
pixel_data[current_total_offset_##N + 2] = \
((uint32_t)upper_row_pixels[current_pixel_offset+kByteOffset##N + 2])\
+ (current_row_pixels[l_index_##N + 2])\
+ (pixel_data[r_index_##N + 2])\
+ (pixel_data[lower_index_##N + 2]) >> 2


class BlurImpls
{
public:
	static void BlurNaive(Bitmap& bitmap);

	static void BlurLoopUnroll3(Bitmap& bitmap);
	static void BlurLoopUnroll5(Bitmap& bitmap);
	static void BlurLoopUnroll7(Bitmap& bitmap);
	static void BlurLoopUnroll9(Bitmap& bitmap);
	static void BlurLoopUnroll11(Bitmap& bitmap);
	static void BlurLoopUnroll13(Bitmap& bitmap);
	static void BlurLoopUnroll15(Bitmap& bitmap);

	static void BlurCacheOpt(Bitmap& bitmap);

#ifndef ARM
	static void BlurAVX(Bitmap& bitmap);
#endif

#ifdef ARM
	static void BlurNeon(Bitmap& bitmap);
#endif

	static void BlurOpenMP(const Bitmap& bitmap, Bitmap& dest_bitmap);
};

