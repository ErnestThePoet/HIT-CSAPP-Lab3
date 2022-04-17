#include "bitmap.h"

int32_t Bitmap::width_px() const
{
	return this->info_header.width_px;
}

int32_t Bitmap::height_px() const
{
	return abs(this->info_header.height_px);
}
