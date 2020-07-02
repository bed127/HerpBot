#include <SPI.h>
#include <NRFLite.h>

int LeftJoy_Left_Pin =27;
int LeftJoy_Right_Pin =28;
int LeftJoy_Up_Pin =24;
int LeftJoy_Down_Pin =23;

int RightJoy_Left_Pin =19;
int RightJoy_Right_Pin =22;
int RightJoy_Up_Pin =25;
int RightJoy_Down_Pin =26;

float LeftJoy_Left;
float LeftJoy_Right;
float LeftJoy_Up;
float LeftJoy_Down;

float RightJoy_Left;
float RightJoy_Right;
float RightJoy_Up;
float RightJoy_Down;

struct JoyData {
  byte LeftStick_Left;
  byte LeftStick_Right;
  byte LeftStick_Up;
  byte LeftStick_Down;
  byte RightStick_Left;
  byte RightStick_Right;
  byte RightStick_Up;
  byte RightStick_Down;
};

JoyData data;


const static uint8_t RADIO_ID = 1;
const static uint8_t DESTINATION_RADIO_ID = 0;
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

enum RadioPacketType
{
    Heartbeat,
    BeginGetData,
    EndGetData,
    ReceiverData,
    JoyData
};

struct RadioPacket
{
    RadioPacketType PacketType;
    uint8_t FromRadioId;
    uint32_t OnTimeMillis;
    uint32_t JoyData;
    };

NRFLite _radio;
uint32_t _lastHeartbeatSendTime, _lastMessageRequestTime;

void setup()
{
    
    Serial.begin(115200);

    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }
}

void loop(){

    data.LeftStick_Left = map(analogRead(27), 0, 1023, 0, 255);
    data.LeftStick_Right = map(analogRead(28), 0, 1023, 0, 255);
    data.LeftStick_Up = map(analogRead(24), 0, 1023, 0, 255);
    data.LeftStick_Down = map(analogRead(23), 0, 1023, 0, 255);

    data.RightStick_Left = map(analogRead(19), 0, 1023, 0, 255);
    data.RightStick_Right = map(analogRead(22), 0, 1023, 0, 255);
    data.RightStick_Up = map(analogRead(25), 0, 1023, 0, 255);
    data.RightStick_Down = map(analogRead(26), 0, 1023, 0, 255);

    String LeftJoy = "LeftJoystick";
    LeftJoy += data.LeftStick_Left;
    LeftJoy += data.LeftStick_Right;
    LeftJoy += data.LeftStick_Up;
    LeftJoy += data.LeftStick_Down;
    Serial.println(LeftJoy);
    
    String RightJoy = "RightJoystick";
    RightJoy += data.LeftStick_Left;
    RightJoy += data.LeftStick_Right;
    RightJoy += data.LeftStick_Up;
    RightJoy += data.LeftStick_Down;
    Serial.println(RightJoy);    
    
    
    // Send a heartbeat once every second.
    if (millis() - _lastHeartbeatSendTime > 133)
    {
        _lastHeartbeatSendTime = millis();

        Serial.print("Sending heartbeat");
        RadioPacket radioData;
        radioData.PacketType = Heartbeat;
        radioData.FromRadioId = RADIO_ID;
        radioData.OnTimeMillis = _lastHeartbeatSendTime;
        radioData.JoyData = JoyData;

        if (_radio.send(DESTINATION_RADIO_ID, &radioData, sizeof(radioData)))
        {
            Serial.println("...Success");
        }
        else
        {
            Serial.println("...Failed");
        }
    }

    // Request data from the primary receiver once every 4 seconds.
    if (millis() - _lastMessageRequestTime > 133)
    {
        _lastMessageRequestTime = millis();

        Serial.println("Requesting data");
        Serial.println("  Sending BeginGetData");
        RadioPacket radioData;
        radioData.PacketType = BeginGetData; // When the receiver sees this packet type, it will load an ACK data packet.
        _radio.send(DESTINATION_RADIO_ID, &radioData, sizeof(radioData));

        Serial.println("  Sending EndGetData");
        radioData.PacketType = EndGetData; // When the receiver replies to this packet, we will get the ACK data that was loaded.
        _radio.send(DESTINATION_RADIO_ID, &radioData, sizeof(radioData));
        
        while (_radio.hasAckData())
        {
            RadioPacket ackData;
            _radio.readData(&ackData);

            String msg = "  Received data from ";
            msg += ackData.FromRadioId;
            msg += ", ";
            msg += ackData.OnTimeMillis;
            msg += " ms";
            Serial.println(msg);
        }
    }
}
