#include "mbed.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

#define B_TOP 20
#define B_BOTTOM 70
#define B_S1 35
#define B_S2 50
#define B_ON_L 30
#define B_ON_R 110
#define B_ON_S 40
#define B_FM_L 115
#define B_FM_R 195
#define B_FM_S 125
#define B_MP3_L 200
#define B_MP3_R 280
#define B_MP3_S 210
#define B_IN_L 285
#define B_IN_R 365
#define B_IN_S 295
#define B_OUT_L 370
#define B_OUT_R 455
#define B_OUT_S 380
#define B_WIDTH 80
#define B_HEIGHT 50
#define THREADS_COUNT 100

typedef enum state { 
    STATE_RADIO_OFF,
    STATE_RADIO_ON,
    STATE_FMAM,
    STATE_MP3,
    STATE_INCALL,
    STATE_OUTCALL  
} state;

void initLCD();
void handleTouch();
void drawButtons();
void drawState(uint8_t *);
void threadFunction();
void changeState(state newState);
state actualState = STATE_RADIO_OFF;
TS_StateTypeDef TS_State;
Semaphore semaphore(1);
Thread threadsArray[THREADS_COUNT];
int currentThread = 0;

int main()
{
    initLCD();
    drawButtons();
    while(true) {
        handleTouch();
    }
}

void drawState(uint8_t * text) {
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_ClearStringLine(10);
    BSP_LCD_DisplayStringAt(0, LINE(10), text, CENTER_MODE);
}

void drawButtons() {
    BSP_LCD_DrawRect(B_ON_L, B_TOP, B_WIDTH, B_HEIGHT);
    BSP_LCD_DisplayStringAt(B_ON_S, B_S1, (uint8_t *)"ON/OFF", LEFT_MODE);
    BSP_LCD_DrawRect(B_FM_L, B_TOP, B_WIDTH, B_HEIGHT); 
    BSP_LCD_DisplayStringAt(B_FM_S, B_S1, (uint8_t *)"AM/FM", LEFT_MODE);
    BSP_LCD_DrawRect(B_MP3_L, B_TOP, B_WIDTH, B_HEIGHT);
    BSP_LCD_DisplayStringAt(B_MP3_S, B_S1, (uint8_t *)"MP3", LEFT_MODE);
    BSP_LCD_DrawRect(B_IN_L, B_TOP, B_WIDTH, B_HEIGHT);
    BSP_LCD_DisplayStringAt(B_IN_S, B_S1, (uint8_t *)"Prichozi", LEFT_MODE);
    BSP_LCD_DisplayStringAt(B_IN_S, B_S2, (uint8_t *)"hovor", LEFT_MODE);
    BSP_LCD_DrawRect(B_OUT_L, B_TOP, B_WIDTH, B_HEIGHT);
    BSP_LCD_DisplayStringAt(B_OUT_S, B_S1, (uint8_t *)"Odchozi", LEFT_MODE);
    BSP_LCD_DisplayStringAt(B_OUT_S, B_S2, (uint8_t *)"hovor", LEFT_MODE);
    
}

void initLCD() {
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetFont(&Font12);

    uint8_t status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
    if (status != TS_OK) {
        BSP_LCD_Clear(LCD_COLOR_RED);
        BSP_LCD_SetBackColor(LCD_COLOR_RED);
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *)"TOUCHSCREEN INIT FAIL", CENTER_MODE);
    }
}

void threadFunction() {
        semaphore.acquire();
        switch(actualState) {
            case STATE_RADIO_OFF :
                drawState((uint8_t *) "OFF");
                break;
            case STATE_RADIO_ON :
                drawState((uint8_t *) "ON");
                break;
            case STATE_FMAM :
                drawState((uint8_t *) "FM/AM");
                break;
            case STATE_MP3 :
                drawState((uint8_t *) "MP3");
                break;
            case STATE_INCALL :
                drawState((uint8_t *) "INCOMMING CALL");
                break;
            case STATE_OUTCALL :
                drawState((uint8_t *) "OUTCOMMING CALL");
                break;
        }    
}

void changeState(state newState) {
    if(newState != actualState) {
        semaphore.release();
        threadsArray[currentThread].terminate();
        actualState = newState;
        currentThread++;
        threadsArray[currentThread].start(threadFunction);
    }
}

void handleTouch() {
    int  x, y;
    BSP_TS_GetState(&TS_State);
    if (TS_State.touchDetected) {
        x = TS_State.touchX[0];
        y = TS_State.touchY[0];
        if((y > B_TOP) && (y < B_BOTTOM)) {
            if((x > B_ON_L) && (x < B_ON_R)) {
                if (actualState == STATE_RADIO_OFF) {
                    changeState(STATE_RADIO_ON);
                } else {
                    changeState(STATE_RADIO_OFF);
                }
            } else if(actualState != STATE_RADIO_OFF){
                if((x > B_FM_L) && (x < B_FM_R)) {
                    changeState(STATE_FMAM);
                }
                if((x > B_MP3_L) && (x < B_MP3_R)) {
                    changeState(STATE_MP3);
                }
                if((x > B_IN_L) && (x < B_IN_R)) {
                    changeState(STATE_INCALL);
                }
                if((x > B_OUT_L) && (x < B_OUT_R)) {
                    changeState(STATE_OUTCALL);
                }                           
            }
        }
    }
    HAL_Delay(300);
}
