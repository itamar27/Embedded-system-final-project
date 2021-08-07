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
 * System structures
 */
typedef struct Time {
    unsigned char seconds;
    unsigned char minutes;
    unsigned char hours;

    unsigned char ampm; // 0 for am , 1 for pm

    unsigned char day;
    unsigned char month;
} Time;

typedef struct StateManager {
    unsigned char menu; //indicates if the menu is active
    unsigned char clockType; // specifies which clock is selected, 1 for digital, 0 for analog.
    unsigned char alarm; // tells if the alarm clock is on
    unsigned char clockSet; //  specifies which time presentation is selected, 0 for 12H , 1 or 24H

    unsigned char s1_click_time; // counting how long s1 is being pressed
    unsigned char s2_click_flag; // controlling s2 button clicks
} StateManager;

typedef struct Menu {
    unsigned char selection; // what is the current selection in menu -  where the cursor is at
    unsigned char subMenu; // tells if subMenu is selected

} Menu;



/*
 * Functions declaration!
 */
void DrawMenu(void);
void DrawMenuStatus(void);
void ClearMenuStatus(void);
void DrawDisplayModeMenu(void);
void DrawTimeModeMenu(void);
void DrawSetClockMenu(void);
void ClearSetClockStatus(void);
void DrawSetClockStatus(void);

void DrawSetDateMenu(void);
void ClearSetDateStatus(void);
void DrawSetDateStatus(void);

void DrawSelectedClock(void);

void DrawAnalogClock();
void DrawDigitalStatus(void);
void ClearDigitalStatus(void);

void DrawAnalogClock();
void DrawAnalogStatus(const uint16_t color);
void ClearAnalogStatus(const uint16_t color);

void DrawAMPM(void);
void ClearAMPM(void);

void DrawAlarm(void);
void ClearAlarm(void);

void DrawDate(void);
void ClearDate(void);

void Timer_Initialize(void);
void MoveTime(void);
void UpdateTimePick(Time* dest, Time* src);

void oled_init(void);
void user_init(void);
void restartSelection(void);




/*
 ****************
 *    Globals!  *
 *****************
 */

StateManager state;
Time time;
Time prevTime;
Time timePicker;

Menu mainMenu;
Menu prevMainMenu;

Menu subMenu;
Menu prevSubMenu;
int clean;
int menuRounder;

unsigned char midpoint = 47;
unsigned char analog_clock[60][2] = {
    {47, 7},
    {51, 7},
    {55, 8},
    {59, 9},
    {63, 10},
    {67, 12},
    {71, 15},
    {74, 17},
    {77, 20},
    {79, 23},
    {82, 27},
    {84, 31},
    {85, 35},
    {86, 39},
    {87, 43},
    {87, 47},
    {87, 51},
    {86, 55},
    {85, 59},
    {84, 63},
    {82, 67},
    {79, 71},
    {77, 74},
    {74, 77},
    {71, 79},
    {67, 82},
    {63, 84},
    {59, 85},
    {55, 86},
    {51, 87},
    {47, 87},
    {43, 87},
    {39, 86},
    {35, 85},
    {31, 84},
    {27, 82},
    {23, 79},
    {20, 77},
    {17, 74},
    {15, 71},
    {12, 67},
    {10, 63},
    {9, 59},
    {8, 55},
    {7, 51},
    {7, 47},
    {7, 43},
    {8, 39},
    {9, 35},
    {10, 31},
    {12, 27},
    {15, 23},
    {17, 20},
    {20, 17},
    {23, 15},
    {27, 12},
    {31, 10},
    {35, 9},
    {39, 8},
    {43, 7}
};
unsigned char months[12] = {31, 28, 30, 30, 31, 30, 31, 30, 31, 30, 31};

/*
 * Method : DrawDate;
 * describe: Drawing current Date on screen
 */
void DrawDate(void) {

    //clear last date
    ClearDate();

    char buff[6];
    NumberToString(time.day, buff);
    buff[2] = '/';
    NumberToString(time.month, buff + 3);

    oledC_DrawString(66, 88, 1, 1, buff, OLEDC_COLOR_WHITE);
}

/*
 * Method : ClearDate;
 * describe: clearing current date window
 */
void ClearDate(void) {
    //clear Date
    oledC_DrawRectangle(66, 88, 83, 95, OLEDC_COLOR_BLACK);
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
    uint8_t Am[3] = {'A', 'M', '\0'};
    uint8_t Pm[3] = {'P', 'M', '\0'};
    if (time.ampm == 0) {

        oledC_DrawString(1, 88, 1, 1, Am, OLEDC_COLOR_WHITE);
    } else if (time.ampm == 1) {
        oledC_DrawString(1, 88, 1, 1, Pm, OLEDC_COLOR_WHITE);
    }
}

/*
 * Method : ClearAMPM;
 * describe: Drawing AmPm singtuare on the screen
 */
void ClearAMPM(void) {
    //ClearAMPM();
    uint8_t Am[3] = {'A', 'M', '\0'};
    uint8_t Pm[3] = {'P', 'M', '\0'};

    if (time.ampm == 0) {

        oledC_DrawString(1, 88, 1, 1, Am, OLEDC_COLOR_BLACK);
    } else {
        oledC_DrawString(1, 88, 1, 1, Pm, OLEDC_COLOR_BLACK);
    }
}

/*
 * Method : DrawDigitalClock;
 * describe: Drawing digital clock structure
 */
void DrawDigitalClock() {

    //clear screen
    oledC_clearScreen();

    if (state.alarm == 1)
        DrawAlarm();


    if (state.clockSet == 0)
        DrawAMPM();

    DrawDate();

    DrawDigitalStatus();
}

/*
 * Method : DrawDigitalClock
 * describe: Drawing digital clock structure
 */

void DrawDigitalStatus(void) {

    char digital_buff[9];

    if (state.clockSet == 1 && time.ampm == 1) {
        NumberToString(time.hours + 12, digital_buff);
    } else if (state.clockSet == 0 || time.ampm == 0) {
        NumberToString(time.hours, digital_buff);
    }
    digital_buff[2] = ':';
    NumberToString(time.minutes, digital_buff + 3);
    digital_buff[5] = ':';
    NumberToString(time.seconds, digital_buff + 6);
    oledC_DrawString(5, 40, 2, 2, digital_buff, OLEDC_COLOR_WHITE);
}

/*
 * Method : ClearDigitalClock
 * describe: clear digital clock structure
 */

void ClearDigitalStatus(void) {

    if (prevTime.hours != time.hours) {
        //calculate hours hand and paint

        if (time.hours % 10 == 0)
            oledC_DrawRectangle(5, 40, 27, 56, OLEDC_COLOR_BLACK);
        else
            oledC_DrawRectangle(16, 40, 27, 56, OLEDC_COLOR_BLACK);

        prevTime.hours = time.hours;
    }

    if (prevTime.minutes != time.minutes) {
        //calculate minutes hand and paint

        if (time.minutes % 10 == 0)
            oledC_DrawRectangle(38, 40, 60, 56, OLEDC_COLOR_BLACK);
        else
            oledC_DrawRectangle(49, 40, 60, 56, OLEDC_COLOR_BLACK);

        prevTime.hours = time.hours;

        prevTime.minutes = time.minutes;
    }

    //calculate seconds hand and paint
    if (time.seconds % 10 == 0)
        oledC_DrawRectangle(71, 40, 93, 56, OLEDC_COLOR_BLACK);
    else
        oledC_DrawRectangle(82, 40, 93, 56, OLEDC_COLOR_BLACK);

    prevTime.seconds = time.seconds;
}

/*
 * Method : DrawAnalog;
 * describe: Drawing analog clock structure
 */
void DrawAnalogClock() {
    //clear screen
    oledC_clearScreen();

    if (state.alarm == 1)
        DrawAlarm();

    DrawAMPM();
    DrawDate();

    unsigned char new_x, new_y;
    unsigned char x, y;

    uint8_t i;

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
 * Method : DrawAnalogStatus;
 * describe: Drawing analog clock hands for seconds minutes and hours
 */
void DrawAnalogStatus(const uint16_t color) {

    unsigned char x, y;
    unsigned char hr_x, hr_y;
    unsigned char min_x, min_y;
    unsigned char sec_x, sec_y;

    //calculate hours hand and paint
    x = analog_clock[(time.hours * 5) % 60][0];
    y = analog_clock[(time.hours * 5) % 60][1];
    hr_x = (midpoint + 2 * (x - midpoint) / 5);
    hr_y = (midpoint + 2 * (y - midpoint) / 5);
    oledC_DrawLine(midpoint, midpoint, hr_x, hr_y, 3, color);

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
 * Method : ClearAnalogStatus;
 * describe: Clear analog clock hands for seconds minutes and hours
 */
void ClearAnalogStatus(const uint16_t color) {

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
        oledC_DrawLine(midpoint, midpoint, hr_x, hr_y, 3, color);

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
 * Method : DrawMenu;
 * describe: Display all menu inputs on screen
 */

void DrawMenu(void) {
    //clear screen
    oledC_clearScreen();


    //Draw upper bar of menu on screen
    if (state.alarm == 1) {
        DrawAlarm();
    }

    DrawMenuStatus();

    oledC_DrawString(15, 20, 1, 1, "Display Mode", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 30, 1, 1, "12H/24H Set", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 40, 1, 1, "Set Time", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 50, 1, 1, "Set Date", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 60, 1, 1, "Set Alarm", OLEDC_COLOR_WHITE);

    //draw first menu selection
    oledC_DrawString(8, (mainMenu.selection + 2) * 10, 1, 1, ">", OLEDC_COLOR_WHITE);

}

/*
 * Method : DrawMenuStatus;
 * describe: Display all menu inputs on screen
 */

void DrawMenuStatus(void) {
    //Draw digital clock on screen
    char digital_buff[9];

    if (time.ampm == 1)
        NumberToString(time.hours + 12, digital_buff);
    else if (time.ampm == 0)
        NumberToString(time.hours, digital_buff);
    digital_buff[2] = ':';
    NumberToString(time.minutes, digital_buff + 3);
    digital_buff[5] = ':';
    NumberToString(time.seconds, digital_buff + 6);
    oledC_DrawString(48, 6, 1, 1, digital_buff, OLEDC_COLOR_WHITE);

    //Draw menu selection
    if (mainMenu.subMenu == 1)
        oledC_DrawString(8, (subMenu.selection + 2) * 10, 1, 1, ">", OLEDC_COLOR_WHITE);
    else
        oledC_DrawString(8, (mainMenu.selection + 2) * 10, 1, 1, ">", OLEDC_COLOR_WHITE);
}

/*
 * Method : ClearMenuStatus;
 * describe: Clear all menu display from screen 
 */
void ClearMenuStatus(void) {

    if (prevTime.hours != time.hours) {
        //calculate hours hand and paint

        if (time.hours % 10 == 0)
            oledC_DrawRectangle(48, 6, 60, 14, OLEDC_COLOR_BLACK);
        else
            oledC_DrawRectangle(56, 6, 60, 14, OLEDC_COLOR_BLACK);

        prevTime.hours = time.hours;
    }

    if (prevTime.minutes != time.minutes) {
        //calculate minutes hand and paint

        if (time.minutes % 10 == 0)
            oledC_DrawRectangle(66, 6, 78, 14, OLEDC_COLOR_BLACK);
        else
            oledC_DrawRectangle(72, 6, 78, 14, OLEDC_COLOR_BLACK);

        prevTime.hours = time.hours;

        prevTime.minutes = time.minutes;
    }

    //calculate seconds hand and paint
    if (time.seconds % 10 == 0)
        oledC_DrawRectangle(84, 6, 96, 14, OLEDC_COLOR_BLACK);
    else
        oledC_DrawRectangle(90, 6, 96, 14, OLEDC_COLOR_BLACK);

    prevTime.seconds = time.seconds;


    //clear main menu pointer 
    if (mainMenu.selection != prevMainMenu.selection) {
        oledC_DrawString(8, (prevMainMenu.selection + 2) * 10, 1, 1, ">", OLEDC_COLOR_BLACK);
        prevMainMenu.selection = mainMenu.selection;
    }//clear sub menu pointer 
    else if ((subMenu.selection != prevSubMenu.selection) && mainMenu.subMenu == 1) {
        oledC_DrawString(8, (prevSubMenu.selection + 2) * 10, 1, 1, ">", OLEDC_COLOR_BLACK);
        prevSubMenu.selection = subMenu.selection;
    }
}

/*
 * Method : DrawDisplayModeMenu;
 * describe: Draw the display menu for analog or   
 */

void DrawDisplayModeMenu(void) {

    //clear the menu
    oledC_DrawRectangle(0, 20, 95, 95, OLEDC_COLOR_BLACK);

    //Draw new menu selection;
    oledC_DrawString(15, 20, 1, 1, "Analog", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 30, 1, 1, "Digital", OLEDC_COLOR_WHITE);
}

/*
 * Method : DrawTimeModeMenu;
 * describe: Draw the menu for 12H/24H time presentation
 */

void DrawTimeModeMenu(void) {

    //clear the menu
    oledC_DrawRectangle(0, 20, 95, 95, OLEDC_COLOR_BLACK);

    //Draw new menu selection;
    oledC_DrawString(15, 20, 1, 1, "12H set", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 30, 1, 1, "24H set", OLEDC_COLOR_WHITE);
}

/*
 * Method : DrawSetclockMenu;
 * describe: Draw the setting clock menu
 */

void DrawSetClockMenu(void) {

    //clear the menu
    oledC_DrawRectangle(0, 20, 95, 95, OLEDC_COLOR_BLACK);

    //Draw new menu selection;
    oledC_DrawString(15, 20, 1, 1, "Hour ", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 30, 1, 1, "Minute ", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 40, 1, 1, "Seconds ", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 50, 1, 1, "AM.PM ", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 60, 1, 1, "OK ", OLEDC_COLOR_WHITE);

    DrawSetClockStatus();
}

/*
 * Method : DrawSetclockStatus;
 * describe: Draw the new setting clock menu status
 */

void DrawSetClockStatus(void) {

    char buff[3];

    //Draw new menu selection;
    NumberToString(timePicker.hours, buff);
    oledC_DrawString(65, 20, 1, 1, buff, OLEDC_COLOR_WHITE);
    NumberToString(timePicker.minutes, buff);
    oledC_DrawString(65, 30, 1, 1, buff, OLEDC_COLOR_WHITE);
    NumberToString(timePicker.seconds, buff);
    oledC_DrawString(65, 40, 1, 1, buff, OLEDC_COLOR_WHITE);
    NumberToString(timePicker.ampm, buff);
    oledC_DrawString(65, 50, 1, 1, buff, OLEDC_COLOR_WHITE);



}

/*
 * Method : ClearSetclockStatus;
 * describe: Clear the last setting clock menu status
 */

void ClearSetClockStatus(void) {

    //clear the menu
    oledC_DrawRectangle(65, 20, 77, 29, OLEDC_COLOR_BLACK);
    oledC_DrawRectangle(65, 30, 77, 39, OLEDC_COLOR_BLACK);
    oledC_DrawRectangle(65, 40, 77, 49, OLEDC_COLOR_BLACK);
    oledC_DrawRectangle(65, 50, 77, 59, OLEDC_COLOR_BLACK);

}

/*
 * Method : DrawSetDateMenu;
 * describe: Draw the setting date menu
 */

void DrawSetDateMenu(void) {

    //clear the menu
    oledC_DrawRectangle(0, 20, 95, 95, OLEDC_COLOR_BLACK);

    //Draw new menu selection;
    oledC_DrawString(15, 20, 1, 1, "Month ", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 30, 1, 1, "Day ", OLEDC_COLOR_WHITE);
    oledC_DrawString(15, 40, 1, 1, "OK ", OLEDC_COLOR_WHITE);

    DrawSetDateStatus();
}

/*
 * Method : DrawSetDateStatus;
 * describe: Draw the new setting date menu status
 */

void DrawSetDateStatus(void) {

    char buff[3];

    //Draw new menu selection;
    NumberToString(timePicker.month + 1, buff);
    oledC_DrawString(65, 20, 1, 1, buff, OLEDC_COLOR_WHITE);
    NumberToString(timePicker.day, buff);
    oledC_DrawString(65, 30, 1, 1, buff, OLEDC_COLOR_WHITE);
}

/*
 * Method : ClearSetDateStatus;
 * describe:  clear the last setting date menu status
 */

void ClearSetDateStatus(void) {

    //clear the menu
    oledC_DrawRectangle(65, 20, 77, 29, OLEDC_COLOR_BLACK);
    oledC_DrawRectangle(65, 30, 77, 39, OLEDC_COLOR_BLACK);

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

    if (state.menu == 0) {
        DrawSelectedClock();
    }

    while (1) {

        int i;

        if (PORTAbits.RA11 == 0) {
            LATAbits.LATA8 = 1;
            for (i = 0; i < 100; i++);
            LATAbits.LATA8 = 0;

            if (state.menu == 1 && mainMenu.selection == prevMainMenu.selection) {
                if (mainMenu.subMenu == 1)
                    subMenu.selection = (subMenu.selection + 1) % menuRounder;
                else
                    mainMenu.selection = (mainMenu.selection + 1) % menuRounder;
            }
        }

        if (PORTAbits.RA12 == 0) {
            LATAbits.LATA9 = 1;
            for (i = 0; i < 100; i++);
            LATAbits.LATA9 = 0;

            if (state.menu == 1) {
                mainMenu.subMenu = 1;

                switch (mainMenu.selection) {
                    case 0:
                        if (subMenu.subMenu == 0) {
                            subMenu.selection = 0;
                            subMenu.subMenu = 1;
                            menuRounder = 2;
                            DrawDisplayModeMenu();
                        } else if (subMenu.subMenu == 1) {

                            state.clockType = subMenu.selection;
                            restartSelection();
                            DrawSelectedClock();
                        }
                        break;
                    case 1:
                        if (subMenu.subMenu == 0) {
                            subMenu.selection = 0;
                            subMenu.subMenu = 1;
                            menuRounder = 2;
                            DrawTimeModeMenu();
                        } else if (subMenu.subMenu == 1) {

                            state.clockSet = subMenu.selection;
                            restartSelection();
                            DrawSelectedClock();

                        }
                        break;
                    case 2:
                        if (subMenu.subMenu == 0) {
                            subMenu.selection = 0;
                            subMenu.subMenu = 1;
                            menuRounder = 5;
                            UpdateTimePick(&timePicker, &time);
                            DrawSetClockMenu();
                        } else if (subMenu.subMenu == 1) {

                            if (subMenu.selection == 4) {
                                restartSelection();
                                UpdateTimePick(&time, &timePicker);
                                DrawSelectedClock();
                            } else {
                                if (state.s2_click_flag == 0) {
                                    switch (subMenu.selection) {
                                        case 0:
                                            timePicker.hours = (++timePicker.hours) % 12;
                                            break;
                                        case 1:
                                            timePicker.minutes = (++timePicker.minutes) % 60;
                                            break;
                                        case 2:
                                            timePicker.seconds = (++timePicker.seconds) % 60;
                                            break;
                                        case 3:
                                            timePicker.ampm = (++timePicker.ampm) % 2;
                                            break;
                                    }
                                    state.s2_click_flag = 1;
                                }
                                ClearSetClockStatus();
                                DrawSetClockStatus();
                            }
                        }
                        break;
                    case 3:
                        if (subMenu.subMenu == 0) {
                            subMenu.selection = 0;
                            subMenu.subMenu = 1;
                            menuRounder = 5;
                            UpdateTimePick(&timePicker, &time);
                            DrawSetClockMenu();
                        } else if (subMenu.subMenu == 1) {

                            if (subMenu.selection == 4) {
                                restartSelection();
                                state.alarm = 1;
                                UpdateTimePick(&time, &timePicker);
                                DrawSelectedClock();
                            } else {
                                switch (subMenu.selection) {
                                    case 0:
                                        timePicker.hours = (++timePicker.hours) % 12;
                                        break;
                                    case 1:
                                        timePicker.minutes = (++timePicker.minutes) % 60;
                                        break;
                                    case 2:
                                        timePicker.seconds = (++timePicker.seconds) % 60;
                                        break;
                                    case 3:
                                        timePicker.ampm = (++timePicker.ampm) % 2;
                                        break;
                                }
                                ClearSetClockStatus();
                                DrawSetClockStatus();
                            }
                        }
                        break;
                    case 4:
                        if (subMenu.subMenu == 0) {
                            subMenu.selection = 0;
                            subMenu.subMenu = 1;
                            menuRounder = 3;
                            UpdateTimePick(&timePicker, &time);
                            DrawSetDateMenu();
                        } else if (subMenu.subMenu == 1) {

                            if (subMenu.selection == 2) {
                                restartSelection();
                                UpdateTimePick(&time, &timePicker);
                                DrawSelectedClock();
                            } else {
                                switch (subMenu.selection) {
                                    case 0:
                                        timePicker.month = (++timePicker.month) % 12;
                                        break;
                                    case 1:
                                        timePicker.day = (++timePicker.day) % months[timePicker.month];
                                        break;
                                }
                                ClearSetDateStatus();
                                DrawSetDateStatus();
                            }
                        }
                        break;
                }
            }
        }



        if (state.s1_click_time == 2) {
            state.s1_click_time = 0;
            restartSelection();

            if (state.menu == 1) {
                DrawMenu();
            } else if (state.menu == 0) {
                if (state.clockType == 0) {
                    //Drawing analog clock
                    DrawAnalogClock();
                } else if (state.clockType == 1) {
                    //Drawing digital clock
                    DrawDigitalClock();
                }
            }
        }
    }

    return 1;
}

/*
 * Method : DrawClock;
 * describe: Draw the clock according to state
 */

void DrawSelectedClock() {
    if (state.clockType == 0) {
        //Drawing analog clock
        DrawAnalogClock();
    } else if (state.clockType == 1) {
        DrawDigitalClock();
    }
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
    time.hours = 10;
    time.minutes = 59;
    time.seconds = 55;

    time.day = 14;
    time.month = 04;

    time.ampm = 1;

    //time intialize
    UpdateTimePick(&timePicker, &time);

    //previous time intialize
    prevTime.hours = 7;
    prevTime.minutes = -1;
    prevTime.seconds = -1;

    prevTime.day = 28;
    prevTime.month = 02;

    prevTime.ampm = 1;

    //state intialize
    state.clockType = 1;
    state.menu = 0;
    state.alarm = 1;
    state.clockSet = 1;
    state.s1_click_time = 0;
    state.s2_click_flag = 0;

    //menu selection intialize
    mainMenu.selection = 0;
    mainMenu.subMenu = 0;
    prevMainMenu.selection = 0;
    subMenu.selection = 0;
    prevSubMenu.selection = 0;


    //Intialize led
    TRISAbits.TRISA8 = 0;
    TRISAbits.TRISA9 = 0;

    //globals init
    menuRounder = 5; //init the menu to max of main menu options
}

/*
 * Method : restartSelection;
 * describe: time control for the system
 */

void restartSelection(void) {
    state.menu = ++state.menu % 2;
    state.s1_click_time = 0;
    mainMenu.subMenu = 0;
    subMenu.subMenu = 0;
    subMenu.selection = 0;
    mainMenu.selection = 0;

    menuRounder = 5;
}

/*
 * Method : UpdateTimePick;
 * describe: updating two time objects 
 */

void UpdateTimePick(Time* dest, Time * src) {

    dest->ampm = src->ampm;
    dest->month = src->month;
    dest->day = src->day;
    dest->hours = src->hours;
    dest->minutes = src->minutes;
    dest->seconds = src->seconds;
};

/*
 * Method : Move Time;
 * describe: time control for the system
 */
void MoveTime(void) {
    time.seconds++;

    if (time.seconds == 60) {
        time.seconds = 0;
        time.minutes++;

        if (time.minutes == 60) {
            time.minutes = 0;
            time.hours++;

            if (time.hours == 12) {
                time.hours = 0;
                time.ampm = ++time.ampm % 2;

                if (prevTime.ampm == 1) {
                    time.day++;
                    if (time.day > months[time.month - 1]) {
                        time.day = 01;
                        time.month++;
                    }
                }
            }
        }
    }
}

/*
 * Method : Interrupt routine 
 * describe: Every actions happing during interrupt time
 */
void __attribute__((__interrupt__)) _T1Interrupt(void) {

    if (PORTAbits.RA11 == 0) {
        state.s1_click_time++;
    } else {
        state.s1_click_time = 0;
    }

    if (state.menu == 0) {
        if (state.clockType == 0) {
            //clean the last time on clock
            ClearAnalogStatus(OLEDC_COLOR_BLACK);

            //draw next time
            DrawAnalogStatus(OLEDC_COLOR_WHITE);
        }
        if (state.clockType == 1) {

            //Clean The last time on clock()
            ClearDigitalStatus();
            //Draw next time
            DrawDigitalStatus();
        }
        if (time.ampm != prevTime.ampm) {
            if (state.clockSet == 0)
                DrawAMPM();
            prevTime.ampm = time.ampm;
        }
        if (time.day != prevTime.day) {
            if (state.menu == 0)
                DrawDate();
            prevTime.day = time.day;
            prevTime.month = time.month;
        }
    } else if (state.menu == 1) {
        //Clean the menu previous status
        ClearMenuStatus();
        //Draw the menu new status
        DrawMenuStatus();
    }

    //s2 control
    state.s2_click_flag = 0;
    //move time every second
    MoveTime();
    //flag interrupt
    IFS0bits.T1IF = 0;
}

/**
 End of File
 */