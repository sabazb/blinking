#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "soc/wdev_reg.h"
#include "xtensa/core-macros.h"
#include <stdint.h>
#include <reent.h>
#include <sys/times.h>
#include <sys/time.h>
#include "esp_system.h"
#include "esp_timer.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

const static char *TAG = "sleep";
#define BLINK_GPIO CONFIG_BLINK_GPIO

void delay(uint32_t delay)
{

    uint64_t start = (uint64_t)esp_timer_get_time();
    uint64_t end = delay + start ;
    while((uint64_t)esp_timer_get_time() < end);
}
uint32_t getRandomDelay(){
    uint32_t random = esp_random();
    return 50 + random / 9544371 ;
}
uint8_t buffer[4];
//uint8_t length = 0;
 void blink_task(void *pvParameter){
     gpio_pad_select_gpio(CONFIG_BLINK_GPIO);
     gpio_set_direction(CONFIG_BLINK_GPIO, GPIO_MODE_OUTPUT);
    buffer[0] = 0b01001010;
     buffer[1] = 0b10010101;
     buffer[2] = 0b11010101;
     buffer[3] = 0b11110101;
    uint64_t period = 1000; 
     //length = sizeof(buffer);
     int i = 0 ;
   // for(int i = 0 ; i < 4 ; i++){
       while( true ){
        for(int b = 0 ; b < 8 ; b++ ){
            uint64_t time = (uint64_t)esp_timer_get_time();
           int state = (buffer[i] >> b ) & 1 ;
           if(state == 0){
               gpio_set_level(CONFIG_BLINK_GPIO , 0);
               uint64_t now = (uint64_t)esp_timer_get_time();
               uint64_t new_time = time + period;
               uint64_t sleep = (new_time - now) > 0 ? (new_time - now) : 0;
               //ESP_LOGI(TAG, "Off and sleep_duration = %llu" , sleep);
               //ESP_LOGI(TAG , "hello off \n");
               delay(sleep);

                //delay(500000);
           }
           else{
               gpio_set_level(CONFIG_BLINK_GPIO , 1);
               uint64_t now = (uint64_t)esp_timer_get_time();
               uint64_t new_time = time + period;
               uint64_t sleep = (new_time - now) > 0 ? (new_time - now) : 0;
               //ESP_LOGI(TAG , "hello on\n");
               //ESP_LOGI(TAG, "On and sleep_duration = %llu" , sleep);
               delay(sleep);
                
                //delay(500000);
           }

        }
        i = ( i + 1 ) % 4;
    }
 }

void app_main(){
  
    //xTaskCreate(&blink_task , "blink_task" , 512 , NULL , 5 , NULL);
    blink_task(NULL);
}