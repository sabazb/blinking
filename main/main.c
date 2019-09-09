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
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#define BLINK_GPIO CONFIG_BLINK_GPIO
const static char *TAG = "message";

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
 uint16_t tobinary(int dec){
    uint16_t output;
    if(dec < 2){
        if(dec == 1){
            output = 10;
            return output;
        }
        else{
            output = 01;
            return output;
        }
    }
    output = tobinary(dec/2) * 10 + dec % 2;
    return output;
} 
uint64_t encode(uint8_t *message, uint16_t message_length){
    int j , c , k = 0;
    uint64_t buffer_data = 0 ;
    buffer_data = (buffer_data << 16) | 0b1001011010010110;
    //ESP_LOGI(TAG, "preamble is =", buffer_data);
    buffer_data = (buffer_data << 16) | tobinary(message_length);
    //ESP_LOGI(TAG, "alan buffer ine is =%llu", buffer_data);

    for(j = 0 ; j < message_length ; j++){
        c = message[j];
        ESP_LOGI(TAG, "c is =%d", c);
        for(k= 7; k+1> 0; k--){
            if(c>= (1<<k)){
                c= c- (1<<k);
                buffer_data = (buffer_data << 2) | 0b10 ;
                }

                else{
                    buffer_data = (buffer_data << 2) | 0b01 ; 
                }
        }
    }
    return buffer_data;
}


uint64_t buffer;

void blink_task(void *pvParameter){

    gpio_pad_select_gpio(CONFIG_BLINK_GPIO);
    gpio_set_direction(CONFIG_BLINK_GPIO, GPIO_MODE_OUTPUT);
    

//buffer[0] = 0b10010110;
//buffer[1] = 0b10010110;

//buffer[2] = 0b01011001;
    //buffer[3] = 0b01010101;
    
   // buffer[4] = 0b10010101;
    //buffer[5] = 0b01100101;
    //buffer[6] = 0b10010110;
    //buffer[7] = 0b01100110;
   

    uint64_t period = 2000;
    int i = 0 ;
    while( true ){
        buffer = encode("hi" , 2 );
        ESP_LOGI(TAG ,"buffer = %llu" , buffer );
        for(int b = 0 ; b < 8 ; b++ ){
            uint64_t time = (uint64_t)esp_timer_get_time();
            int state = (buffer >> b ) & 1 ;
            if(state == 0){
                gpio_set_level(CONFIG_BLINK_GPIO, 0);
                uint64_t now = (uint64_t)esp_timer_get_time();
                uint64_t new_time = time + period;
                uint64_t sleep = (new_time - now) > 0 ? (new_time - now) : 0;
                delay(sleep);
            }
            else{

                gpio_set_level(CONFIG_BLINK_GPIO , 1);
                uint64_t now = (uint64_t)esp_timer_get_time();
                uint64_t new_time = time + period;
                uint64_t sleep = (new_time - now) > 0 ? (new_time - now) : 0;
                delay(sleep);
            }

        }
        i = ( i + 1 ) % 8;
    }
 
 }

void app_main(){
    esp_task_wdt_delete(xTaskGetIdleTaskHandleForCPU(0));
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
    TIMERG0.wdt_feed=1;
    TIMERG0.wdt_wprotect=0;
    blink_task(NULL);
}