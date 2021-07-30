/*
 * File:   Final Project - SmartWatch
 * Author: Itamar Yarden
 *
 * Created on April 11, 2021, 16:17 AM
 */

#include <stdlib.h>
#include <stdio.h>

#include "System/system.h"
#include "System/delay.h"
#include "oledDriver/oledC.h"
#include "oledDriver/oledC_colors.h"
#include "oledDriver/oledC_shapes.h"

#include "HelperFunctions.c"

#define MAX_WIDTH 95;
#define MAX_HEIGHT 95;

/*
 * Functions declaration!
 */
void DrawAnalogClock();
void DrawDigitalDigits(void);
void ClearDigitalDigits(void);
void DrawAnalogClock();
void DrawAnalogHand(const uint16_t color);
void ClearAnalogHand(const uint16_t color);
void DrawAMPM(void);
void ClearAMPM(void);
void DrawAlarm(void);
void ClearAlarm(void);
void DrawDate(void);
void ClearDate(void);


void oled_init();
void user_init();

void moveTime();

/*
 * System structures
 */
typedef struct Time {
    unsigned char seconds;
    unsigned char minutes;
    unsigned char hours;
    
    unsigned char ampm;

    unsigned char day;
    unsigned char month;
} Time;

typedef struct StateManager {
    unsigned char menu;
    unsigned char clockType;
    unsigned char alarm;
} StateManager;

/*
 ****************
 *    Globals!  *
 *****************
 */

StateManager state;
Time time;
Time prevTime;
int clean;


unsigned char midpoint = 47;
unsigned char analog_clock[60][2] = {{47, 7}, {51, 7}, {55, 8}, {59, 9}, {63, 10}, {67, 12}, {71, 15}, {74, 17}, {77, 20}, {79, 23}, {82, 27}, {84, 31},{85, 35},{86, 39},{87, 43},
                                    {87, 47},{87, 51}, {86, 55}, {85, 59}, {84, 63}, {82, 67}, {79, 71}, {77, 74}, {74, 77}, {71, 79}, {67, 82}, {63, 84}, {59, 85}, {55, 86}, {51, 87},
                                    {47, 87}, {43, 87}, {39, 86}, {35, 85}, {31, 84}, {27, 82}, {23, 79}, {20, 77}, {17, 74}, {15, 71}, {12, 67}, {10, 63}, {9, 59}, {8, 55}, {7, 51},
                                    {7, 47}, {7, 43}, {8, 39}, {9, 35}, {10, 31}, {12, 27}, {15, 23}, {17, 20}, {20, 17}, {23, 15}, {27, 12}, {31, 10}, {35, 9}, {39, 8}, {43, 7}};
unsigned char months[12] =          {31, 28, 30, 30, 31, 30, 31, 30, 31, 30, 31}; 

/*
 * Method : DrawDate;
 * describe: Drawing current Date on screen
 */
void DrawDate(void){

    //clear last date
    ClearDate();
    
    char buff[6];
    NumberToString(time.day, buff);
    buff[2] = '/';
    NumberToString(time.month, buff + 3);

    oledC_DrawString(68, 88 , 1, 1, buff, OLEDC_COLOR_WHITE);
}

/*
 * Method : ClearDate;
 * describe: clearing current date window
 */
void ClearDate(void) {
    //clear Date
    oledC_DrawRectangle(68,88, 83, 95, OLEDC_COLOR_BLACK);
}

/*
 * Method : DrawAlarm;
 * describe: Drawing alarm symbol when choosing alarm
 */
void DrawAlarm(void) {
    oledC_DrawRing(6, 6, 3, 2, OLEDC_COLOR_CRIMSON);
}

/*
 * Method : ClearAlarm;
 * describe: clearing alarm symbol from window
 */
void ClearAlarm(void) {
    oledC_DrawRing(6, 6, 2, 1, OLEDC_COLOR_BLACK);
}


/*
 * Method : DrawAMPM;
 * describe: Drawing AmPm singtuare on the screen
 *           where '0' represent AM and '1' represent PM
 */
void DrawAMPM(void) {
    ClearAMPM();
    uint8_t Am[3] = {'A','M','\0'};
    uint8_t Pm[3] = {'P','M','\0'};
    if(time.ampm == 0){
        
        oledC_DrawString(1, 88 , 1, 1, Am, OLEDC_COLOR_WHITE);
    }
    else if(time.ampm == 1){  
        oledC_DrawString(1, 88 , 1, 1, Pm, OLEDC_COLOR_WHITE);
     }
}
/*
 * Method : ClearAMPM;
 * describe: Drawing AmPm singtuare on the screen
 */
void ClearAMPM(void) {
    //ClearAMPM();
    uint8_t Am[3] = {'A','M','\0'};
    uint8_t Pm[3] = {'P','M','\0'};

    if(time.ampm == 0){
        
        oledC_DrawString(1, 88 , 1, 1, Am, OLEDC_COLOR_BLACK);
    }
    else {  
        oledC_DrawString(1, 88 , 1, 1, Pm, OLEDC_COLOR_BLACK);
    }

}

/*
 * Method : DrawDigitalClock;
 * describe: Drawing digital clock structure
 */
void DrawDigitalClock() {
    
    
    //clear screen
    oledC_clearScreen();
    
    DrawAMPM();
    DrawDate();
    
    char digital_buff[9];

    NumberToString(time.hours, digital_buff);
    digital_buff[2] = ':';
    NumberToString(time.minutes, digital_buff +3);
    digital_buff[5] = ':';
    NumberToString(time.seconds, digital_buff+6);
    oledC_DrawString(5, 40,2,2,digital_buff, OLEDC_COLOR_WHITE);
}

/*
 * Method : DrawDigitalClock
 * describe: Drawing digital clock structure
 */

void DrawDigitalDigits(void){
    
    char digital_buff[9];

    NumberToString(time.hours, digital_buff);
    digital_buff[2] = ':';
    NumberToString(time.minutes, digital_buff +3);
    digital_buff[5] = ':';
    NumberToString(time.seconds, digital_buff+6);
    oledC_DrawString(5, 40, 2 , 2, digital_buff, OLEDC_COLOR_WHITE);
}

/*
 * Method : ClearDigitalClock
 * describe: clear digital clock structure
 */

void ClearDigitalDigits(void){
    

    if (prevTime.hours != time.hours) {
        //calculate hours hand and paint
        
        if(time.hours % 10 == 0 )
            oledC_DrawRectangle(5,40, 27 , 56, OLEDC_COLOR_BLACK);
        else
            oledC_DrawRectangle(16,40, 27 , 56, OLEDC_COLOR_BLACK);
        
        prevTime.hours = time.hours;
    }

    if (prevTime.minutes != time.minutes) {
        //calculate minutes hand and paint
                
        if(time.minutes % 10 == 0 )
            oledC_DrawRectangle(38, 40, 60 , 56, OLEDC_COLOR_BLACK);
        else
            oledC_DrawRectangle(49, 40, 60 , 56, OLEDC_COLOR_BLACK);
        
        prevTime.hours = time.hours;
        
        prevTime.minutes = time.minutes;

    }
    
    //calculate seconds hand and paint
    if(time.seconds % 10 == 0)
        oledC_DrawRectangle(71, 40, 93 , 56, OLEDC_COLOR_BLACK);
    else
        oledC_DrawRectangle(82, 40, 93 , 56, OLEDC_COLOR_BLACK);

    prevTime.seconds = time.seconds;
    
}

/*
 * Method : DrawAnalog;
 * describe: Drawing analog clock structure
 */
void DrawAnalogClock() {
     //clear screen
    oledC_clearScreen();
    
    DrawAMPM();
    DrawDate();

    unsigned char new_x, new_y;
    unsigned char x, y;

    uint8_t i;

    //oledC_DrawCircle(47, 47, 6, OLEDC_COLOR_WHITE);
    
    for (i = 0; i < 60; i++) {

        x = analog_clock[i][0];
        y = analog_clock[i][1];

        if (i % 5 == 0 && i % 15 != 0) {
            new_x = (midpoint + 9 * (x - midpoint) / 10);
            new_y = (midpoint + 9 * (y - midpoint) / 10);
            oledC_DrawLine(new_x, new_y, x, y, 1, OLEDC_COLOR_WHITE);
        }
        if (i % 15 == 0) {
            new_x = (midpoint + (8.5 * (x - midpoint)) / 10);
            new_y = (midpoint + (8.5 * (y - midpoint)) / 10);
            oledC_DrawLine(new_x, new_y, x, y, 2, OLEDC_COLOR_SLATEBLUE);
        }
    }
    
}

/*
 * Method : DrawAnalogHand;
 * describe: Drawing analog clock hands for seconds minutes and hours
 */
void DrawAnalogHand(const uint16_t color) {

    unsigned char x, y;
    unsigned char hr_x, hr_y;
    unsigned char min_x, min_y;
    unsigned char sec_x, sec_y;

    //calculate hours hand and paint
    x = analog_clock[(time.hours * 5) % 60][0];
    y = analog_clock[(time.hours * 5) % 60][1];
    hr_x = (midpoint + 2 * (x - midpoint) / 5);
    hr_y = (midpoint + 2 * (y - midpoint) / 5);
    oledC_DrawLine(midpoint, midpoint, hr_x, hr_y, 2, color);


    //calculate minutes hand and paint
    x = analog_clock[time.minutes][0];
    y = analog_clock[time.minutes][1];
    min_x = (midpoint + 3 * (x - midpoint) / 4);
    min_y = (midpoint + 3 * (y - midpoint) / 4);
    oledC_DrawLine(midpoint, midpoint, min_x, min_y, 2, color);

    //calculate seconded hand and paint
    x = analog_clock[time.seconds][0];
    y = analog_clock[time.seconds][1];
    sec_x = (midpoint + 3 * (x - midpoint) / 4);
    sec_y = (midpoint + 3 * (y - midpoint) / 4);
    oledC_DrawLine(midpoint, midpoint, sec_x, sec_y, 1, color);


}


/*
 * Method : ClearAnalogHand;
 * describe: Clear analog clock hands for seconds minutes and hours
 */
void ClearAnalogHand(const uint16_t color) {

    unsigned char x, y;
    unsigned char hr_x, hr_y;
    unsigned char min_x, min_y;
    unsigned char sec_x, sec_y;

    if (prevTime.hours != time.hours) {
        //calculate hours hand and paint
        x = analog_clock[(prevTime.hours * 5) % 60][0];
        y = analog_clock[(prevTime.hours * 5) % 60][1];
        hr_x = (midpoint + 2 * (x - midpoint) / 5);
        hr_y = (midpoint + 2 * (y - midpoint) / 5);
        oledC_DrawLine(midpoint, midpoint, hr_x, hr_y, 2, color);
        
        prevTime.hours = time.hours;
    }

    if (prevTime.minutes != time.minutes) {
        //calculate hours hand and paint
        x = analog_clock[prevTime.minutes][0];
        y = analog_clock[prevTime.minutes][1];
        min_x = (midpoint + 3 * (x - midpoint) / 4);
        min_y = (midpoint + 3 * (y - midpoint) / 4);
        oledC_DrawLine(midpoint, midpoint, min_x, min_y, 2, color);
        
        prevTime.minutes = time.minutes;

    }
    //calculate hours hand and paint
    x = analog_clock[prevTime.seconds][0];
    y = analog_clock[prevTime.seconds][1];
    sec_x = (midpoint + 3 * (x - midpoint) / 4);
    sec_y = (midpoint + 3 * (y - midpoint) / 4);
    oledC_DrawLine(midpoint, midpoint, sec_x, sec_y, 1, color);
    
    prevTime.seconds = time.seconds;

}

/*
 * Method : Move Time;
 * describe: time control for the system
 */
void moveTime(void) {
    time.seconds++;
    
    if (time.seconds == 60) {
        time.seconds = 0;
        time.minutes++;

        if (time.minutes == 60) {
            time.minutes = 0;
            time.hours++;
            
            if (time.hours == 12) {
                time.hours = 0;
                time.ampm = (++time.ampm) % 2;
                
                if(prevTime.ampm == 1){
                     time.day++;
                    if(time.day > months[time.month -1]){
                        time.day = 01;
                        time.month++;
                    }                         
                }
            }
        } 
    }

}

/*
 *  Main application
 */
int main(void) {

    //Initialize the system
    SYSTEM_Initialize();
    Timer_Initialize();
    oled_init();
    user_init();
    


    
    if (state.menu == 0)
    {
        if(state.clockType == 0){
            //Drawing analog clock
            DrawAnalogClock();
         }
        else if(state.clockType == 1){
            DrawDigitalClock();
        }
    }
    
    while (1) {
        
        int i;
        if (PORTAbits.RA12 == 0) {
            LATAbits.LATA8 = 1;
            
            for (i = 0; i < 100; i++);
            LATAbits.LATA8 = 0;
        }
        if(PORTAbits.RA11 == 0){
            LATAbits.LATA9 = 1;
               for (i = 0; i < 100; i++);
            LATAbits.LATA9 = 0;

        }
    
    }

    return 1;
}

/*
 * System initializers functions 
 */

void Timer_Initialize(void) {
    T1CONbits.TON = 1; /* Start Timer */
    T1CONbits.TCKPS = 0b11; /* Select 1:256 Prescaler */
    PR1 = 16595; /* Count limit */
    IFS0bits.T1IF = 0; /* Flag interrupt */
    IEC0bits.T1IE = 1; /* Enable interrupt */
}

void oled_init() {
    oledC_setBackground(OLEDC_COLOR_BLACK);
    oledC_clearScreen();
}

void user_init(void) {
    //time intialize
    time.hours = 11;
    time.minutes = 59;
    time.seconds = 59;

    time.day = 28;
    time.month = 02;
    
    time.ampm = 1;

    //previous time intialize
    prevTime.hours = 7;
    prevTime.minutes = -1;
    prevTime.seconds = -1;

    prevTime.day = 28;
    prevTime.month = 02;
    
    prevTime.ampm =1;

    //state intialize
    state.clockType = 1;
    state.menu = 0;
    state.alarm = 1;
    
   
    //Intialize led 
    TRISAbits.TRISA8 = 0;
    TRISAbits.TRISA9 = 0;
}

/*
 * Method : Interrupt routine 
 * describe: Every actions happing during interrupt time
 */
void __attribute__((__interrupt__)) _T1Interrupt(void) {

    if(state.alarm == 1){
        DrawAlarm();
    }
    if(state.menu == 0){

        if (state.clockType == 0) {

            //clean the last time on clock
            ClearAnalogHand(OLEDC_COLOR_BLACK);

            //draw next time
            DrawAnalogHand(OLEDC_COLOR_WHITE);
        
            //move time every second
            moveTime();
        }
        if(state.clockType == 1){
            
            
            //Clean The last time on clock()
            ClearDigitalDigits();
            //Draw next time
            DrawDigitalDigits();
            
            //move time every second
            moveTime();
        }
        if(time.ampm != prevTime.ampm){
            DrawAMPM();
            prevTime.ampm = time.ampm;
        }
        if(time.day != prevTime.day){
            DrawDate();
            prevTime.day = time.day;
            prevTime.month = time.month;
        }
    }
    else{
        
    }
   
    IFS0bits.T1IF = 0; //flag interrupt 

}

/**
 End of File
 */