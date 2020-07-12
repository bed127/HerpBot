
#include <SPI.h>
#include <NRFLite.h>

// https://www.circuitxcode.com/using-arduino-struct/ Example of structs for arduino

int LeftJoy_Left_Pin =27;
int LeftJoy_Right_Pin =28;
int LeftJoy_Up_Pin =24;
int LeftJoy_Down_Pin =23;

int RightJoy_Left_Pin =19;
int RightJoy_Right_Pin =22;
int RightJoy_Up_Pin =25;
int RightJoy_Down_Pin =26;

const static uint8_t RADIO_ID = 1;
const static uint8_t DESTINATION_RADIO_ID = 0;
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

enum RadioPacketType
{
    AcknowledgementData, // Produced by the primary receiver and provided to the transmitter via an acknowledgement data packet.
    Heartbeat,    // Sent by the primary transmitter.
    BeginGetData, // Sent by the primary transmitter to tell the receiver it should load itself with an acknowledgement data packet.
    EndGetData,    // Sent by the primary transmitter to receive the acknowledgement data packet from the receiver.
};

struct RadioPacket
{
    RadioPacketType PacketType;
    uint8_t FromRadioId;
    uint32_t OnTimeMillis;
};

typedef struct {
  byte LeftStick_Left;
  byte LeftStick_Right;
  byte LeftStick_Up;
  byte LeftStick_Down;
  byte RightStick_Left;
  byte RightStick_Right;
  byte RightStick_Up;
  byte RightStick_Down;
} JoyData;

NRFLite _radio;
uint32_t _lastHeartbeatSendTime, _lastDataRequestTime;

void setup()
{
    Serial.begin(115200);

    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }
}

void loop()
{
    // Send a heartbeat once every second.
    if (millis() - _lastHeartbeatSendTime > 999)
    {
        _lastHeartbeatSendTime = millis();
        sendHeartbeat();
    }

    // Request data from the primary receiver once every 4 seconds.
    if (millis() - _lastDataRequestTime > 3999)
    {
        _lastDataRequestTime = millis();
        requestData();        
    }
    
    int LeftStick_Left = map(analogRead(27), 0, 1023, 0, 255);
    int LeftStick_Right = map(analogRead(28), 0, 1023, 0, 255);
    int LeftStick_Up = map(analogRead(24), 0, 1023, 0, 255);
    int LeftStick_Down = map(analogRead(23), 0, 1023, 0, 255);

    int RightStick_Left = map(analogRead(19), 0, 1023, 0, 255);
    int RightStick_Right = map(analogRead(22), 0, 1023, 0, 255);
    int RightStick_Up = map(analogRead(25), 0, 1023, 0, 255);
    int RightStick_Down = map(analogRead(26), 0, 1023, 0, 255);
}

void sendHeartbeat()
{
    Serial.print("Sending heartbeat");
    RadioPacket radioData;
    radioData.PacketType = Heartbeat;
    radioData.FromRadioId = RADIO_ID;
    radioData.OnTimeMillis = millis();

    if (_radio.send(DESTINATION_RADIO_ID, &radioData, sizeof(radioData)))
    {
        Serial.println("...Success");
    }
    else
    {
        Serial.println("...Failed");
    }
}

void JoySticks(JoyData)
{
    Serial.print("Sending joystick");
    JoyData data;
    data.JoyData = LeftStick_Left; //attribute
    data.JoyData = LeftStick_Right;
    data.JoyData = LeftStick_Up;
    data.JoyData = LeftStick_Down;
    data.JoyData = RightStick_Left;
    data.JoyData = RightStick_Right;
    data.JoyData = RightStick_Up;
    data.JoyData = RightStick_Down;
}

void requestData()
{
    Serial.println("Requesting data");
    Serial.print("  Sending BeginGetData");

    RadioPacket radioData;
    radioData.PacketType = BeginGetData; // When the receiver sees this packet type, it will load an ACK data packet.
    
    if (_radio.send(DESTINATION_RADIO_ID, &radioData, sizeof(radioData)))
    {
        Serial.println("...Success");
        Serial.print("  Sending EndGetData");

        radioData.PacketType = EndGetData; // When the receiver acknowledges this packet, we will get the ACK data.

        if (_radio.send(DESTINATION_RADIO_ID, &radioData, sizeof(radioData)))
        {
            Serial.println("...Success");

            while (_radio.hasAckData()) // Look to see if the receiver provided the ACK data.
            {
                RadioPacket ackData;
                _radio.readData(&ackData);

                if (ackData.PacketType == AcknowledgementData)
                {
                    String msg = "  Received ACK data from ";
                    msg += ackData.FromRadioId;
                    msg += ", ";
                    msg += ackData.OnTimeMillis;
                    msg += " ms";
                    Serial.println(msg);
                }
            }
        }
        else
        {
            Serial.println("...Failed");
        }
    }
    else
    {
        Serial.println("...Failed");
    }    
}
