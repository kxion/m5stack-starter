#include "display.h"

#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_timer.h"
#include "tft.h"
#include "tftspi.h"

#include "wifi.h"


static time_t time_now, time_last = 0;
static char tmp_buff[64];

static void _dispTime()
{
    static struct tm* tm_info;
	Font curr_font = cfont;
    if (_width < 240) TFT_setFont(DEF_SMALL_FONT, NULL);
	else TFT_setFont(DEFAULT_FONT, NULL);

    time(&time_now);
	time_last = time_now;
	tm_info = localtime(&time_now);
	sprintf(tmp_buff, "%02d:%02d:%02d UTC", tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
	TFT_print(tmp_buff, 3, _height-TFT_getfontheight()-5);

    cfont = curr_font;
}

static void _dispIpAddr()
{
    unsigned char ip[4];
    WIFI_GetIp(ip);
    snprintf(tmp_buff, sizeof(tmp_buff), "%d.%d.%d.%d ", ip[0], ip[1], ip[2], ip[3]);
    TFT_print(tmp_buff, RIGHT, _height-TFT_getfontheight()-5);
}

void display_init()
{
    esp_err_t ret;

    TFT_PinsInit();

	esp_timer_init();

    vTaskDelay(500 / portTICK_RATE_MS);

    printf("Initializing TFT display...\r\n");
    printf("==============================\r\n");

    spi_lobo_device_handle_t spi;
	
    spi_lobo_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,				// set SPI MISO pin
        .mosi_io_num=PIN_NUM_MOSI,				// set SPI MOSI pin
        .sclk_io_num=PIN_NUM_CLK,				// set SPI CLK pin
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
		.max_transfer_sz = 6*1024,
    };
    spi_lobo_device_interface_config_t devcfg={
        .clock_speed_hz=8000000,                // Initial clock out at 8 MHz
        .mode=0,                                // SPI mode 0
        .spics_io_num=-1,                       // we will use external CS pin
		.spics_ext_io_num=PIN_NUM_CS,           // external CS pin
		.flags=LB_SPI_DEVICE_HALFDUPLEX,        // ALWAYS SET  to HALF DUPLEX MODE!! for display spi
    };
    // ==================================================================
	// ==== Initialize the SPI bus and attach the LCD to the SPI bus ====

	ret=spi_lobo_bus_add_device(TFT_HSPI_HOST, &buscfg, &devcfg, &spi);
    assert(ret==ESP_OK);
	printf("SPI: display device added to spi bus (%d)\r\n",  TFT_HSPI_HOST);
	disp_spi = spi;

	// ==== Test select/deselect ====
	ret = spi_lobo_device_select(spi, 1);
    assert(ret==ESP_OK);
	ret = spi_lobo_device_deselect(spi);
    assert(ret==ESP_OK);

	printf("SPI: attached display device, speed=%u\r\n", spi_lobo_get_speed(spi));
	printf("SPI: bus uses native pins: %s\r\n", spi_lobo_uses_native_pins(spi) ? "true" : "false");

    printf("SPI: display init...\r\n");
	TFT_display_init();
    printf("OK\r\n");
	
	// ---- Detect maximum read speed ----
	max_rdclock = find_rd_speed();
	printf("SPI: Max rd speed = %u\r\n", max_rdclock);

    // ==== Set SPI clock used for display operations ====
	spi_lobo_set_speed(spi, DEFAULT_SPI_CLOCK);
	printf("SPI: Changed speed to %u\r\n", spi_lobo_get_speed(spi));

    TFT_setFont(DEFAULT_FONT, NULL);

    TFT_setclipwin(1, TFT_getfontheight()+1, _width-1, _height - TFT_getfontheight() - 1);
}

//---------------------------------
void display_set_header(const char *info)
{    
	TFT_fillScreen(TFT_BLACK);
	TFT_resetclipwin();

	_fg = TFT_YELLOW;
	_bg = (color_t){ 80, 80, 80 };

    if (_width < 240) TFT_setFont(DEF_SMALL_FONT, NULL);
	else TFT_setFont(DEFAULT_FONT, NULL);
	TFT_fillRect(0, 0, _width-1, TFT_getfontheight()+8, _bg);
	TFT_drawRect(0, 0, _width-1, TFT_getfontheight()+8, TFT_CYAN);

	TFT_fillRect(0, _height-TFT_getfontheight()-9, _width-1, TFT_getfontheight()+8, _bg);
	TFT_drawRect(0, _height-TFT_getfontheight()-9, _width-1, TFT_getfontheight()+8, TFT_CYAN);

	TFT_print(info, CENTER, 4);
	_dispTime();
    _dispIpAddr();

	_bg = TFT_BLACK;
	TFT_setclipwin(0,TFT_getfontheight()+9, _width-1, _height-TFT_getfontheight()-10);
}

void display_update()
{
	color_t last_fg, last_bg;

	TFT_saveClipWin();
	TFT_resetclipwin();

	Font curr_font = cfont;
	last_bg = _bg;
	last_fg = _fg;
	_fg = TFT_YELLOW;
	_bg = (color_t){ 80, 80, 80 };
    if (_width < 240) TFT_setFont(DEF_SMALL_FONT, NULL);
	else TFT_setFont(DEFAULT_FONT, NULL);

    TFT_fillRect(1, _height-TFT_getfontheight()-8, _width-3, TFT_getfontheight()+6, _bg);
    _dispTime();
    _dispIpAddr();

	cfont = curr_font;
	_fg = last_fg;
	_bg = last_bg;

	TFT_restoreClipWin();
}
