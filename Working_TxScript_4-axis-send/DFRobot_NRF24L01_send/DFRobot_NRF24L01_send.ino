#include <SPI.h>
#include <DFRobot_NRF24L01.h>

DFRobot_NRF24L01 MyNrf;

struct Data {
  byte LeftStick_Left;
  byte LeftStick_Right;
  byte LeftStick_Up;
  byte LeftStick_Down;
  byte RightStick_Left;
  byte RightStick_Right;
  byte RightStick_Up;
  byte RightStick_Down;
  
};

Data data;

void setup(){
  Serial.begin(9600);
  MyNrf.spi = &MirfHardwareSpi;
  MyNrf.NRF24L01_Gpio_Init(9,10);        /*CE = 6, CS = 5*/
  while(!MyNrf.NRF24L01_check()){
    Serial.println("RF24L01 is not find !...");
    delay(1000);
  }
  MyNrf.NRF24L01_Init();
  MyNrf.NRF24L01_Set_Mode(MODE_TX);

}

void loop(){

  data.LeftStick_Left = map(analogRead(A0), 0, 1023, 0, 255);
  data.LeftStick_Right = map(analogRead(A1), 0, 1023, 0, 255);
  data.LeftStick_Up = map(analogRead(A4), 0, 1023, 0, 255);
  data.LeftStick_Down = map(analogRead(A5), 0, 1023, 0, 255);
  data.RightStick_Left = map(analogRead(A2), 0, 1023, 0, 255);
  data.RightStick_Right = map(analogRead(A3), 0, 1023, 0, 255);
  data.RightStick_Up = map(analogRead(A6), 0, 1023, 0, 255);
  data.RightStick_Down = map(analogRead(A7), 0, 1023, 0, 255);

  Serial.print("LeftStick_Left: ");
  Serial.print(data.LeftStick_Left);
  Serial.print("LeftStick_Right: ");
  Serial.print(data.LeftStick_Right);
  Serial.print("LeftStick_Up: ");
  Serial.print(data.LeftStick_Up);
  Serial.print("LeftStick_Down: ");
  Serial.print(data.LeftStick_Down);
  Serial.print("RightStick_Left: ");
  Serial.print(data.RightStick_Left);
  Serial.print("RightStick_Right: ");
  Serial.print(data.RightStick_Right);
  Serial.print("RightStick_Up: ");
  Serial.print(data.RightStick_Up);
  Serial.print("RightStick_Down: ");
  Serial.print(data.RightStick_Down);
  Serial.println();
  
  uint8_t data[32],len = 0;
  while(Serial.available()>0){
    data[len] = char(Serial.read());
    len++;
    if(len == 32)break;     /*Each send can only send up to 32 bytes*/
  }
  if(len>0){
    MyNrf.send(data, len);
    while(MyNrf.isSending()){
      delay(10);
    }
  }else{
    delay(1000);
  }
}
