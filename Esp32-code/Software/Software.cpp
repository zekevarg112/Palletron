#include "src/include.h"
#include "src/config.h"

#define DEBUG(format, ...) debug_message("Serial parser: Debug: ", format, ##__VA_ARGS__);


#define NUM_LEDS 1
#define LED_PIN GPIO_NUM_4



Adafruit_NeoPixel pixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
const uint32_t led_red = pixel.Color(255*(LED_BRIGHTNESS/255.0), 0, 0);
const uint32_t led_green = pixel.Color(0, 255*(LED_BRIGHTNESS/255.0), 0);
const uint32_t led_yellow = pixel.Color(255*(LED_BRIGHTNESS/255.0), 255*(LED_BRIGHTNESS/255.0), 0);
const uint32_t led_blue = pixel.Color(0, 0, 255*(LED_BRIGHTNESS/255.0));

unsigned long last_serial_msg;

// CAN parameters
const uint8_t vesc_id_right = 62;
const uint8_t vesc_id_left = 63;

CAN_Manager CAN;

Vesc vesc;

AS5600 as5600;   //  use default Wire




TaskHandle_t vesc_status_handle = NULL;
void vesc_status_function(void* param){
    uint8_t task_delay = 1000/VESC_STATUS_FREQUENCY; // time between commands
    DEBUG("Serial thread started\n");
 
    while(1){
        send_vesc_rpm(&datalayer.Motor_R, RIGHT);
        send_vesc_rpm(&datalayer.Motor_L, LEFT);     
        float angle = as5600.readAngle()*2.0*PI/4096 - 1.76;
        send_encoder_angle(angle);
        // printf("Angle: %f\n", angle);

        vTaskDelay(pdMS_TO_TICKS(task_delay)); // wait until next publish, configured to 50 Hz
    }
}




void setup(){
    debug_init();
    CAN.init();
    vesc.init();




    // Magnetic encoder, code from example in library
    Wire.begin(SDA_PIN, SCL_PIN);

    as5600.begin(32);  //  set direction pin. Not used?
    as5600.setDirection(AS5600_CLOCK_WISE);  //  default, just be explicit.
    int b = as5600.isConnected();
    DEBUG("Connect to encoder: \n");
    DEBUG("%d\n", b);
    // uncomment below if actually connected
    Serial.print(as5600.readAngle());
    Serial.print("\t");
    Serial.println(as5600.rawAngle());

    pixel.begin();
    pixel.clear();
    pixel.fill(led_red);
    pixel.show();




    xTaskCreatePinnedToCore(&vesc_status_function, "Status", 1024*4, NULL, 2, &vesc_status_handle, 1); // start serial publish thread 

}



void loop(){

    // check and parse serial input
    serial_parsed_data parsed_data = check_and_parse_serial();
    if(parsed_data.succes){
        pixel.fill(led_green);
        pixel.show();
        last_serial_msg = millis();
        switch (parsed_data.device)
        {
        case 1: //right
            vesc.vesc_handle_command(RIGHT, parsed_data.command, parsed_data.data);
            break;
        case 2: // left
            vesc.vesc_handle_command(LEFT, parsed_data.command, parsed_data.data);
            break;
        case 3: //both

            break;
        
        default:
            break;
        }
    }
    if(last_serial_msg + 10 < millis()){
        pixel.fill(led_red);
        pixel.show();
    }

    
    vTaskDelay(pdMS_TO_TICKS(10));
}