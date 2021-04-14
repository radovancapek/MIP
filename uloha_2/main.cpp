#include "mbed.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

#define DEFAULT_TIME 120 //seconds
#define MIN_TIME 1 //seconds
#define MAX_TIME 3599 //seconds
#define INCREASE_VALUE 5 //seconds
#define DECREASE_VALUE 5 //seconds
#define WATCHDOG 3000 //seconds


void initLCD();
void drawButtons();
void drawTimer();
void drawWires();
void handleTouch();
void decreaseRemainingTime();
void increaseRemainingTime();
void startTimer();
void timer();
void end();
void deactivate();

Thread timerThread;
TS_StateTypeDef TS_State;
volatile static int remainingTime = DEFAULT_TIME;
bool timerRunning = false;
bool stopTimer = false;

int main()
{   
    initLCD();
    while(true) {
        drawButtons();
        drawWires();
        drawTimer();
        handleTouch();
    }
}

void timer() {
    while((remainingTime > 0) && (!stopTimer)) {
        remainingTime = remainingTime - 1;
        Watchdog::get_instance().kick();
        ThisThread::sleep_for(1s);   
    }
    if(remainingTime <= 0) {
        end();
    }
}

void handleTouch() {
    int  x, y;
    BSP_TS_GetState(&TS_State);
    if (TS_State.touchDetected) {
        x = TS_State.touchX[0];
        y = TS_State.touchY[0];
        
        if(timerRunning) {
            if((x > 40) && (x < 60)) {
                end();
            }
            if((x > 420) && (x < 440)) {
                deactivate();
            }
        } else {
            if((y > 200) && (y < 250)) {
                if((x > 105) && (x < 185)) decreaseRemainingTime();
                if((x > 185) && (x < 285)) startTimer();
                if((x > 285) && (x < 365)) increaseRemainingTime();
            }
        }
    }
    HAL_Delay(100);
}

void initLCD() {
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    
    uint8_t status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
    if (status != TS_OK) {
        BSP_LCD_Clear(LCD_COLOR_RED);
        BSP_LCD_SetBackColor(LCD_COLOR_RED);
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *)"TOUCHSCREEN INIT FAIL", CENTER_MODE);
    }
}

void drawButtons() {
    BSP_LCD_DrawRect(105, 200, 80, 50);
    BSP_LCD_DisplayStringAt(138, 215, (uint8_t *)"-", LEFT_MODE);
    BSP_LCD_DrawRect(185, 200, 100, 50);
    BSP_LCD_DisplayStringAt(0, 215, (uint8_t *)"START", CENTER_MODE);
    BSP_LCD_DrawRect(285, 200, 80, 50);
    BSP_LCD_DisplayStringAt(138, 215, (uint8_t *)"+", RIGHT_MODE);
}

void drawWires() {
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DrawRect(50, 0, 1, 271);
    
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    BSP_LCD_DrawRect(430, 0, 1, 271);
    
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
}

void drawTimer() {
    int minutes = remainingTime / 60;
    int seconds = remainingTime - minutes * 60;
    uint8_t displayTime[25];
    sprintf((char*) displayTime, "%02d:%02d", minutes, seconds);
    BSP_LCD_DisplayStringAt(0, 150, (uint8_t *)displayTime, CENTER_MODE);
}

void decreaseRemainingTime() {
    if(remainingTime > (MIN_TIME + DECREASE_VALUE)) remainingTime -= DECREASE_VALUE;
}

void increaseRemainingTime() {
    if(remainingTime < (MAX_TIME - INCREASE_VALUE)) remainingTime += INCREASE_VALUE;
}

void startTimer() {
    Watchdog::get_instance().start(WATCHDOG);
    timerRunning = true;
    timerThread.start(timer);
}

void end() {
    stopTimer = true;
    bool flag = true;
    while(1) {
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(0, 150, (uint8_t *)"BOOOOM!!!", CENTER_MODE);
        if(flag) {
            BSP_LCD_Clear(LCD_COLOR_RED);
            BSP_LCD_SetBackColor(LCD_COLOR_RED);
        } else {
            BSP_LCD_Clear(LCD_COLOR_ORANGE);
            BSP_LCD_SetBackColor(LCD_COLOR_ORANGE);
        }
        flag = !flag;
    }
}

void deactivate() {
    stopTimer = true;
    BSP_LCD_Clear(LCD_COLOR_GREEN);
    BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    while(1) {
        BSP_LCD_DisplayStringAt(0, 150, (uint8_t *)"BOMB DEACTIVATED", CENTER_MODE);
    }
}
