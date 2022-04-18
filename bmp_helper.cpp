#include "bmp_helper.h"

Bitmap BmpHelper::Load(const std::string& file_name)
{
	std::ifstream ifs(file_name, std::ios::in | std::ios::binary);
	if (!ifs.is_open())
	{
		std::cerr << BRACKETED_LINE("Cannot load bmp: " << file_name << " cannot be opened.");
		return Bitmap();
	}

	Bitmap bitmap;

	ifs.read(reinterpret_cast<char*>(bitmap.file_type), 2);
	ifs.read(reinterpret_cast<char*>(&bitmap.header), sizeof(Bitmap::BmpHeader));
	ifs.read(reinterpret_cast<char*>(&bitmap.info_header), sizeof(Bitmap::BmpInfoHeader));

	bitmap.palettes.resize(bitmap.info_header.palette_color_count);

	ifs.read(
		reinterpret_cast<char*>(bitmap.palettes.data()),
		sizeof(Bitmap::BmpPalette) * bitmap.palettes.size());

	size_t bmp_data_size =
		// each row size in bytes
		((bitmap.info_header.width_px * bitmap.info_header.bits_per_pixel / 8 - 1) / 4 + 1) * 4
		// row count
		* abs(bitmap.info_header.height_px);

	bitmap.alloc_4096_aligned_pixel_data(bmp_data_size);

	if (bitmap.pixel_data() == NULL)
	{
		std::cerr << BRACKETED_LINE(
			"Cannot load bitmap: Failed to allocate 4096-byte aligned pixel data buffer.");
		return Bitmap();
	}

	ifs.read(reinterpret_cast<char*>(bitmap.pixel_data()), bmp_data_size);

	ifs.close();

	return bitmap;
}

void BmpHelper::PrintBitmapInfo(const Bitmap& bitmap)
{
	using std::cout;
	using std::endl;

	cout << "File type: "
		<< static_cast<char>(bitmap.file_type[0])
		<< static_cast<char>(bitmap.file_type[1])
		<< endl;
	cout << "File size: " << bitmap.header.file_size << endl;
	cout << "Reserved 1: " << bitmap.header.reserved_1 << endl;
	cout << "Reserved 2: " << bitmap.header.reserved_2 << endl;
	cout << "BMP offset: " << bitmap.header.bmp_offset << endl << endl;

	cout << "Header size: " << bitmap.info_header.header_size << endl;
	cout << "Width in px: " << bitmap.info_header.width_px << endl;
	cout << "Height in px: " << bitmap.info_header.height_px << endl;
	cout << "Plane count: " << bitmap.info_header.planes_count << endl;
	cout << "Bits per pixel: " << bitmap.info_header.bits_per_pixel << endl;
	cout << "Compress type: " << bitmap.info_header.compress_type << endl;
	cout << "Raw data size: " << bitmap.info_header.raw_data_size << endl;
	cout << "H resolution in ppm: " << bitmap.info_header.h_res_ppm << endl;
	cout << "V resolution in ppm: " << bitmap.info_header.v_res_ppm << endl;
	cout << "Palette color count: " << bitmap.info_header.palette_color_count << endl;
	cout << "Important color count: " << bitmap.info_header.important_color_count << endl;

	constexpr size_t kPixelsToPrint = 5;

	uint32_t bytes_per_px = bitmap.info_header.bits_per_pixel / 8;

	if (bitmap.pixel_data_size() < kPixelsToPrint * bytes_per_px)
	{
		return;
	}

	cout << "First " << kPixelsToPrint << " pixel data: ";

	for (size_t i = 0; i < kPixelsToPrint; i++)
	{
		for (size_t j = 0; j < bytes_per_px; j++)
		{
			cout << (int)bitmap.pixel_data()[i * bytes_per_px + j];
			if (j != bytes_per_px - 1)
			{
				cout << ',';
			}
		}

		cout << "  ";
	}

	cout << endl;
}

void BmpHelper::Save(const Bitmap& bitmap, const std::string& file_name)
{
	std::ofstream ofs(file_name, std::ios::out | std::ios::binary);
	if (!ofs.is_open())
	{
		std::cerr << BRACKETED_LINE("Cannot save bmp: " << file_name << " cannot be opened.");
		return;
	}

	ofs.write(reinterpret_cast<const char*>(bitmap.file_type), 2);
	ofs.write(reinterpret_cast<const char*>(&bitmap.header), sizeof(Bitmap::BmpHeader));
	ofs.write(reinterpret_cast<const char*>(&bitmap.info_header), sizeof(Bitmap::BmpInfoHeader));
	ofs.write(reinterpret_cast<const char*>(
		bitmap.palettes.data()),
		sizeof(Bitmap::BmpPalette) * bitmap.palettes.size());
	ofs.write(reinterpret_cast<const char*>(bitmap.pixel_data()), bitmap.pixel_data_size());

	ofs.close();
}