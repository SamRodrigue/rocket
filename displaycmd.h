// Command generator
// Fundamental commands
uint8_t display_set_contrast(uint8_t contrast_value, uint8_t *buffer)
{
	buffer[0] = 0x81;
	buffer[1] = contrast_value;
	return 2;
}

uint8_t display_set_follow_ram(bool enable, uint8_t *buffer)
{
	buffer[0] = enable ? 0xa4 : 0xa5;
	return 1;
}

uint8_t display_set_inversion(bool enable, uint8_t *buffer)
{
	buffer[0] = enable ? 0xa6 : 0xa7;
	return 1;
}

uint8_t display_set_enable(bool enable, uint8_t *buffer)
{
	buffer[0] = enable ? 0xaf : 0xae;
	return 1;
}

// Scrolling commands
enum display_scroll_frame_interval
{
	scroll_2_frames   = 0b111,
	scroll_3_frames   = 0b100,
	scroll_4_frames   = 0b101,
	scroll_5_frames   = 0b000,
	scroll_25_frames  = 0b110,
	scroll_64_frames  = 0b001,
	scroll_128_frames = 0b010,
	scroll_256_frames = 0b011
};

uint8_t display_set_horizontal_scroll(bool right_scroll, uint8_t start_page, enum display_scroll_frame_interval frame_interval, uint8_t end_page, uint8_t *buffer)
{
	buffer[0] = 0x26 | (right_scroll ? 0x1 : 0x0);
	buffer[1] = 0x00;
	buffer[2] = start_page;
	buffer[3] = (uint8_t)frame_interval;
	buffer[4] = end_page;
	buffer[5] = 0x00;
	buffer[6] = 0xff;

	return 7;
}

uint8_t display_set_diagonal_scroll(bool right_scroll, uint8_t start_page, enum display_scroll_frame_interval frame_interval, uint8_t end_page, uint8_t vertical_scrolling_offset, uint8_t *buffer)
{
	buffer[0] = 0x29 | (right_scroll ? 0x1 : 0x2);
	buffer[1] = 0x00;
	buffer[2] = start_page;
	buffer[3] = (uint8_t)frame_interval;
	buffer[4] = end_page;
	buffer[5] = vertical_scrolling_offset;

	return 6;
}

uint8_t display_set_scrol(bool enable, uint8_t *buffer)
{
	buffer[0] = 0x2e | (enable ? 0x1 : 0x0);

	return 1;
}

uint8_t display_set_vertical_scroll_area(uint8_t start_row, uint8_t scroll_rows, uint8_t *buffer)
{
	buffer[0] = 0xa3;
	buffer[1] = start_row;
	buffer[2] = scroll_rows;

	return 3;
}