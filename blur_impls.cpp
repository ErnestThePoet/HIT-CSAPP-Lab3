#include "blur_impls.h"

void BlurImpls::BlurNaive(Bitmap& bitmap)
{
	int32_t width_px = bitmap.width_px();
	int32_t height_px = bitmap.height_px();
	uint32_t bytes_per_px = bitmap.info_header.bits_per_pixel / 8;
	uint32_t bytes_per_row = ((width_px * bytes_per_px - 1) / 4 + 1) * 4;

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

			// UP+L+R+DOWN
			pixel_data[current_row_offset + current_pixel_offset] = (upper_row_pixels[current_pixel_offset] >> 2)
				+ (current_row_pixels[current_pixel_offset-bytes_per_px] >> 2)
				+ (pixel_data[current_row_offset + current_pixel_offset+bytes_per_px] >> 2)
				+ (pixel_data[lower_row_offset + current_pixel_offset] >> 2);

			pixel_data[current_row_offset + current_pixel_offset + 1] = (upper_row_pixels[current_pixel_offset+1] >> 2)
				+ (current_row_pixels[current_pixel_offset-bytes_per_px + 1] >> 2)
				+ (pixel_data[current_row_offset + current_pixel_offset+bytes_per_px + 1] >> 2)
				+ (pixel_data[lower_row_offset + current_pixel_offset+1] >> 2);

			pixel_data[current_row_offset + current_pixel_offset + 2] = (upper_row_pixels[current_pixel_offset+2] >> 2)
				+ (current_row_pixels[current_pixel_offset-bytes_per_px + 2] >> 2)
				+ (pixel_data[current_row_offset + current_pixel_offset+bytes_per_px + 2] >> 2)
				+ (pixel_data[lower_row_offset + current_pixel_offset+2] >> 2);
		}
		
		std::copy(
			pixel_data + lower_row_offset,
			pixel_data + lower_row_offset + bytes_per_px * width_px,
			upper_row_pixels);

		std::swap(upper_row_pixels, current_row_pixels);
	}
}

void BlurImpls::BlurMemoryBlocking(Bitmap& bitmap,const int block_size)
{
	int32_t width_px = bitmap.width_px();
	int32_t height_px = bitmap.height_px();
	int32_t bytes_per_px = bitmap.info_header.bits_per_pixel / 8;
	int32_t bytes_per_row = ((width_px * bytes_per_px - 1) / 4 + 1) * 4;
	int32_t bytes_per_block_row = block_size * bytes_per_px;

	std::vector<uint8_t> pixel_store_1(bytes_per_block_row);
	std::vector<uint8_t> pixel_store_2(bytes_per_block_row);

	uint8_t* const pixel_data = bitmap.pixel_data.data();
	uint8_t* upper_row_pixels = pixel_store_1.data();
	uint8_t* current_row_pixels = pixel_store_2.data();

	for (int32_t i = 1; i < height_px - block_size; i += block_size)
	{
		// row offset for the first row of current block
		int32_t current_block_row_offset = i * bytes_per_row;
		int32_t upper_block_row_offset = current_block_row_offset - bytes_per_row;

		for (int32_t j = 1; j < width_px - block_size; j += block_size)
		{
			// pixel (in-row) offset of first element of each row of current block
			int32_t current_block_pixel_offset = j * bytes_per_px;

			// total offset of first element of first row of current block
			int32_t current_block_total_offset = current_block_pixel_offset + current_block_row_offset;
			int32_t upper_block_total_offset = current_block_pixel_offset + upper_block_row_offset;

			// copy first mini row data
			std::copy(
				&bitmap.pixel_data[upper_block_total_offset],
				&bitmap.pixel_data[upper_block_total_offset + bytes_per_block_row],
				upper_row_pixels);

			// copy second mini row data
			std::copy(
				&bitmap.pixel_data[current_block_total_offset],
				&bitmap.pixel_data[current_block_total_offset + bytes_per_block_row],
				current_row_pixels);

			for (int32_t m = i; m < i + block_size; m++)
			{
				int32_t current_row_offset = m * bytes_per_row;
				int32_t lower_row_offset = current_row_offset + bytes_per_row;

				for (int32_t n = j; n < j + block_size; n++)
				{
					// Beginning index of current pixel data.
					int32_t current_pixel_offset = n * bytes_per_px;

					// UP+L+R+DOWN
					pixel_data[current_row_offset + current_pixel_offset] = (upper_row_pixels[current_pixel_offset] >> 2)
						+ (current_row_pixels[current_pixel_offset - bytes_per_px] >> 2)
						+ (pixel_data[current_row_offset + current_pixel_offset + bytes_per_px] >> 2)
						+ (pixel_data[lower_row_offset + current_pixel_offset] >> 2);

					pixel_data[current_row_offset + current_pixel_offset + 1] = (upper_row_pixels[current_pixel_offset + 1] >> 2)
						+ (current_row_pixels[current_pixel_offset - bytes_per_px + 1] >> 2)
						+ (pixel_data[current_row_offset + current_pixel_offset + bytes_per_px + 1] >> 2)
						+ (pixel_data[lower_row_offset + current_pixel_offset + 1] >> 2);

					pixel_data[current_row_offset + current_pixel_offset + 2] = (upper_row_pixels[current_pixel_offset + 2] >> 2)
						+ (current_row_pixels[current_pixel_offset - bytes_per_px + 2] >> 2)
						+ (pixel_data[current_row_offset + current_pixel_offset + bytes_per_px + 2] >> 2)
						+ (pixel_data[lower_row_offset + current_pixel_offset + 2] >> 2);
				}

				std::copy(
					pixel_data + lower_row_offset + current_block_pixel_offset,
					pixel_data + lower_row_offset + current_block_pixel_offset + bytes_per_block_row,
					upper_row_pixels);

				std::swap(upper_row_pixels, current_row_pixels);
			}
		}
	}

}
