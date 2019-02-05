#include <errno.h>
#include <time.h>


#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_system.h"

#include "mdns.h"
#include "tft.h"
#include "tftspi.h"

#include "button.h"
#include "display.h"
#include "wifi.h"

button_t button_a;
button_t button_b;

static void set_message(const char *message)
{
	Font curr_font = cfont;
	TFT_setFont(DEFAULT_FONT, NULL);
	TFT_fillWindow(TFT_BLACK);
	TFT_print(message, 5, 5);
    cfont = curr_font;
}

void app_main()
{
    esp_err_t ret;
    const esp_app_desc_t *app_desc;
    app_desc = esp_ota_get_app_description();

	display_init();

    WIFI_Initialize();


    display_set_header("m5stack starter");

	button_init(&button_a, BUTTON_A);
	button_init(&button_b, BUTTON_B);

    while(1) {
		int set_bits;
        display_update();
        vTaskDelay(500 / portTICK_RATE_MS);
		set_bits = xEventGroupWaitBits(
			button_event_group(),
			(1ULL << button_a.event_bit) | (1ULL << button_b.event_bit),
			pdTRUE, 
			pdFALSE, 
			500 / portTICK_RATE_MS);
		if(set_bits & (1<<button_a.event_bit)) {
			printf("Button A pressed!\n");
			set_message("Button A pressed");
		} else if(set_bits & (1<<button_b.event_bit)) {
			printf("Button B pressed!\n");
			set_message("Button B pressed");
		}
    }
}
// 999 is too high