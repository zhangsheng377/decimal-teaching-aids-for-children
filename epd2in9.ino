/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>

const int a_pin = 32;
const int b_pin = 33;
const int key_pin = 35;

UBYTE *BlackImage;
const uint8_t scale = 4;
// const uint16_t partial_height = EPD_2IN9_V2_HEIGHT / 3;
// uint16_t ge_Xstart = partial_height * 2.5 - Font24.Width * scale / 2;
const uint16_t ge_Xstart = 100;
// uint16_t Ystart = (EPD_2IN9_V2_WIDTH / 2) - (Font24.Height * scale / 2);
const uint16_t Ystart = 23;
const uint16_t shi_Xstart = 75;
const uint16_t bai_Xstart = 50;

int32_t num = 0;
int32_t old_num_ge = 0;
int32_t old_num_shi = 0;
int32_t old_num_bai = 0;

/* Entry point ----------------------------------------------------------------*/
void setup() {
  pinMode(a_pin, INPUT);
  pinMode(b_pin, INPUT);
  pinMode(key_pin, INPUT);

  printf("EPD_2IN9_V2_test Demo\r\n");
  DEV_Module_Init();

  printf("e-Paper Init and Clear...\r\n");
  EPD_2IN9_V2_Init();
  EPD_2IN9_V2_Clear();
  // DEV_Delay_ms(500);

  UWORD Imagesize = ((EPD_2IN9_V2_WIDTH % 4 == 0) ? (EPD_2IN9_V2_WIDTH / 4) : (EPD_2IN9_V2_WIDTH / 4 + 1)) * EPD_2IN9_V2_HEIGHT;
  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    printf("Failed to apply for black memory...\r\n");
    while (1)
      ;
  }
  printf("Paint_NewImage\r\n");
  Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 90, WHITE);
  Paint_SetScale(scale);
  Paint_Clear(WHITE);

  partial_draw_num(0, ge_Xstart, Ystart);
  partial_draw_num(0, shi_Xstart, Ystart);
  partial_draw_num(0, bai_Xstart, Ystart);
  EPD_2IN9_V2_Display_Partial(BlackImage);
}

int getEncoderTurn() {
  //return -1,0,1
  static int old_a = LOW;
  static int old_b = LOW;
  int result = 0;
  int new_a = digitalRead(a_pin);
  int new_b = digitalRead(b_pin);

  if (new_a == old_a && new_b == old_b) {
    return 0;
  }
  if (old_a == LOW && new_a == HIGH) {
    result = -(old_b * 2 - 1);
  }
  old_a = new_a;
  old_b = new_b;
  delay(2);
  return -result;
}

void partial_draw_num(int32_t num, uint16_t Xstart, uint16_t Ystart) {
  Paint_ClearWindows(Xstart, Ystart, Xstart + Font24.Width, Ystart + Font24.Height, WHITE);
  Paint_DrawChar(Xstart, Ystart, num + '0', &Font24, BLACK, WHITE);
}

void display_num(int32_t num) {
  bool is_change = false;
  int32_t num_ge = num % 10;
  if (num_ge != old_num_ge) {
    old_num_ge = num_ge;
    is_change = true;
    partial_draw_num(num_ge, ge_Xstart, Ystart);
  }
  int32_t num_shi = num / 10 % 10;
  if (num_shi != old_num_shi) {
    old_num_shi = num_shi;
    is_change = true;
    partial_draw_num(num_shi, shi_Xstart, Ystart);
  }
  int32_t num_bai = num / 100;
  if (num_bai != old_num_bai) {
    old_num_bai = num_bai;
    is_change = true;
    partial_draw_num(num_bai, bai_Xstart, Ystart);
  }

  if (is_change) {
    EPD_2IN9_V2_Display_Partial(BlackImage);
  }
}

/* The main loop -------------------------------------------------------------*/
void loop() {
  static int encoder_result = 0;

  int key = digitalRead(key_pin);
  if (key == 0) {
    num = 0;
    display_num(num);
  } else {
    encoder_result = getEncoderTurn();
    if (encoder_result != 0) {
      printf("encoder_result : %d\r\n", encoder_result);
      num = num + encoder_result;
      if (num < 0) {
        num = 0;
      }
      if (num > 999) {
        num = 999;
      }

      display_num(num);
    }
  }
}
