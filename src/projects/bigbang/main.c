/*************************************************************************
*                             岱默科技DEMOK Kinetis开发小组
*                                  SonyCCD图像采集实验
*
*  实验说明：通过Kinetis的DMA和EXTI驱动SonyCCD摄像头
*
*  实验操作：Kinetis与SonyCCD接线方式：
*                       Kinetis            SonyCCD
*                        PTE7               PCLK
*                        PTD10              HREF
*                        PTE0               VSYN
*                        GND                GND
*             Kinetis与串口模块接线方式：
*                   PTC3接RX    PTC4接TX   默认串口1 波特率115200
*             分辨率输出320*240
*
*  说    明：数据的发送和采集并不同步进行，采集完一帧图像停止采集，将其发
*            送，发送完毕后才继续采集
*
*  修    改：聂晓艺
*************************************************************************/

#include "common.h"
#include "include.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"

u8 ImageBuf[ROW][COL];

volatile u32 rowCnt = 0 ;
volatile u8 SampleFlag = 0;


/*-----------------使用串口猎人接受一幅图像数据的主程序-------------*/

void main()
{
  //char s[20];
  char ch=0;
  char s[10]="%u";
  char wave[]={129,130,132,133,135,137,138,140,141,143,145,146,148,149,151,153,154,156,157,159,160,162,163,165,167,168,170,171,173,174,176,177,179,180,182,183,185,186,187,189,190,192,193,194,196,197,199,200,201,202,204,205,206,208,209,210,211,213,214,215,216,217,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,237,238,239,240,241,241,242,243,244,244,245,246,246,247,248,248,249,249,250,250,251,251,252,252,253,253,253,254,254,254,255,255,255,255,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,255,255,255,255,254,254,254,254,253,253,252,252,252,251,251,250,250,249,249,248,247,247,246,246,245,244,244,243,242,241,241,240,239,238,237,236,236,235,234,233,232,231,230,229,228,227,226,225,224,223,222,220,219,218,217,216,215,214,212,211,210,209,207,206,205,203,202,201,200,198,197,195,194,193,191,190,188,187,186,184,183,181,180,178,177,175,174,172,171,169,168,166,165,163,162,160,158,157,155,154,152,151,149,147,146,144,143,141,139,138,136,135,133,131,130,128,126,125,123,122,120,118,117,115,114,112,110,109,107,106,104,103,101,99,98,96,95,93,92,90,89,87,85,84,82,81,79,78,77,75,74,72,71,69,68,66,65,64,62,61,60,58,57,55,54,53,52,50,49,48,47,45,44,43,42,41,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,18,17,16,15,15,14,13,12,12,11,10,10,9,9,8,8,7,7,6,6,5,5,4,4,4,3,3,3,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,3,3,3,4,4,4,5,5,6,6,6,7,7,8,9,9,10,10,11,12,12,13,14,14,15,16,17,17,18,19,20,21,22,23,23,24,25,26,27,28,29,30,31,32,33,35,36,37,38,39,40,41,43,44,45,46,47,49,50,51,53,54,55,56,58,59,61,62,63,65,66,67,69,70,72,73,75,76,78,79,81,82,84,85,87,88,90,91,93,94,96,97,99,101,102,104,105,107,108,110,112,113,115,116,118,120,121,123,124,126,128,129,131,133,134,136,137,139,141,142,144,145,147,149,150,152,153,155,156,158,160,161,163,164,166,167,169,170,172,173,175,176,178,179,181,182,184,185,187,188,190,191,192,194,195,196,198,199,201,202,203,204,206,207,208,210,211,212,213,214,216,217,218,219,220,221,222,224,225,226,227,228,229,230,231,232,233,234,234,235,236,237,238,239,240,240,241,242,243,243,244,245,245,246,247,247,248,248,249,250,250,251,251,251,252,252,253,253,253,254,254,254,255,255,255,255,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,255,255,255,255,254,254,254,253,253,253,252,252,251,251,250,250,249,249,248,248,247,247,246,245,245,244,243,242,242,241,240,239,239,238,237,236,235,234,233,232,231,230,229,228,227,226,225,224,223,222,221,220,219,218,216,215,214,213,212,210,209,208,207,205,204,203,202,200,199,197,196,195,193,192,191,189,188,186,185,183,182,180,179,178,176,175,173,172,170,168,167,165,164,162,161,159,158,156,154,153,151,150,148,147,145,143,142,140,139,137,135,134,132,131,129,127,126,124,122,121,119,118,116,114,113,111,110,108,106,105,103,102,100,99,97,95,94,92,91,89,88,86,85,83,82,80,79,77,76,74,73,71,70,69,67,66,64,63,62,60,59,57,56,55,54,52,51,50,48,47,46,45,43,42,41,40,39,38,37,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,21,20,19,18,17,16,16,15,14,13,13,12,11,11,10,10,9,8,8,7,7,6,6,5,5,5,4,4,3,3,3,3,2,2,2,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,3,3,3,4,4,4,5,5,6,6,7,7,8,8,9,9,10,11,11,12,13,13,14,15,16,16,17,18,19,20,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,40,41,42,43,44,46,47,48,49,51,52,53,55,56,57,58,60,61,63,64,65,67,68,70,71,72,74,75,77,78,80,81,83,84,86,87,89,90,92,94,95,97,98,100,101,103,104,106,108,109,111,112,114,116,117,119,120,122,124,125,127,128};
  int i= 1000;
  //uint16_t AD;
  Light_init;
  
  
  uart_init(UART0,115200);
  LCD_init();
  LCD_write_english_string(0,0,"hello");
  while(1)
  {
    ch = uart_getchar(UART0);
    Light1_on;
    i= 1000;
    while(i)
    {
      //uart_putchar(UART0,wave[1000-(i--)]);
      printf("%d ",wave[i--]);
    }
    Light1_off;
  }
}
