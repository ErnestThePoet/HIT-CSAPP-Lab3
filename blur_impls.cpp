#include "blur_impls.h"

constexpr int32_t bytes_per_px = 3;

void BlurImpls::BlurNaive(Bitmap& bitmap)
{
	int32_t width_px = bitmap.width_px();
	int32_t height_px = bitmap.height_px();
	int32_t bytes_per_row = ((width_px * bytes_per_px - 1) / 4 + 1) * 4;

	std::vector<uint8_t> pixel_store_1(bytes_per_px * width_px);
	// copy first row data (except right paddings)
	std::copy(
		bitmap.pixel_data(),
		bitmap.pixel_data()+ bytes_per_px * width_px,
		pixel_store_1.begin());

	std::vector<uint8_t> pixel_store_2(bytes_per_px * width_px);
	// copy second row data (except right paddings)
	std::copy(
		bitmap.pixel_data()+bytes_per_row,
		bitmap.pixel_data()+bytes_per_row + bytes_per_px * width_px,
		pixel_store_2.begin());

	uint8_t* const pixel_data = bitmap.pixel_data();
	uint8_t* upper_row_pixels = pixel_store_1.data();
	uint8_t* current_row_pixels = pixel_store_2.data();

	for (int32_t i = 1; i < height_px - 1; i++)
	{
		int32_t current_row_offset = i * bytes_per_row;
		int32_t lower_row_offset = current_row_offset + bytes_per_row;

		for (int32_t j = 1; j < width_px - 1; j++)
		{
			// Beginning index of current pixel data.
			int32_t current_pixel_offset = j * bytes_per_px;

			int32_t current_total_offset = current_row_offset + current_pixel_offset;
			int32_t l_index = current_pixel_offset - bytes_per_px;
			int32_t r_index = current_row_offset + current_pixel_offset + bytes_per_px;
			int32_t lower_index = lower_row_offset + current_pixel_offset;

			// UP+L+R+DOWN
			pixel_data[current_row_offset + current_pixel_offset] =
				((uint32_t)upper_row_pixels[current_pixel_offset])
				+ (current_row_pixels[l_index])
				+ (pixel_data[r_index])
				+ (pixel_data[lower_index]) >> 2;

			pixel_data[current_row_offset + current_pixel_offset + 1] =
				((uint32_t)upper_row_pixels[current_pixel_offset + 1])
				+ (current_row_pixels[l_index + 1])
				+ (pixel_data[r_index + 1])
				+ (pixel_data[lower_index + 1]) >> 2;

			pixel_data[current_row_offset + current_pixel_offset + 2] =
				((uint32_t)upper_row_pixels[current_pixel_offset + 2])
				+ (current_row_pixels[l_index + 2])
				+ (pixel_data[r_index + 2])
				+ (pixel_data[lower_index + 2]) >> 2;
		}

		std::copy(
			pixel_data + lower_row_offset,
			pixel_data + lower_row_offset + bytes_per_px * width_px,
			upper_row_pixels);

		std::swap(upper_row_pixels, current_row_pixels);
	}
}


void BlurImpls::BlurLoopUnroll3(Bitmap& bitmap)
{
	constexpr int32_t kWpl = 3;

#include "loop_unroll_begin.inc"

	SINGLE_UNROLL_LOCALS(0);
	SINGLE_UNROLL_LOCALS(1);
	SINGLE_UNROLL_LOCALS(2);

	SINGLE_UNROLL(0);
	SINGLE_UNROLL(1);
	SINGLE_UNROLL(2);

#include "loop_unroll_end.inc"
}

void BlurImpls::BlurLoopUnroll5(Bitmap& bitmap)
{
	constexpr int32_t kWpl = 5;

#include "loop_unroll_begin.inc"

	SINGLE_UNROLL_LOCALS(0);
	SINGLE_UNROLL_LOCALS(1);
	SINGLE_UNROLL_LOCALS(2);
	SINGLE_UNROLL_LOCALS(3);
	SINGLE_UNROLL_LOCALS(4);

	SINGLE_UNROLL(0);
	SINGLE_UNROLL(1);
	SINGLE_UNROLL(2);
	SINGLE_UNROLL(3);
	SINGLE_UNROLL(4);

#include "loop_unroll_end.inc"
}

void BlurImpls::BlurLoopUnroll7(Bitmap& bitmap)
{
	constexpr int32_t kWpl = 7;

#include "loop_unroll_begin.inc"

	SINGLE_UNROLL_LOCALS(0);
	SINGLE_UNROLL_LOCALS(1);
	SINGLE_UNROLL_LOCALS(2);
	SINGLE_UNROLL_LOCALS(3);
	SINGLE_UNROLL_LOCALS(4);
	SINGLE_UNROLL_LOCALS(5);
	SINGLE_UNROLL_LOCALS(6);

	SINGLE_UNROLL(0);
	SINGLE_UNROLL(1);
	SINGLE_UNROLL(2);
	SINGLE_UNROLL(3);
	SINGLE_UNROLL(4);
	SINGLE_UNROLL(5);
	SINGLE_UNROLL(6);

#include "loop_unroll_end.inc"
}

void BlurImpls::BlurLoopUnroll9(Bitmap& bitmap)
{
	constexpr int32_t kWpl = 9;

#include "loop_unroll_begin.inc"

	SINGLE_UNROLL_LOCALS(0);
	SINGLE_UNROLL_LOCALS(1);
	SINGLE_UNROLL_LOCALS(2);
	SINGLE_UNROLL_LOCALS(3);
	SINGLE_UNROLL_LOCALS(4);
	SINGLE_UNROLL_LOCALS(5);
	SINGLE_UNROLL_LOCALS(6);
	SINGLE_UNROLL_LOCALS(7);
	SINGLE_UNROLL_LOCALS(8);

	SINGLE_UNROLL(0);
	SINGLE_UNROLL(1);
	SINGLE_UNROLL(2);
	SINGLE_UNROLL(3);
	SINGLE_UNROLL(4);
	SINGLE_UNROLL(5);
	SINGLE_UNROLL(6);
	SINGLE_UNROLL(7);
	SINGLE_UNROLL(8);

#include "loop_unroll_end.inc"
}

void BlurImpls::BlurLoopUnroll11(Bitmap& bitmap)
{
	constexpr int32_t kWpl = 11;

#include "loop_unroll_begin.inc"

	SINGLE_UNROLL_LOCALS(0);
	SINGLE_UNROLL_LOCALS(1);
	SINGLE_UNROLL_LOCALS(2);
	SINGLE_UNROLL_LOCALS(3);
	SINGLE_UNROLL_LOCALS(4);
	SINGLE_UNROLL_LOCALS(5);
	SINGLE_UNROLL_LOCALS(6);
	SINGLE_UNROLL_LOCALS(7);
	SINGLE_UNROLL_LOCALS(8);
	SINGLE_UNROLL_LOCALS(9);
	SINGLE_UNROLL_LOCALS(10);

	SINGLE_UNROLL(0);
	SINGLE_UNROLL(1);
	SINGLE_UNROLL(2);
	SINGLE_UNROLL(3);
	SINGLE_UNROLL(4);
	SINGLE_UNROLL(5);
	SINGLE_UNROLL(6);
	SINGLE_UNROLL(7);
	SINGLE_UNROLL(8);
	SINGLE_UNROLL(9);
	SINGLE_UNROLL(10);

#include "loop_unroll_end.inc"
}

void BlurImpls::BlurLoopUnroll13(Bitmap& bitmap)
{
	constexpr int32_t kWpl = 13;

#include "loop_unroll_begin.inc"

	SINGLE_UNROLL_LOCALS(0);
	SINGLE_UNROLL_LOCALS(1);
	SINGLE_UNROLL_LOCALS(2);
	SINGLE_UNROLL_LOCALS(3);
	SINGLE_UNROLL_LOCALS(4);
	SINGLE_UNROLL_LOCALS(5);
	SINGLE_UNROLL_LOCALS(6);
	SINGLE_UNROLL_LOCALS(7);
	SINGLE_UNROLL_LOCALS(8);
	SINGLE_UNROLL_LOCALS(9);
	SINGLE_UNROLL_LOCALS(10);
	SINGLE_UNROLL_LOCALS(11);
	SINGLE_UNROLL_LOCALS(12);

	SINGLE_UNROLL(0);
	SINGLE_UNROLL(1);
	SINGLE_UNROLL(2);
	SINGLE_UNROLL(3);
	SINGLE_UNROLL(4);
	SINGLE_UNROLL(5);
	SINGLE_UNROLL(6);
	SINGLE_UNROLL(7);
	SINGLE_UNROLL(8);
	SINGLE_UNROLL(9);
	SINGLE_UNROLL(10);
	SINGLE_UNROLL(11);
	SINGLE_UNROLL(12);

#include "loop_unroll_end.inc"
}

void BlurImpls::BlurLoopUnroll15(Bitmap& bitmap)
{
	constexpr int32_t kWpl = 15;

#include "loop_unroll_begin.inc"

	SINGLE_UNROLL_LOCALS(0);
	SINGLE_UNROLL_LOCALS(1);
	SINGLE_UNROLL_LOCALS(2);
	SINGLE_UNROLL_LOCALS(3);
	SINGLE_UNROLL_LOCALS(4);
	SINGLE_UNROLL_LOCALS(5);
	SINGLE_UNROLL_LOCALS(6);
	SINGLE_UNROLL_LOCALS(7);
	SINGLE_UNROLL_LOCALS(8);
	SINGLE_UNROLL_LOCALS(9);
	SINGLE_UNROLL_LOCALS(10);
	SINGLE_UNROLL_LOCALS(11);
	SINGLE_UNROLL_LOCALS(12);
	SINGLE_UNROLL_LOCALS(13);
	SINGLE_UNROLL_LOCALS(14);

	SINGLE_UNROLL(0);
	SINGLE_UNROLL(1);
	SINGLE_UNROLL(2);
	SINGLE_UNROLL(3);
	SINGLE_UNROLL(4);
	SINGLE_UNROLL(5);
	SINGLE_UNROLL(6);
	SINGLE_UNROLL(7);
	SINGLE_UNROLL(8);
	SINGLE_UNROLL(9);
	SINGLE_UNROLL(10);
	SINGLE_UNROLL(11);
	SINGLE_UNROLL(12);
	SINGLE_UNROLL(13);
	SINGLE_UNROLL(14);

#include "loop_unroll_end.inc"
}

void BlurImpls::BlurCacheOpt(Bitmap& bitmap)
{
	int32_t width_px = bitmap.width_px();
	int32_t height_px = bitmap.height_px();
	int32_t bytes_per_row = ((width_px * bytes_per_px - 1) / 4 + 1) * 4;

	std::vector<uint8_t> pixel_store_1(bytes_per_px * width_px);
	// copy first row data (except right paddings)
	std::copy(
		bitmap.pixel_data(),
		bitmap.pixel_data()+bytes_per_px * width_px,
		pixel_store_1.begin());

	std::vector<uint8_t> pixel_store_2(bytes_per_px * width_px);
	// copy second row data (except right paddings)
	std::copy(
		bitmap.pixel_data()+bytes_per_row,
		bitmap.pixel_data()+bytes_per_row + bytes_per_px * width_px,
		pixel_store_2.begin());

	uint8_t* const pixel_data = bitmap.pixel_data();
	uint8_t* upper_row_pixels = pixel_store_1.data();
	uint8_t* current_row_pixels = pixel_store_2.data();


	int32_t inner_loop_ubound = bytes_per_px * width_px - bytes_per_px;

	for (int32_t i = 1; i < height_px - 1; i++)
	{
		int32_t current_row_offset = i * bytes_per_row;
		int32_t lower_row_offset = current_row_offset + bytes_per_row;

		for (int32_t j = bytes_per_px; j < inner_loop_ubound; j += bytes_per_px)
		{
			// Beginning index of current pixel data.
			int32_t current_total_offset = current_row_offset + j;

			int32_t l_index = j - bytes_per_px;
			int32_t r_index = current_row_offset + j + bytes_per_px;
			int32_t lower_index = lower_row_offset + j;

			uint32_t avg_b =
				(upper_row_pixels[j]);
			uint32_t avg_g =
				(upper_row_pixels[j + 1]);
			uint32_t avg_r =
				(upper_row_pixels[j + 2]);


			avg_b +=
				(current_row_pixels[l_index]);
			avg_g +=
				(current_row_pixels[l_index + 1]);
			avg_r +=
				(current_row_pixels[l_index + 2]);



			avg_b +=
				(pixel_data[r_index]);
			avg_g +=
				(pixel_data[r_index + 1]);
			avg_r +=
				(pixel_data[r_index + 2]);



			avg_b +=
				(pixel_data[lower_index]);
			avg_g +=
				(pixel_data[lower_index + 1]);
			avg_r +=
				(pixel_data[lower_index + 2]);



			pixel_data[current_total_offset] = avg_b >> 2;
			pixel_data[current_total_offset + 1] = avg_g >> 2;
			pixel_data[current_total_offset + 2] = avg_r >> 2;
		}


		std::copy(
			pixel_data + lower_row_offset,
			pixel_data + lower_row_offset + bytes_per_px * width_px,
			upper_row_pixels);

		std::swap(upper_row_pixels, current_row_pixels);
	}
}

// requires AVX, AVX2
void BlurImpls::BlurAVX(Bitmap& bitmap)
{
	// pixels per inner loop
	constexpr int32_t kWpl = 32;

	int32_t width_px = bitmap.width_px();
	int32_t height_px = bitmap.height_px();
	int32_t bytes_per_row = ((width_px * bytes_per_px - 1) / 4 + 1) * 4;

	std::vector<uint8_t> pixel_store_1(bytes_per_px * width_px);
	// copy first row data (except right paddings)
	std::copy(
		bitmap.pixel_data(),
		bitmap.pixel_data()+bytes_per_px * width_px,
		pixel_store_1.begin());

	std::vector<uint8_t> pixel_store_2(bytes_per_px * width_px);
	// copy second row data (except right paddings)
	std::copy(
		bitmap.pixel_data()+bytes_per_row,
		bitmap.pixel_data()+bytes_per_row + bytes_per_px * width_px,
		pixel_store_2.begin());

	uint8_t* const pixel_data = bitmap.pixel_data();
	uint8_t* upper_row_pixels = pixel_store_1.data();
	uint8_t* current_row_pixels = pixel_store_2.data();

	int32_t inner_loop_ubound = bytes_per_px * width_px - bytes_per_px - kWpl + 1;
	static const __m256i kPackedSrliMask = _mm256_set1_epi16((short)0xFCFF);

	for (int32_t i = 1; i < height_px - 1; i++)
	{
		int32_t current_row_offset = i * bytes_per_row;
		int32_t lower_row_offset = current_row_offset + bytes_per_row;

		for (int32_t j = bytes_per_px; j < inner_loop_ubound; j += kWpl)
		{
			int32_t current_total_offset = current_row_offset + j;

			__m256i packed_pixel_data_upper = 
				_mm256_loadu_si256((__m256i*)(upper_row_pixels + j));

			__m256i packed_pixel_data_l = 
				_mm256_loadu_si256((__m256i*)(current_row_pixels + j - bytes_per_px));

			__m256i packed_pixel_data_r =
				_mm256_loadu_si256((__m256i*)(pixel_data + current_total_offset + bytes_per_px));

			__m256i packed_pixel_data_lower =
				_mm256_loadu_si256((__m256i*)(pixel_data + lower_row_offset + j));

			// emulate _mm256_srli_epi8 with _mm256_srli_epi16 and _mm256_and_si256:
			// 1. and the original pack to set the lowest 2 bits of each even-indexed byte to 0.
			// 2. then _mm256_srli_epi16 by 2 and we get a result as if each byte in original pack
			//    is _mm256_srli_epi8 shifted by 2.

			packed_pixel_data_upper = _mm256_and_si256(packed_pixel_data_upper, kPackedSrliMask);
			packed_pixel_data_upper = _mm256_srli_epi16(packed_pixel_data_upper, 2);

			packed_pixel_data_l = _mm256_and_si256(packed_pixel_data_l, kPackedSrliMask);
			packed_pixel_data_l = _mm256_srli_epi16(packed_pixel_data_l, 2);

			packed_pixel_data_r = _mm256_and_si256(packed_pixel_data_r, kPackedSrliMask);
			packed_pixel_data_r = _mm256_srli_epi16(packed_pixel_data_r, 2);

			packed_pixel_data_lower = _mm256_and_si256(packed_pixel_data_lower, kPackedSrliMask);
			packed_pixel_data_lower = _mm256_srli_epi16(packed_pixel_data_lower, 2);


			__m256i packed_avg_pixel_data = _mm256_add_epi8(
				_mm256_add_epi8(
					packed_pixel_data_upper,
					packed_pixel_data_l
				),
				_mm256_add_epi8(
					packed_pixel_data_r,
					packed_pixel_data_lower
				)
			);

			_mm256_storeu_si256((__m256i*)(pixel_data + current_total_offset), packed_avg_pixel_data);
		}

		std::copy(
			pixel_data + lower_row_offset,
			pixel_data + lower_row_offset + bytes_per_px * width_px,
			upper_row_pixels);

		std::swap(upper_row_pixels, current_row_pixels);
	}
}

void BlurImpls::BlurOpenMP(const Bitmap& bitmap, Bitmap& dest_bitmap)
{
	int32_t width_px = bitmap.width_px();
	int32_t height_px = bitmap.height_px();
	int32_t bytes_per_row = ((width_px * bytes_per_px - 1) / 4 + 1) * 4;

	const uint8_t* const source_pixel_data = bitmap.pixel_data();
	uint8_t* const dest_pixel_data = dest_bitmap.pixel_data();

#pragma omp parallel for
	for (int32_t i = 1; i < height_px - 1; i++)
	{
		int32_t current_row_offset = i * bytes_per_row;
		int32_t upper_row_offset = current_row_offset - bytes_per_row;
		int32_t lower_row_offset = current_row_offset + bytes_per_row;

		for (int32_t j = 1; j < width_px - 1; j++)
		{
			// Beginning index of current pixel data.
			int32_t current_pixel_offset = j * bytes_per_px;

			int32_t current_total_offset = current_row_offset + current_pixel_offset;
			int32_t l_index = current_total_offset - bytes_per_px;
			int32_t r_index = current_total_offset + bytes_per_px;
			int32_t upper_index = upper_row_offset + current_pixel_offset;
			int32_t lower_index = lower_row_offset + current_pixel_offset;

			// UP+L+R+DOWN
			dest_pixel_data[current_total_offset] =
				((uint32_t)source_pixel_data[upper_index])
				+ (source_pixel_data[l_index])
				+ (source_pixel_data[r_index])
				+ (source_pixel_data[lower_index]) >> 2;

			dest_pixel_data[current_total_offset + 1] =
				((uint32_t)source_pixel_data[upper_index + 1])
				+ (source_pixel_data[l_index + 1])
				+ (source_pixel_data[r_index + 1])
				+ (source_pixel_data[lower_index + 1]) >> 2;

			dest_pixel_data[current_total_offset + 2] =
				((uint32_t)source_pixel_data[upper_index + 2])
				+ (source_pixel_data[l_index + 2])
				+ (source_pixel_data[r_index + 2])
				+ (source_pixel_data[lower_index + 2]) >> 2;
		}
	}
}
