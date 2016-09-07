#include <tizen.h>
#include "koreanfotnwatch.h"
#include "utils.h"

#include <string>

using pius::utils::LogD;

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
	Evas_Object *test;
} appdata_s;

#define TEXT_BUF_SIZE 256

const char* krsec[] = {
		"영", "일", "이", "삼", "사", "오", "육", "칠", "팔", "구", "십",
		"십일", "십이", "십삼", "십사", "십오", "십육", "십칠", "십팔", "십구", "이십",
		"이십일", "이십이", "이십삼", "이십사", "이십오", "이십육", "이십칠", "이십팔", "이십구", "삼십",
		"삼십일", "삼십이", "삼십삼", "삼십사", "삼십오", "삼십육", "삼십칠", "삼십팔", "삼십구", "사십",
		"사십일", "사십이", "사십삼", "사십사", "사십오", "사십육", "사십칠", "사십팔", "사십구", "오십",
		"오십일", "오십이", "오십삼", "오십사", "오십오", "오십육", "오십칠", "오십팔", "오십구"
};

static void update_watch(appdata_s *ad, watch_time_h watch_time, int ambient)
{
	char watch_text[TEXT_BUF_SIZE];
	int hour24, minute, second;

	if (watch_time == NULL)
		return;

	watch_time_get_hour24(watch_time, &hour24);
	watch_time_get_minute(watch_time, &minute);
	watch_time_get_second(watch_time, &second);
	snprintf(watch_text, TEXT_BUF_SIZE, "%s", krsec[second]);

	elm_object_part_text_set(ad->test, "text2", watch_text);
}

static void create_base_gui(appdata_s *ad, int width, int height)
{
	int ret;
	watch_time_h watch_time = NULL;

	/* Window */
	ret = watch_app_get_elm_win(&ad->win);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get window. err = %d", ret);
		return;
	}

	evas_object_resize(ad->win, width, height);

	/* Conformant */
	ad->conform = elm_conformant_add(ad->win);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	Evas_Object *tmp = evas_object_rectangle_add(evas_object_evas_get(ad->conform));
	evas_object_color_set(tmp, 255, 0, 0, 255);
	evas_object_move(tmp, width/2 - 50, height / 3);
	evas_object_resize(tmp, 100, 80);
	evas_object_show(tmp);

	ad->test = elm_layout_add(ad->conform);
	elm_layout_file_set(ad->test, pius::utils::GetResource("edje/watch.edj").c_str(), "main");
	evas_object_move(ad->test, width/2 - 50, height / 3);
	evas_object_resize(ad->test, 100, 80);
	LogD("%d, %d - ", width/2 - 50, height / 3);
	evas_object_show(ad->test);


	ret = watch_time_get_current_time(&watch_time);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get current time. err = %d", ret);

	update_watch(ad, watch_time, 0);
	watch_time_delete(watch_time);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool app_create(int width, int height, void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	appdata_s *ad = static_cast<appdata_s*>(data);

	pius::utils::InitializeConfigs("KoreanFontWatch");

	create_base_gui(ad, width, height);

	return true;
}

static void app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
}

static void app_terminate(void *data)
{
	/* Release all resources. */
}

static void app_time_tick(watch_time_h watch_time, void *data)
{
	/* Called at each second while your app is visible. Update watch UI. */
	appdata_s *ad = static_cast<appdata_s*>(data);
	update_watch(ad, watch_time, 0);
}

static void app_ambient_tick(watch_time_h watch_time, void *data)
{
	/* Called at each minute while the device is in ambient mode. Update watch UI. */
	appdata_s *ad = static_cast<appdata_s*>(data);
	update_watch(ad, watch_time, 1);
}

static void app_ambient_changed(bool ambient_mode, void *data)
{
	/* Update your watch UI to conform to the ambient mode */
}

static void watch_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	app_event_get_language(event_info, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void watch_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

int main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	watch_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;
	event_callback.time_tick = app_time_tick;
	event_callback.ambient_tick = app_ambient_tick;
	event_callback.ambient_changed = app_ambient_changed;

	watch_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
		APP_EVENT_LANGUAGE_CHANGED, watch_app_lang_changed, &ad);
	watch_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
		APP_EVENT_REGION_FORMAT_CHANGED, watch_app_region_changed, &ad);

	ret = watch_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_main() is failed. err = %d", ret);
	}

	return ret;
}

