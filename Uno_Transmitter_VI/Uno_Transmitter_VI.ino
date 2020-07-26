
//  Not entirely sure why I need SPI but I bet it's the radio!
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//  This is identical to the other radio is defines a unique ID and channel to match the other radio
const uint64_t my_radio_pipe = 0xE8E8F0F0E1LL;

//  The radio pins
RF24 radio(9, 10);

//  The size of this struct can be up to 32 bytes, which is more than enough
struct Data_to_be_sent {
  byte ch1;
  byte ch2;
  byte ch3;
  byte ch4;
  byte ch5;
  byte ch6;
  byte ch7;
};

//  Create a variable with the structure above and name it sent_data
Data_to_be_sent sent_data;


////////////////////////////////
void setup(){
  
  //  Starting the serial connection so that Serial Monitor can be used for debugging
  Serial.begin(115200);
  //  Starting the radio, setting auto acknowledge to false, data rate to 250KBPS which can be up to 2Mbps
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(my_radio_pipe);

  //  I'm hoping that this is the correct thing to do with buttons, but I'm only applying this logic because of context
  pinMode(A0, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  
  //  Reset each channel value while we're starting the sketch
  sent_data.ch1 = 0;
  sent_data.ch2 = 0;
  sent_data.ch3 = 0;
  sent_data.ch4 = 0;
  sent_data.ch5 = 0;
  sent_data.ch6 = 0;
  sent_data.ch7 = 0;
}

////////////////////////////////
void loop()
{

  //  With the array defined above, we simply loop here mapping our analog input from the potentiometer. These pins are 8 or 10 bit
  //  So that means at one end of the axis we get 5v which read as 1023, and the other 0. We map this to the 8-bit maximum of 255  
  sent_data.ch1 = map( analogRead(A0), 0, 1023, 0, 255);
  sent_data.ch2 = map( analogRead(A1), 0, 1023, 0, 255);
  sent_data.ch3 = map( analogRead(A2), 0, 1023, 0, 255);
  sent_data.ch4 = map( analogRead(A3), 0, 1023, 0, 255);
  sent_data.ch5 = map( analogRead(A4), 0, 1023, 0, 255);
  sent_data.ch6 = map( analogRead(A5), 0, 1023, 0, 255);
  sent_data.ch7 = digitalRead(8);

  //  Here we define what we want the Serial monitor to display
  Serial.println();
  Serial.print(sent_data.ch2,"  ");
  Serial.print(sent_data.ch3);
  Serial.println();
  Serial.print(sent_data.ch5,"  ");
  Serial.print(sent_data.ch6);

  //  This tiny blip of code is "apparently" what actually just grabs all that code up there and says "send"
  radio.write(&sent_data, sizeof(Data_to_be_sent));
}
