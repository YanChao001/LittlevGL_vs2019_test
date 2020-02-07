#include "lvgl_app.h"
#include <stdio.h>

static void btn1_cb(lv_obj_t* obj, lv_event_t event);
static void create_win1(void);

void my_app_create(void)
{
	LV_FONT_DECLARE(my_font);
	lv_theme_t* theme1 = lv_theme_zen_init(150, &my_font);
	lv_theme_set_current(theme1);

	lv_obj_t* scr = lv_obj_create(NULL, NULL);
	lv_disp_load_scr(scr);

	lv_obj_t* icon = lv_img_create(lv_scr_act(), NULL);
	lv_img_set_src(icon, "D:\\test\\test.bin");

	lv_obj_t* btn1 = lv_btn_create(lv_scr_act(), NULL);
	//lv_obj_set_size(btn1, 40, 40);
	lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_btn_set_fit(btn1, LV_FIT_TIGHT);
	lv_obj_set_event_cb(btn1, btn1_cb);
	lv_obj_t* label = lv_label_create(btn1, NULL);
	lv_label_set_text(label, "汉字测试"LV_SYMBOL_SETTINGS);
}

static void btn1_cb(lv_obj_t* obj, lv_event_t event)
{
	if (event == LV_EVENT_CLICKED)
	{
		create_win1();
	}
}
	
void lv_win1_close_cb(lv_obj_t* btn, lv_event_t event)
{
	if (event == LV_EVENT_RELEASED)
	{
		lv_obj_t* win = lv_win_get_from_btn(btn);
		lv_obj_del(win);
		lv_mem_free(win->user_data);
	}
}

static void create_win1(void)
{
	lv_fs_file_t f_p;
	uint32_t f_size, r_size;
	uint8_t* font_buff;
	lv_fs_open(&f_p, "D:\\TEST\\ReadMe.txt", LV_FS_MODE_RD);
	lv_fs_size(&f_p, &f_size);
	font_buff = lv_mem_alloc(f_size + 1);
	lv_fs_read(&f_p, font_buff, f_size, &r_size);
	lv_fs_close(&f_p);

	lv_obj_t* win = lv_win_create(lv_scr_act(), NULL);
	lv_win_set_title(win, "测试窗口");
	lv_obj_set_size(win, 200, 200);
	lv_obj_align(win, NULL, LV_ALIGN_CENTER, 0, 0);
	win->user_data = font_buff;

	lv_obj_t* close_btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);
	lv_win_set_btn_size(win, 30);
	lv_obj_set_event_cb(close_btn, lv_win1_close_cb);

	lv_obj_t* txt = lv_label_create(win, NULL);
	lv_label_set_text(txt, font_buff);
}
