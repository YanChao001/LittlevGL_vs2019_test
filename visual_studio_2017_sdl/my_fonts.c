#include "my_font.h"
#include "lvgl/lvgl.h"

static bool my_get_glyph_dsc_cb(const lv_font_t* font, lv_font_glyph_dsc_t* dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next);
static const uint8_t* my_get_glyph_bitmap_cb(const lv_font_t* font, uint32_t unicode_letter);

extern lv_font_fmt_txt_dsc_t my_font_dsc;
lv_font_t my_font =
{
	.get_glyph_dsc = my_get_glyph_dsc_cb,
	.get_glyph_bitmap = my_get_glyph_bitmap_cb,
	.line_height = 19,
	.base_line = 4,
	.subpx = LV_FONT_SUBPX_NONE,
	.dsc = &my_font_dsc
};

static bool my_get_glyph_dsc_cb(const lv_font_t* font, lv_font_glyph_dsc_t* dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
{
	if ((unicode_letter > 0x19 && unicode_letter < 0x7f) || (unicode_letter > 0xf000 && unicode_letter < 0xf8a3))
	{
		bool res;
		res = lv_font_get_glyph_dsc_fmt_txt(font, dsc_out, unicode_letter, unicode_letter_next);
		return res;
	}
	else
	{
		dsc_out->adv_w = 16;
		dsc_out->box_h = 16;
		dsc_out->box_w = 16;
		dsc_out->ofs_x = 0;
		dsc_out->ofs_y = -2;
		dsc_out->bpp = 1;
		return true;
	}
}

static const uint8_t* my_get_glyph_bitmap_cb(const lv_font_t* font, uint32_t unicode_letter)
{
	if ((unicode_letter > 0x19 && unicode_letter < 0x7f) || (unicode_letter > 0xf000 && unicode_letter < 0xf8a3))
	{
		static const uint8_t* buff;
		buff = lv_font_get_bitmap_fmt_txt(font, unicode_letter);
		return buff;
	}
	else
	{
		lv_fs_file_t f_p;
		uint32_t offset;
		uint32_t bit_to_read;
		static uint8_t font_buff[32];
		if (unicode_letter < 65536)
		{
			offset = unicode_letter * 32;
			lv_fs_open(&f_p, "D:\\test\\unicode.bin", LV_FS_MODE_RD);
			lv_fs_seek(&f_p, offset);
			lv_fs_read(&f_p, font_buff, 32 , &bit_to_read);
			lv_fs_close(&f_p);
			return font_buff;
		}
		else
		{
			return NULL;
		}
	}
}
