#include <stdlib.h>
#include <stdio.h>
#include "../oledDriver/oledC_shapes.h";

/*
 Globals
 */
uint8_t analog_clock[60][2] = {{77,47},{77,50},{76,53},{76,56},{74,59},{73,62},{71,65},{69,67},{67,69},{65,71},
                               {62,73},{59,74},{56,76},{53,76},{50,77},{47,77},{44,77},{41,76},{38,76},{35,74},
                               {32,73},{29,71},{27,69},{25,67},{23,65},{21,62},{20,59},{18,56},{18,53},{17,50},
                               {17,47},{},{},{},{},{},{},{},{},{},
                               {},{},{},{},{},{},{},{},{},{},
                               {},{},{},{},{},{},{},{},{},{}};

unsigned char mid_point = 47;
/*
 * Method : DrawAnalog;
 * describe: 
 */
void DrawAnalog()
{
  
 
    for(uint8_t i = 0; i < 7; i++){
  
        oledC_DrawLine(x, y, uint8_t end_x, uint8_t end_y, uint8_t width, uint16_t color);
        oledC
    
        
    }
}