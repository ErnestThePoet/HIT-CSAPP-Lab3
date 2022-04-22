}

		std::copy(
			pixel_data + lower_row_offset,
			pixel_data + lower_row_offset + bytes_per_px * width_px,
			upper_row_pixels);

		std::swap(upper_row_pixels, current_row_pixels);
	}