//include the libraries
#include <dht.h>
#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <Debug.h>
#include "DEV_Config.h"
#include "Waveshare_AS7341.h"
#include <Wire.h>
U8G2_SSD1306_64X32_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 12, /* dc=*/ 9, /* reset=*/ 10);
dht DHT;//create a variable type of dht

const int DHT11_PIN= 7;//Humiture sensor attach to pin7
const int keyPin= 2;
const int ledPin= 3;

byte variable[80];
byte index = 0;
char Temperature[20]; 
char Humidity[20];
char Light[20];  
char Od[20];
unsigned char hexdata[8] = {0x01,0x03,0x00,0x02,0x00,0x02,0x65,0xCB};
double od600 = 0;


void setup()
{
  DEV_ModuleInit();
  Serial.begin(9600);//initialize the serial
  // Serial.print("IIC ready! Now start initializing AS7341!\r\n");
  AS7341_Init(eSpm);
  AS7341_ATIME_config(100);
  AS7341_ASTEP_config(999);
  AS7341_AGAIN_config(6);
  AS7341_EnableLED(true);// LED Enable
  pinMode(keyPin,INPUT);
  pinMode(ledPin,OUTPUT);
}

void loop()
{  

  Serial.write(hexdata, 8);
  // READ DATA
  delay(100);
  while( Serial.available()>0 ){
    variable[index++] = Serial.read();
    Serial.print(variable[index-1]);
  }
  index = 0;
  long lux = (((long)variable[6]))+(((long)variable[5])*256)+ (((long)variable[4])*256*256)+(((long)variable[3])*256*256*256);
  double Lux = (double)lux / 1000.0;
  // Serial.print(Lux);
  // D: int chk = DHT.read11(DHT11_PIN);//read the value returned from sensor
  DHT.read11(DHT11_PIN);
  //启动LED和光谱

  if(digitalRead(keyPin)){
    od600 = 0;
    digitalWrite(ledPin, LOW);
  }else{
    digitalWrite(ledPin, HIGH); 
    AS7341_ControlLed(false,10);//Turn on or off the LED and set the brightness of the LED
    AS7341_startMeasure(eF1F4ClearNIR);
    od600 = AS7341_ReadSpectralDataTwo().channel6;
  }


  // DISPLAY DATA

  dtostrf(DHT.temperature, 5, 2, Temperature);  // 5表示总的字符宽度，2表示小数点后的位数
  dtostrf(DHT.humidity, 5, 2, Humidity);
  dtostrf(Lux, 7, 2, Light);
  dtostrf(od600, 7, 2, Od);

  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  
  u8g2.drawStr(0, 8, "T:");
  u8g2.drawStr(15, 8, Temperature);
  u8g2.drawStr(50, 8, "°C");
  
  u8g2.drawStr(0, 16, "H:");
  u8g2.drawStr(15, 16, Humidity);
  u8g2.drawStr(50, 16, "%");

  u8g2.drawStr(0, 24, "L:");
  u8g2.drawStr(10, 24, Light);
  u8g2.drawStr(50, 24, "Lux");
  
  u8g2.drawStr(0, 32, "O:");
  u8g2.drawStr(10, 32, Od);
 
  u8g2.sendBuffer();
  
  delay(2000); //wait a while 
}
