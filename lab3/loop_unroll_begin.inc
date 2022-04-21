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

	for (int32_t i = 1; i < height_px - 1; i++)
	{
		int32_t current_row_offset = i * bytes_per_row;
		int32_t lower_row_offset = current_row_offset + bytes_per_row;

		for (int32_t j = 1; j < width_px - kWpl; j+= kWpl)
		{
			// Beginning index of current pixel data.
			int32_t current_pixel_offset = j * bytes_per_px;
