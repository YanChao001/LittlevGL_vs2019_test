#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <SDL.h>
#include "lvgl/lvgl.h"
#include "lv_drivers/display/monitor.h"
#include "lv_drivers/indev/mouse.h"
#include "lv_drivers/indev/keyboard.h"
#include "lv_examples/lv_apps/demo/demo.h"
#include "lv_examples/lv_apps/benchmark/benchmark.h"
#include "lv_examples/lv_tests/lv_test_theme/lv_test_theme_1.h"
#include "lv_examples/lv_tutorial/10_keyboard/lv_tutorial_keyboard.h"

#include "lv_png_decoder/lv_lodepng.h"

#include "lvgl_app.h"

static void hal_init(void);
static int tick_thread(void *data);

bool my_ready_cb(lv_fs_drv_t* drv);
lv_fs_res_t my_open_cb(lv_fs_drv_t* drv, void* file_p, const char* path, lv_fs_mode_t mode);
lv_fs_res_t my_close_cb(lv_fs_drv_t* drv, void* file_p);
lv_fs_res_t my_read_cb(lv_fs_drv_t* drv, void* file_p, void* buf, uint32_t btr, uint32_t* br);
lv_fs_res_t my_write_cb(lv_fs_drv_t* drv, void* file_p, const void* buf, uint32_t btw, uint32_t* bw);
lv_fs_res_t my_seek_cb(lv_fs_drv_t* drv, void* file_p, uint32_t pos);
lv_fs_res_t my_tell_cb(lv_fs_drv_t* drv, void* file_p, uint32_t* pos_p);
lv_fs_res_t my_trunc_cb(lv_fs_drv_t* drv, void* file_p);
lv_fs_res_t my_size_cb(lv_fs_drv_t* drv, void* file_p, uint32_t* size_p);
lv_fs_res_t my_rename_cb(lv_fs_drv_t* drv, const char* oldname, const char* newname);
lv_fs_res_t my_free_space_cb(lv_fs_drv_t* drv, uint32_t* total_p, uint32_t* free_p);

static lv_indev_t * kb_indev;

int main(int argc, char** argv)
{
    lv_init();
    hal_init();
	lv_lodepng_init();	//������ʼ��

    //demo_create();
    //benchmark_create();
	//lv_test_theme_2();
    //lv_test_theme_1(lv_theme_night_init(210, NULL));
    //lv_test_theme_1(lv_theme_night_init(100, NULL));
    //lv_test_theme_1(lv_theme_material_init(210, NULL));
    //lv_test_theme_1(lv_theme_alien_init(210, NULL));
    //lv_test_theme_1(lv_theme_zen_init(210, NULL));
    //lv_test_theme_1(lv_theme_nemo_init(210, NULL));
    //lv_test_theme_1(lv_theme_mono_init(210, NULL));
    //lv_test_theme_1(lv_theme_default_init(210, NULL));
    //lv_tutorial_keyboard(kb_indev);

	my_app_create();

    while (1)
	{
        lv_task_handler();
        Sleep(10);
    }
    return 0;
}

static int tick_thread(void* data)
{
	while (1)
	{
		lv_tick_inc(5);
		SDL_Delay(5);   /*Sleep for 1 millisecond*/
	}
	return 0;
}

static void hal_init(void)
{
    monitor_init();

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    static lv_disp_buf_t disp_buf1;
    static lv_color_t buf1_1[LV_HOR_RES_MAX*LV_VER_RES_MAX];
    lv_disp_buf_init(&disp_buf1, buf1_1, NULL, LV_HOR_RES_MAX*LV_VER_RES_MAX);

    disp_drv.buffer = &disp_buf1;
    disp_drv.flush_cb = monitor_flush;
    lv_disp_drv_register(&disp_drv);

    mouse_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read;
    lv_indev_drv_register(&indev_drv);

	lv_fs_drv_t drv;
	lv_fs_drv_init(&drv);                     /*������ʼ��*/
	drv.letter = 'D';                         /* ���ڱ�ʶ��������ԭ��drive��ָ�洢�豸���������������Ӳ���������������������� �� ����ע���Ĵ�д��ĸ */
	drv.file_size = sizeof(FILE*);			  /*�洢�ļ���������Ĵ�С*/
	drv.rddir_size = NULL;					  /*Size required to store a directory object (used by dir_open/close/read)*/
	drv.ready_cb = my_ready_cb;               /*�ж��������Ƿ����ʹ�õĻص�*/
	drv.open_cb = my_open_cb;                 /*��һ���ļ��Ļص� */
	drv.close_cb = my_close_cb;               /*�ر�һ���ļ��Ļص� */
	drv.read_cb = my_read_cb;                 /*�ļ���ȡ�ص� */
	drv.write_cb = my_write_cb;               /*�ļ�д��ص� */
	drv.seek_cb = my_seek_cb;                 /*�ļ�seek�ص� (�ƶ��ļ�ָ��) */
	drv.tell_cb = my_tell_cb;                 /*������ǰ�ļ�ָ��λ�ûص�  */
	drv.trunc_cb = my_trunc_cb;				  /*ɾ���ļ��ص�*/
	drv.size_cb = my_size_cb;                 /*�����ļ���С�ص� */
	drv.rename_cb = my_rename_cb;			  /*�������ļ��ص� */

	drv.dir_open_cb = NULL;					/*��Ŀ¼�Զ�ȡ�����ݻص� */
	drv.dir_read_cb = NULL;					/*��ȡĿ¼���ݻص�*/
	drv.dir_close_cb = NULL;				/*�ر�Ŀ¼�ص�*/

	drv.free_space_cb = my_free_space_cb;     /*�����������ϵĿ��ÿռ� */

	drv.user_data = NULL;					  /*�����Ҫ�����Զ�����������*/
	lv_fs_drv_register(&drv);                 /*���ע��Ӳ��*/

    SDL_CreateThread(tick_thread, "tick", NULL);
}

bool my_ready_cb(lv_fs_drv_t* drv)
{
	return true;
}

lv_fs_res_t my_open_cb(lv_fs_drv_t* drv, void* file_p, const char* path, lv_fs_mode_t mode)
{
	char temp[100];
	char count;

	temp[0] = drv->letter;
	temp[1] = ':';
	temp[2] = '\\';
	for (count = 3; path[count - 3] != '\0'; count++)
	{
		temp[count] = path[count - 3];
	}
	temp[count] = '\0';
	if (mode == LV_FS_MODE_WR)
	{
		*((FILE**)file_p) = fopen(temp, "wb");
	}
	else if(mode == LV_FS_MODE_RD)
	{
		*((FILE**)file_p) = fopen(temp, "rb");
	}
	else
	{
		return LV_FS_RES_INV_PARAM;
	}
	if (*((FILE**)file_p) == NULL)
	{
		return LV_FS_RES_UNKNOWN;
	}
	return LV_FS_RES_OK;
}

lv_fs_res_t my_close_cb(lv_fs_drv_t* drv, void* file_p)
{
	int res;

	res = fclose(*((FILE**)file_p));
	if (res == EOF)
	{
		return LV_FS_RES_UNKNOWN;
	}
	return LV_FS_RES_OK;
}

lv_fs_res_t my_read_cb(lv_fs_drv_t* drv, void* file_p, void* buf, uint32_t btr, uint32_t* br)
{
	*br = fread(buf, 1, btr, *((FILE**)file_p));
	return LV_FS_RES_OK;
}

lv_fs_res_t my_write_cb(lv_fs_drv_t* drv, void* file_p, const void* buf, uint32_t btw, uint32_t* bw)
{
	*bw = fwrite(buf, 1, btw, *((FILE**)file_p));
	return LV_FS_RES_OK;
}

lv_fs_res_t my_seek_cb(lv_fs_drv_t* drv, void* file_p, uint32_t pos)
{
	int res;

	res = fseek(*((FILE**)file_p), pos, SEEK_SET);
	if (res == EOF)
	{
		return LV_FS_RES_UNKNOWN;
	}
	return LV_FS_RES_OK;
}

lv_fs_res_t my_tell_cb(lv_fs_drv_t* drv, void* file_p, uint32_t* pos_p)
{
	*pos_p = ftell(*((FILE**)file_p));
	return LV_FS_RES_OK;
}

lv_fs_res_t my_trunc_cb(lv_fs_drv_t* drv, void* file_p)
{
	int res = 1;

	//res = remove(*((FILE**)file_p));
	if (res == EOF)
	{
		return LV_FS_RES_UNKNOWN;
	}
	return LV_FS_RES_OK;
}

lv_fs_res_t my_size_cb(lv_fs_drv_t* drv, void* file_p, uint32_t* size_p)
{
	fseek(*((FILE**)file_p), 0, SEEK_END);
	*size_p = ftell(*((FILE**)file_p));
	rewind(*((FILE**)file_p));
	return LV_FS_RES_OK;
}

lv_fs_res_t my_rename_cb(lv_fs_drv_t* drv, const char* oldname, const char* newname)
{
	int res;

	res = rename(oldname, newname);
	if (res == EOF)
	{
		return LV_FS_RES_UNKNOWN;
	}
	return LV_FS_RES_OK;
}

lv_fs_res_t my_free_space_cb(lv_fs_drv_t* drv, uint32_t* total_p, uint32_t* free_p)
{
	*total_p = 1024 * 100;
	*free_p = 1024 * 100;
	return LV_FS_RES_OK;
}