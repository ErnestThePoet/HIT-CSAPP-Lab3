#include "bitmap.h"

void Bitmap::alloc_4096_aligned_pixel_data(size_t size)
{
	// ensure a multiple of 64-byte
	size_t actual_alloc_size =
		size == 0 ? 0 : (((size - 1) / 64 + 1) * 64);

	if (this->pixel_data_==NULL)
	{
		this->pixel_data_ =
			static_cast<uint8_t*>(_aligned_malloc(actual_alloc_size, 4096));
	}
	else
	{
		this->pixel_data_ =
			static_cast<uint8_t*>(_aligned_realloc(this->pixel_data_, actual_alloc_size, 4096));
	}

	if (this->pixel_data_ == NULL)
	{
		std::cerr << BRACKETED_LINE("Failed to allocate bitmap pixel data buffer.");
		this->pixel_data_size_ = this->alloc_size_ = 0;
	}
	else
	{
		this->pixel_data_size_ = size;
		this->alloc_size_ = actual_alloc_size;
	}
}

uint8_t* Bitmap::pixel_data() const
{
	return this->pixel_data_;
}

size_t Bitmap::pixel_data_size() const
{
	return this->pixel_data_size_;
}

int32_t Bitmap::width_px() const
{
	return this->info_header.width_px;
}

int32_t Bitmap::height_px() const
{
	return abs(this->info_header.height_px);
}
