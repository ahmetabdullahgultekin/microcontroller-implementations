// Original code author: Aleksander Alekseev
// https://github.com/afiskon/stm32-ssd1306
// Modified by Yifeng Zhu

#include <string.h>
#include <stdio.h>
#include "ssd1306.h"
#include "ssd1306_tests.h"

void delay(uint32_t d){
	int i, j;
	volatile uint32_t k = 0;
	for(i=0; i<d; i++){
		for(j=0; j<1000000;j++){
			k += 1;
		}
	}
}

//void ssd1306_TestBorder() {
//    uint32_t start, end;
//    uint8_t x = 0;
//    uint8_t y = 0;
//	
//		ssd1306_Fill(Black);
//	
//		start = get_tick();
//		end = start;
//    do {
//        ssd1306_DrawPixel(x, y, Black);

//        if((y == 0) && (x < 127))
//            x++;
//        else if((x == 127) && (y < 63))
//            y++;
//        else if((y == 63) && (x > 0)) 
//            x--;
//        else
//            y--;

//        ssd1306_DrawPixel(x, y, White);
//        ssd1306_UpdateScreen();
//    
//        delay(5);
//        end = get_tick();
//    } while((end - start) < 8000);
//   
//    delay(1000);
//}

void ssd1306_TestFonts() {
    ssd1306_Fill(Black);
    ssd1306_SetCursor(2, 0);
    ssd1306_WriteString("Font 16x26", Font_16x26, White);
    ssd1306_SetCursor(2, 26);
    ssd1306_WriteString("Font 11x18", Font_11x18, White);
    ssd1306_SetCursor(2, 26+18);
    ssd1306_WriteString("Font 7x10", Font_7x10, White);
    ssd1306_SetCursor(2, 26+18+10);
    ssd1306_WriteString("Font 6x8", Font_6x8, White);
    ssd1306_UpdateScreen();
}

//void ssd1306_TestFPS() {

//    uint32_t start, end;
//		char ch;
//    int fps = 0;
//		char buff[64];
//    char message[] = "ABCDEFGHIJK";
//   
//		ssd1306_Fill(White);
//		
//    start = get_tick();
//		end = start;
//	
//    ssd1306_SetCursor(2,0);
//    ssd1306_WriteString("Testing...", Font_11x18, Black);
//   
//    do {
//        ssd1306_SetCursor(2, 18);
//        ssd1306_WriteString(message, Font_11x18, Black);
//        ssd1306_UpdateScreen();
//       
//        ch = message[0];
//        memmove(message, message+1, sizeof(message)-2);
//        message[sizeof(message)-2] = ch;

//        fps++;
//        end = get_tick();
//    } while((end - start) < 5000);
//   
//    delay(1000);

//    fps = (float)fps / ((end - start) / 1000.0);
//    snprintf(buff, sizeof(buff), "~%d FPS", fps);
//   
//    ssd1306_Fill(White);
//    ssd1306_SetCursor(2, 18);
//    ssd1306_WriteString(buff, Font_11x18, Black);
//    ssd1306_UpdateScreen();
//}

void ssd1306_TestLine() {

  ssd1306_Line(1,1,SSD1306_WIDTH - 1,SSD1306_HEIGHT - 1,White);
  ssd1306_Line(SSD1306_WIDTH - 1,1,1,SSD1306_HEIGHT - 1,White);
  ssd1306_UpdateScreen();
  return;
}

void ssd1306_TestRectangle() {
  uint32_t delta;

  for(delta = 0; delta < 5; delta ++) {
    ssd1306_DrawRectangle(1 + (5*delta),1 + (5*delta) ,SSD1306_WIDTH-1 - (5*delta),SSD1306_HEIGHT-1 - (5*delta),White);
  }
  ssd1306_UpdateScreen();
  return;
}

void ssd1306_TestCircle() {
  uint32_t delta;

  for(delta = 0; delta < 5; delta ++) {
    ssd1306_DrawCircle(20* delta+30, 30, 10, White);
  }
  ssd1306_UpdateScreen();
  return;
}

void ssd1306_TestArc() {

  ssd1306_DrawArc(30, 30, 30, 20, 270, White);
  ssd1306_UpdateScreen();
  return;
}

void ssd1306_TestPolyline() {
  SSD1306_VERTEX loc_vertex[] =
  {
      {35,40},
      {40,20},
      {45,28},
      {50,10},
      {45,16},
      {50,10},
      {53,16}
  };

  ssd1306_Polyline(loc_vertex,sizeof(loc_vertex)/sizeof(loc_vertex[0]),White);
  ssd1306_UpdateScreen();
  return;
}

void ssd1306_TestAll() {
    ssd1306_Init();
    //ssd1306_TestFPS();
    delay(3000);
    //ssd1306_TestBorder();
    ssd1306_TestFonts();
    delay(3000);
    ssd1306_Fill(Black);
    ssd1306_TestRectangle();
    ssd1306_TestLine();
    delay(3000);
    ssd1306_Fill(Black);
    ssd1306_TestPolyline();
    delay(3000);
    ssd1306_Fill(Black);
    ssd1306_TestArc();
    delay(3000);
    ssd1306_Fill(Black);
    ssd1306_TestCircle();
    delay(3000);
		
}

