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
		&bitmap.pixel_data[0],
		&bitmap.pixel_data[bytes_per_px * width_px],
		pixel_store_1.begin());

	std::vector<uint8_t> pixel_store_2(bytes_per_px * width_px);
	// copy second row data (except right paddings)
	std::copy(
		&bitmap.pixel_data[bytes_per_row],
		&bitmap.pixel_data[bytes_per_row + bytes_per_px * width_px],
		pixel_store_2.begin());

	uint8_t* const pixel_data = bitmap.pixel_data.data();
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
			pixel_data[current_row_offset + current_pixel_offset] = (upper_row_pixels[current_pixel_offset] >> 2)
				+ (current_row_pixels[l_index] >> 2)
				+ (pixel_data[r_index] >> 2)
				+ (pixel_data[lower_index] >> 2);

			pixel_data[current_row_offset + current_pixel_offset + 1] = (upper_row_pixels[current_pixel_offset+1] >> 2)
				+ (current_row_pixels[l_index + 1] >> 2)
				+ (pixel_data[r_index + 1] >> 2)
				+ (pixel_data[lower_index +1] >> 2);

			pixel_data[current_row_offset + current_pixel_offset + 2] = (upper_row_pixels[current_pixel_offset+2] >> 2)
				+ (current_row_pixels[l_index + 2] >> 2)
				+ (pixel_data[r_index + 2] >> 2)
				+ (pixel_data[lower_index +2] >> 2);
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

void BlurImpls::BlurCacheOpt(Bitmap& bitmap)
{
	int32_t width_px = bitmap.width_px();
	int32_t height_px = bitmap.height_px();
	int32_t bytes_per_row = ((width_px * bytes_per_px - 1) / 4 + 1) * 4;

	std::vector<uint8_t> pixel_store_1(bytes_per_px * width_px);
	// copy first row data (except right paddings)
	std::copy(
		&bitmap.pixel_data[0],
		&bitmap.pixel_data[bytes_per_px * width_px],
		pixel_store_1.begin());

	std::vector<uint8_t> pixel_store_2(bytes_per_px * width_px);
	// copy second row data (except right paddings)
	std::copy(
		&bitmap.pixel_data[bytes_per_row],
		&bitmap.pixel_data[bytes_per_row + bytes_per_px * width_px],
		pixel_store_2.begin());

	uint8_t* const pixel_data = bitmap.pixel_data.data();
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
			int32_t r_index = current_pixel_offset + bytes_per_px;
			int32_t lower_index = lower_row_offset + current_pixel_offset;

			uint8_t avg_b =
				(upper_row_pixels[current_pixel_offset] >> 2);
			uint8_t avg_g =
				(upper_row_pixels[current_pixel_offset + 1] >> 2);
			uint8_t avg_r =
				(upper_row_pixels[current_pixel_offset + 2] >> 2);


			avg_b += 
				(current_row_pixels[l_index] >> 2);
			avg_g +=
				(current_row_pixels[l_index + 1] >> 2);
			avg_r +=
				(current_row_pixels[l_index + 2] >> 2);


			avg_b +=
				(current_row_pixels[r_index] >> 2);
			avg_g +=
				(current_row_pixels[r_index + 1] >> 2);
			avg_r +=
				(current_row_pixels[r_index + 2] >> 2);


			avg_b +=
				(pixel_data[lower_index] >> 2);
			avg_g +=
				(pixel_data[lower_index + 1] >> 2);
			avg_r +=
				(pixel_data[lower_index + 2] >> 2);


			pixel_data[current_total_offset] = avg_b;
			pixel_data[current_total_offset + 1] = avg_g;
			pixel_data[current_total_offset + 2] = avg_r;
		}

		std::copy(
			pixel_data + lower_row_offset,
			pixel_data + lower_row_offset + bytes_per_px * width_px,
			upper_row_pixels);

		std::swap(upper_row_pixels, current_row_pixels);
	}
}
