
#define OLED_SPI
#define SHT45
#define ROTARY_ENCODER_A_PIN 48
#define ROTARY_ENCODER_B_PIN 47
#define ROTARY_ENCODER_C_PIN 21
#define ROTARY_ENCODER_BUTTON_PIN 16
#define RGB_LED 37
#define NUM_RGB 4
#define ADC_12V 15
#define STEP_EN 6
#define DIR_PIN 4
#define STEP_PIN 5
#define TP_INT 16
#define BUZZER 38
#define OLED_CLK 10
#define OLED_MOSI 11
#define OLED_CS 14
#define OLED_DC 12
#define OLED_RESET 13
#include <LovyanGFX.hpp>

#include <lvgl.h>

/*Change to your screen resolution*/
static const uint16_t screenWidth = 128;
static const uint16_t screenHeight = 128;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[2][screenWidth * 10];
class LGFX : public lgfx::LGFX_Device
{
    // Prepare an instance that matches the type of panel you want to connect.
    // lgfx::Panel_GC9A01      _panel_instance;
    // lgfx::Panel_SH110x      _panel_instance; // SH1106, SH1107
    // lgfx::Panel_SSD1306     _panel_instance;
    lgfx::Panel_SSD1327 _panel_instance;
    // lgfx::Panel_SSD1331     _panel_instance;
    // lgfx::Panel_SSD1351     _panel_instance; // SSD1351, SSD1357

    lgfx::Bus_SPI _bus_instance; // SPIバスのインスタンス
                                 // lgfx::Bus_I2C        _bus_instance;   // I2Cバスのインスタンス

    // If backlight control is possible, prepare an instance. (Delete if not required)
    lgfx::Light_PWM _light_instance;

    // Prepare an instance that matches the touch screen type. (Delete if not needed)
    // lgfx::Touch_CST816S          _touch_instance;
    lgfx::Touch_FT5x06 _touch_instance; // FT5206, FT5306, FT5406, FT6206, FT6236, FT6336, FT6436
                                        // lgfx::Touch_XPT2046          _touch_instance;

public:
    LGFX(void)
    {
        {                                      // Set up the bus control.
            auto cfg = _bus_instance.config(); // Get the bus configuration structure.
#ifdef OLED_SPI
            // SPIバスの設定
            cfg.spi_host = SPI3_HOST; // 使用するSPIを選択  ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32 : VSPI_HOST or HSPI_HOST
            // ※ Due to the ESP-IDF version upgrade, the VSPI_HOST and HSPI_HOST descriptions are deprecated. If an error occurs, use SPI2_HOST and SPI3_HOST instead.
            cfg.spi_mode = 0;          // SPI通信モードを設定 (0 ~ 3)
            cfg.freq_write = 20000000; // SPI clock during transmission (Maximum 80MHz, rounded to an integer value of 80MHz)
            cfg.freq_read = 16000000;  // 受信時のSPIクロック
            cfg.spi_3wire = true;      // Set to true if reception is done on the MOSI pin
            cfg.use_lock = true;       // Set to true if you want to use transaction locking.
            cfg.dma_channel = 0;       // Set the DMA channel to use (0=DMA not used / 1=1ch / 2=ch / SPI_DMA_CH_AUTO=automatic setting)
                                       // * Due to the ESP-IDF version upgrade, SPI_DMA_CH_AUTO (automatic setting) is now recommended for the DMA channel. Specifying 1ch or 2ch is no longer recommended.
            cfg.pin_sclk = OLED_CLK;   // SPIのSCLKピン番号を設定
            cfg.pin_mosi = OLED_MOSI;  // SPIのMOSIピン番号を設定
            cfg.pin_miso = -1;         // SPIのMISOピン番号を設定 (-1 = disable)
            cfg.pin_dc = OLED_DC;      // SPIのD/Cピン番号を設定  (-1 = disable)

            // SDカードと共通のSPIバスを使う場合、MISOは省略せず必ず設定してください。
//*/
#endif
            /*
            // I2Cバスの設定
                  cfg.i2c_port    = 0;          // 使用するI2Cポートを選択 (0 or 1)
                  cfg.freq_write  = 400000;     // 送信時のクロック
                  cfg.freq_read   = 400000;     // 受信時のクロック
                  cfg.pin_sda     = 8;         // SDAを接続しているピン番号
                  cfg.pin_scl     = 9;         // SCLを接続しているピン番号
                  cfg.i2c_addr    = 0x3C;       // I2Cデバイスのアドレス
            //*/

            _bus_instance.config(cfg);              // 設定値をバスに反映します。
            _panel_instance.setBus(&_bus_instance); // バスをパネルにセットします。
        }

        {                                        // Set up the display panel control.
            auto cfg = _panel_instance.config(); /// Get the structure for display panel settings.

#ifdef OLED_SPI
            cfg.pin_cs = OLED_CS;     // CSが接続されているピン番号   (-1 = disable)
            cfg.pin_rst = OLED_RESET; // RSTが接続されているピン番号  (-1 = disable)
#else
            cfg.pin_cs = -1;  // CSが接続されているピン番号   (-1 = disable)
            cfg.pin_rst = -1; // RSTが接続されているピン番号  (-1 = disable)
#endif
            cfg.pin_busy = -1; // BUSYが接続されているピン番号 (-1 = disable)

            // ※ 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。

            cfg.panel_width = 128;    // 実際に表示可能な幅
            cfg.panel_height = 128;   // 実際に表示可能な高さ
            cfg.offset_x = 0;         // パネルのX方向オフセット量
            cfg.offset_y = 0;         // パネルのY方向オフセット量
            cfg.offset_rotation = 0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
            cfg.dummy_read_pixel = 8; // ピクセル読出し前のダミーリードのビット数
            cfg.dummy_read_bits = 1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
            cfg.readable = true;      // データ読出しが可能な場合 trueに設定
            cfg.invert = false;       // パネルの明暗が反転してしまう場合 trueに設定
            cfg.rgb_order = false;    // パネルの赤と青が入れ替わってしまう場合 trueに設定
            cfg.dlen_16bit = false;   // 16bitパラレルやSPIでデータ長を16bit単位で送信するパネルの場合 trueに設定
            cfg.bus_shared = true;    // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

            // 以下はST7735やILI9163のようにピクセル数が可変のドライバで表示がずれる場合にのみ設定してください。
            //    cfg.memory_width     =   240;  // ドライバICがサポートしている最大の幅
            //    cfg.memory_height    =   320;  // ドライバICがサポートしている最大の高さ

            _panel_instance.config(cfg);
        }

        // //*
        // {                                      // バックライト制御の設定を行います。（必要なければ削除）
        //   auto cfg = _light_instance.config(); // バックライト設定用の構造体を取得します。

        //   cfg.pin_bl = 32;     // バックライトが接続されているピン番号
        //   cfg.invert = false;  // バックライトの輝度を反転させる場合 true
        //   cfg.freq = 44100;    // バックライトのPWM周波数
        //   cfg.pwm_channel = 7; // 使用するPWMのチャンネル番号

        //   _light_instance.config(cfg);
        //   _panel_instance.setLight(&_light_instance); // バックライトをパネルにセットします。
        // }
        //*/

        //*
        { // タッチスクリーン制御の設定を行います。（必要なければ削除）
            auto cfg = _touch_instance.config();

            cfg.x_min = 0;           // タッチスクリーンから得られる最小のX値(生の値)
            cfg.x_max = 128;         // タッチスクリーンから得られる最大のX値(生の値)
            cfg.y_min = 0;           // タッチスクリーンから得られる最小のY値(生の値)
            cfg.y_max = 128;         // タッチスクリーンから得られる最大のY値(生の値)
            cfg.pin_int = TP_INT;    // INTが接続されているピン番号
            cfg.bus_shared = true;   // 画面と共通のバスを使用している場合 trueを設定
            cfg.offset_rotation = 0; // 表示とタッチの向きのが一致しない場合の調整 0~7の値で設定

            // SPI接続の場合
            // cfg.spi_host = VSPI_HOST;// 使用するSPIを選択 (HSPI_HOST or VSPI_HOST)
            // cfg.freq = 1000000;     // SPIクロックを設定
            // cfg.pin_sclk = 18;     // SCLKが接続されているピン番号
            // cfg.pin_mosi = 23;     // MOSIが接続されているピン番号
            // cfg.pin_miso = 19;     // MISOが接続されているピン番号
            // cfg.pin_cs   =  5;     //   CSが接続されているピン番号

            // I2C接続の場合
            cfg.i2c_port = 1;    // Select the I2C to use (0 or 1)
            cfg.i2c_addr = 0x38; // I2Cデバイスアドレス番号
            cfg.pin_sda = 8;     // SDAが接続されているピン番号
            cfg.pin_scl = 9;     // SCLが接続されているピン番号
            cfg.freq = 400000;   // I2Cクロックを設定

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance); // タッチスクリーンをパネルにセットします。
        }
        //*/

        setPanel(&_panel_instance); // 使用するパネルをセットします。
    }
};

LGFX gfx;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    Serial.printf("flush %d %d %d %d\n", area->x1, area->y1, area->x2, area->y2);
    if (gfx.getStartCount() == 0)
    { // Processing if not yet started
        gfx.startWrite();
    }
    for (int y = area->y1; y <= area->y2; y++)
    {
        for (int x = area->x1; x <= area->x2; x++)
        {
            gfx.drawPixel(x, y, color_p->full);
            color_p++;
        }
    }
    gfx.display();
    // gfx.pushImageDMA(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (lgfx::swap565_t *)&color_p->full);

    lv_disp_flush_ready(disp);
}

void sd_access_sample(void)
{
    // if (gfx.getStartCount() > 0)
    // {   // Free the bus before accessing the SD card
    //     gfx.endWrite();
    // }

    // // Something to manipulate the SD card.
    // auto file = SD.open("/file");
    // file.close();
}
static void event_cb(lv_event_t *e)
{
    /*The original target of the event. Can be the buttons or the container*/
    lv_obj_t *target = lv_event_get_target(e);

    /*The current target is always the container as the event is added to it*/
    lv_obj_t *cont = lv_event_get_current_target(e);

    /*If container was clicked do nothing*/
    if (target == cont)
        return;

    /*Make the clicked buttons red*/
    lv_obj_set_style_bg_color(target, lv_palette_main(LV_PALETTE_RED), 0);
}

/*Read the touchpad*/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;

    data->state = LV_INDEV_STATE_REL;

    if (gfx.getTouch(&touchX, &touchY))
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

void setup()
{
    Serial.begin(115200);
    gfx.begin();
    gfx.setColorDepth(16);
    gfx.setRotation(1);
    gfx.invertDisplay(true);

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf[0], buf[1], screenWidth * 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    lv_theme_mono_init(0, false, NULL);
    lv_theme_mono_is_inited();
    /*Initialize the input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello Arduino, I'm LVGL!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    // lv_demo_benchmark();
    gfx.setCursor(20, 64);
    gfx.print("Hello Word");
    gfx.startWrite();
    gfx.display();

    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, 290, 200);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    uint32_t i;
    for (i = 0; i < 30; i++)
    {
        lv_obj_t *btn = lv_btn_create(cont);
        lv_obj_set_size(btn, 80, 50);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "%" LV_PRIu32, i);
        lv_obj_center(label);
    }

    lv_obj_add_event_cb(cont, event_cb, LV_EVENT_CLICKED, NULL);
}
int count = 0;
void loop()
{
    count++;
    lv_task_handler();
    /* let the GUI do its work */
    delay(5);
}
