#pragma once
#include <cstdint>
#include <cstdlib>
#include <vector>

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

	uint8_t file_type[2]{};
	BmpHeader header{};
	BmpInfoHeader info_header{};
	std::vector<BmpPalette> palettes;
	// Experiments show that vector.data() has equal performance to naive heap array.
	// And in Release mode (/O2), vector[] has equal performance to vector.data()
	std::vector<uint8_t> pixel_data;

	int32_t width_px() const;
	int32_t height_px() const;
};

