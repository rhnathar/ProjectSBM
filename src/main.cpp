#include <lvgl.h>
#include <Adafruit_Sensor.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <XPT2046_Touchscreen.h>
#include <DHT.h>

//screen resolution
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

//display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

//for DHT22
#define DHTPIN 23
#define DHTTYPE DHT22

//SPI for touchscreen
SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

//TFT
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight);

//DHT
DHT dht(DHTPIN, DHTTYPE);

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

//Read the touchpad
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
    uint16_t touchX = 0, touchY = 0;

    if( !touchscreen.tirqTouched() && !touchscreen.touched())
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        TS_Point p = touchscreen.getPoint();
        // Calibrate Touchscreen points with map function to the correct width and height
        touchX = map(p.x, 200, 3700, 1, 320);
        touchY = map(p.y, 240, 3800, 1, 240);

        //Set the coordinates
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print( "Data x " );
        Serial.println( touchX );

        Serial.print( "Data y " );
        Serial.println( touchY );
    }
}

//reset temp and hum values
void ui_reset (){
    lv_arc_set_value(ui_ArcTemp, 0);
    lv_obj_clear_flag(ui_ArcTemp, LV_OBJ_FLAG_CLICKABLE); // disable click
    lv_arc_set_value(ui_ArcHum, 0);
    lv_obj_clear_flag(ui_ArcHum, LV_OBJ_FLAG_CLICKABLE); //disable click
    lv_label_set_text(ui_Label3, "0°C");
    lv_label_set_text(ui_Label4, "0%");
}

void updateDHT(){
    float temp = dht.readTemperature(false, false);
    float hum = dht.readHumidity();

    int intTemp = static_cast<int>(round(temp)); 
    int intHum = static_cast<int>(round(hum)); 

    Serial.println("Temp: " + String(temp) + "°C, Hum: " + String(hum) + "%");
    
    if( temp < 100 && hum < 101 ){
        //update label
        lv_label_set_text(ui_Label3, (String(intTemp) + "°C").c_str());
        lv_label_set_text(ui_Label4, (String(intHum) + "%").c_str());
        //update arc
        lv_arc_set_value(ui_ArcTemp, intTemp);
        lv_arc_set_value(ui_ArcHum, intHum);
        //update chart
        update_chart(temp, hum);
    }
}

void DHTTask(void *pvParameters){
    for(;;){
        updateDHT();
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}

void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */
    touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSPI);
    touchscreen.setRotation(4);

    lv_init();

    tft.begin();          /* TFT init */
    tft.setRotation( 3 ); /* Landscape orientation, flipped */

    //display buffer
    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );

    //display setup
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    //Initialize the input device driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );


    ui_init();
    ui_reset();
    xTaskCreatePinnedToCore(
        DHTTask,
        "DHTTask",
        4096,
        NULL,
        0, 
        NULL,
        1
    );

    Serial.println( "Setup done" );
}

void loop()
{
    //let the lvgl timer handler do its thing
    lv_timer_handler(); 
    delay(5);
}

