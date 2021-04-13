#include "mbed.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

#define THREADS_COUNT 4

void initLCD();
void handleTouch();
void draw(int, int);
void threadFun(int * threadId);

Semaphore semaphore(1);
Thread threadsArray[THREADS_COUNT];
TS_StateTypeDef TS_State;
int timers[THREADS_COUNT];
bool touch = false;

int main()
{
    int i = 0;
    initLCD();
    for(i = 0; i < THREADS_COUNT; i++) {
        timers[i] = 0;
        threadsArray[i].start(callback(threadFun, (int *) i));
    }
    while(true) {
        handleTouch();
    }
}

void threadFun(int * threadId) {
    int id = (uint32_t) threadId;
    while(true){
        semaphore.acquire();
        while(true) {
            draw(id, timers[id]);
            ThisThread::sleep_for(1s);
            if(timers[id] < 99)
                timers[id]++;
            else
                timers[id] = 0;
            if(touch) {
                semaphore.release();
                touch = false;
                break;
            }
        }
    }
}

void handleTouch() {
    BSP_TS_GetState(&TS_State);
    if (TS_State.touchDetected) {
        touch = true;
    }
    HAL_Delay(100);
}

void draw(int threadNum, int time) {
    uint8_t threadStr[20];
    sprintf((char*) threadStr, "Thread %d - %02d",(uint32_t) threadNum , (uint32_t) time);
    BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *)threadStr, CENTER_MODE);
}

void initLCD() {
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    
    uint8_t status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
    if (status != TS_OK) {
        BSP_LCD_Clear(LCD_COLOR_RED);
        BSP_LCD_SetBackColor(LCD_COLOR_RED);
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *)"TOUCHSCREEN INIT FAIL", CENTER_MODE);
    }
}
