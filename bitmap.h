#pragma once
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <utility>

#include <malloc.h>

#include "utilities.h"

struct Bitmap
{
public:
	struct BmpHeader
	{
		// the two-byte file type cannot be placed here or data won't be read into the struct correctly.
		// this is because memory alignment will pad 2 bytes after it.
		uint32_t file_size;
		uint16_t reserved_1;
		uint16_t reserved_2;
		uint32_t bmp_offset;
	};

	struct BmpInfoHeader
	{
		uint32_t header_size;
		int32_t width_px;
		int32_t height_px;
		uint16_t planes_count;
		uint16_t bits_per_pixel;
		uint32_t compress_type;
		uint32_t raw_data_size;
		int32_t h_res_ppm;
		int32_t v_res_ppm;
		uint32_t palette_color_count;
		uint32_t important_color_count;
	};

	// no palette bytes if 16-bit or higher
	struct BmpPalette
	{
		uint8_t b;
		uint8_t g;
		uint8_t r;
		uint8_t reserved;
	};

	Bitmap() :pixel_data_(NULL), pixel_data_size_(0), alloc_size_(0) {}
	~Bitmap()
	{
		if (this->pixel_data_)
		{
			_aligned_free(this->pixel_data_);
		}
	}

	Bitmap(const Bitmap& bitmap)
	{
		CopyFromBitmap(bitmap);
	}

	Bitmap& operator = (const Bitmap& bitmap)
	{
		CopyFromBitmap(bitmap);
		return *this;
	}

	uint8_t file_type[2]{};
	BmpHeader header{};
	BmpInfoHeader info_header{};
	std::vector<BmpPalette> palettes;

	// be sure to check pixel_data() to confirm whether successfully allocated
	// just provide bmp padded pixel data size. auto align memory that is a multiple of 64-byte
	void alloc_4096_aligned_pixel_data(size_t pixel_data_size);
	uint8_t* pixel_data() const;
	size_t pixel_data_size() const;

	int32_t width_px() const;
	int32_t height_px() const;

private:
	// 4096 byte aligned
	uint8_t* pixel_data_ = NULL;
	// actual padded size of bmp data, used to read/write bmp
	size_t pixel_data_size_ = 0;
	// allocated size that is a multiple of 64-byte
	size_t alloc_size_ = 0;

	void CopyFromBitmap(const Bitmap& bitmap)
	{
		this->file_type[0] = bitmap.file_type[0];
		this->file_type[1] = bitmap.file_type[1];

		this->header = bitmap.header;
		this->info_header = bitmap.info_header;
		this->palettes = bitmap.palettes;

		this->alloc_4096_aligned_pixel_data(bitmap.alloc_size_);

		if (this->pixel_data_ != NULL)
		{
			std::copy(bitmap.pixel_data_, bitmap.pixel_data_ + bitmap.alloc_size_, this->pixel_data_);
		}
	}
};

