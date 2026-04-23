/*
 * File:   main.c
 * Author: nmudd
 *
 * Created on April 14, 2026, 1:16 PM
 */
//#define F_CPU 16000000UL
//#include <util/delay.h>
#include "IMU.h"
#include <math.h>

#define width 240
#define height 320
#define PI 3.14

//External Assembly commands
void pinInit(void);
void softReset(void);
void sleepOut(void);
void screenInit(void);
void drawPixel(void);
void drawBlock(void);
void drawBackground(void);

//C commands
void paintBackground(char,char,char);
void paintPixel(int,int);
void paintBlock(int,int,int,int,char,char,char);
void RGBConversion(char,char,char);
void colorWhite(void);
void colorBlack(void);
void function(float,int);
void clampScale(int16_t,int16_t,int16_t);

unsigned char xh;
unsigned char xl;

unsigned char xh2;
unsigned char xl2;

unsigned char yh;
unsigned char yl;

unsigned char yh2;
unsigned char yl2;

unsigned char colorh;
unsigned char colorl;

float yaw = 0;
int pitch = 0;
int roll = 0;

float pyaw = 0;
int ppitch = 0;
int proll = 0;

void main(void) {
    imu_startup();
    //imu_zero();
    /*offset.x_axis = 0;
    offset.y_axis = 0;
    offset.z_axis = 0*/
    imu_probe();
    
    pinInit();
    _delay_ms(500);
    softReset();
    _delay_ms(500);
    sleepOut();
    _delay_ms(500);
    screenInit();
    paintBackground(255,255,255);
    
    //int yint = -280;
    //int slope = 1;
    //colorBlack();
    while(1){
        //mower demo
        /*colorWhite();
        paintPixel(originx,originy);
        paintPixel(originx,originy+1);
        paintPixel(originx,originy+2);
        paintPixel(originx,originy+3);
        paintPixel(originx,originy+4);
        paintPixel(originx,originy+5);
        paintPixel(originx,originy+6);
        paintPixel(originx,originy+7);
        paintPixel(originx,originy+8);
        paintPixel(originx,originy+9);
        paintBlock(originx+1,originy,0,0,255,0,0);
        addcoord();*/
        
        /*function(slope,yint);
        yint+=5;
        if(yint>160+(width/2)*slope){
            slope++;
            yint=(160+(width/2)*slope)*-1;
        }*/
        //_delay_ms(50);
        imu_probe();
        clampScale(imu_data.x_axis,imu_data.y_axis,imu_data.z_axis);
        paintBlock(proll+(width/2),ppitch+(height/2),0,0,255,255,255);
        function(tan(pyaw+PI/2),0);
        paintBlock(roll+(width/2),pitch+(height/2),0,0,0,0,0);
        function(tan(yaw+PI/2),0);
        proll=roll;
        ppitch=pitch;
        pyaw=yaw;
        //paintBackground(255,255,255);
    }
    return;
}

void function(float slope,int intercept){
    int x;
    int y;
    x=width/2*-1;
    while(x<width/2){
        y = x*slope+intercept;
        if(height/2-y>=0&&height/2-y<height){
            paintPixel(x+width/2,height/2-y);
        }
        x++;
    }
}

void paintBackground(char red, char green, char blue){
    //RGBConversion(red,green,blue);
    red=(red*31)/255;
    green=(green*63)/255;
    blue=(blue*31)/255;
    colorh = red*8 + green/8;
    colorl = green*32 + blue;
    drawBackground();
}

void paintPixel(int x, int y){
    xh = x/256;
    xl = x;
    
    yh = y/256;
    yl = y;
    
    //RGBConversion(red,green,blue);
    drawPixel();
}

void paintBlock(int x, int y, int x2, int y2, char red, char green, char blue){
    xh = x/256;
    xl = x;
    
    yh = y/256;
    yl = y;
    
    xh2 = x/256;
    xl2 = x;
    
    yh2 = y/256;
    yl2 = y;
    
    RGBConversion(red,green,blue);
    drawBlock();
}

void RGBConversion(char red,char green ,char blue){
    red=(red*31)/255;
    green=(green*63)/255;
    blue=(blue*31)/255;
    colorh = red*8 + green/8;
    colorl = green*32 + blue;
}

void colorBlack(){
    colorh=0;
    colorl=0;
}

void colorWhite(){
    colorh=255;
    colorl=255;
}

void clampScale(int16_t gyrox,int16_t gyroy,int16_t gyroz){
    yaw = (gyrox) * (PI/180);
    pitch = (gyroy)*(height/90.0);
    roll = (gyroz)*(width/180.0);
}