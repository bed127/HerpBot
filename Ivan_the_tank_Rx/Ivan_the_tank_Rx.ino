#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>  //To create PWM signals we need this lybrary


// Defining the inputs on the Motor control shield, and assigning them a Digital pin. 
#define enA 2
#define in1 3
#define in2 4
#define in3 5
#define in4 6
#define enB 7

// This is engaging the radio and setting the ID and the tranmission pipeline. Must match the Transmitter
const uint64_t pipeIn = 0xE8E8F0F0E1LL;

//CSN and CE pins for the nRF24
RF24 radio(9, 10);


// The sizeof this struct should not exceed 32 bytes

struct Received_data {
  byte ch1;
  byte ch2;
  byte ch3;
  byte ch4;
  byte ch5;
  byte ch6;
  byte ch7;
};

// Creating and linking a data object from the radio for the code to refer to
Received_data received_data;

// creating the variables for the left and right throttle that we will later bind to received data
int leftthrottle, righttrhrottle;

// creating variables to store the incoming channel data from the transmitter with a null value
int ch1_value = 0;
int ch2_value = 0;
int ch3_value = 0;
int ch4_value = 0;
int ch5_value = 0;
int ch6_value = 0;
int ch7_value = 0;

// When the loop intiates it checks to see if there is new input and if there is not if refers here to reset the values in case of signal loss
void reset_the_Data() 
{
  // 'safe' values to use when NO radio input is detected

  // ch1 - left thumbstick button
  received_data.ch1 = LOW;
  // ch2 - left stick X
  received_data.ch2 = 127;
  // ch3 - left stick Y
  received_data.ch3 = 127;
  // ch4 - right thumbstick button
  received_data.ch4 = LOW;
  // ch5 - right stick X
  received_data.ch5 = 127;
  // ch6 - right stick Y
  received_data.ch6 = 127;
  // ch7 - null
  received_data.ch7 = 0;
}



/**************************************************/

void setup(){

  //starting the serial connection for debugging, setting a baud rate of 115200, and the value is arbitrary
  Serial.begin(115200);

  // using the variables defined in the beggining, we use pinMode to declare the pins attached to them as 'output' pins
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);
  
  //We reset the received values at the start of the sketch
  reset_the_Data();

  //  Begin and radio configuration, there are further options like opening another pipe for 2 way communication
  radio.begin();
  radio.setAutoAck(false);
  //  Regarding the 'set data rate' there are options going as high as 2Mbps. Pros and cons unknown
  radio.setDataRate(RF24_250KBPS);  
  radio.openReadingPipe(1,pipeIn);
  
  //  We start the radio comunication and tell the radio to listen
  radio.startListening();

}

// starting a timer variable
unsigned long lastRecvTime = 0;

//  We create the function that will read the data each certain time
void receive_the_data()
{
  while ( radio.available() ) {
  //  Here we are referencing the 'received data' array and alligning the variables along the structure defined in 'Received_data' with uppercase R
  radio.read(&received_data, sizeof(Received_data));
  //  Here we receive the data
  lastRecvTime = millis(); 
}
}

void loop()
{
  //  Receive the radio data
  receive_the_data();

  //  This small if will reset the data if signal is lost for 1 sec.
  unsigned long now = millis();
  if ( now - lastRecvTime > 1000 ) {
    //  signal lost?
    reset_the_Data();
    //  Go up and change the initial values to match centre for calibration, read using the Serial monitor
   } 

  //  In the transmission sketch and the receiver sketch the byte array is identical and contain a list of bytes up to a maximum of 32
  //  Here is the loop we are grabbing the received_data and mapping it and assiging it one channel per line
  //  With the four values, we are defining the incoming low, high, and what our container low and high is
  //  Having issues here as the values are all 'int' and potentially behaving poorly
  ch1_value = map(received_data.ch1, 0, 255, 0, 255);
  ch2_value = map(received_data.ch2, 0, 255, 0, 255);
  ch3_value = map(received_data.ch3, 0, 255, 0, 255);
  ch4_value = map(received_data.ch4, 0, 255, 0, 255);
  ch5_value = map(received_data.ch5, 0, 255, 0, 255);
  ch6_value = map(received_data.ch6, 0, 255, 0, 255);
  ch7_value = map(received_data.ch7, 0, 255, 0, 255);

  //  This section here is where I define what output I need to look at for debugging 
  Serial.println();
  Serial.print(ch2_value,"  ");
  Serial.print(ch3_value);
  Serial.println();
  Serial.print(ch5_value,"  ");
  Serial.print(ch6_value);
  Serial.println();
  Serial.print(ch1_value, ch4_value);

  //  Taking the ch_value that we mapped and binding it to the throttle variables created in the beggining
  ch3_value = leftthrottle;
  ch5_value = righttrhrottle;

  //  Here is where I attempt to create the logic that will controlled by the received data chunks, using the throttles to move two motos in two directions
  if (leftthrottle > 130){
    
  }

  

  }
  
  
//Loop end
