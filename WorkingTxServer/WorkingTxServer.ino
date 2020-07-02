#include <SPI.h>
#include <NRFLite.h>

int LeftStick_Left =0;
int LeftStick_Right =0;
int LeftStick_Up =0;
int LeftStick_Down =0;
int RightStick_Left =0;
int RightStick_Right =0;
int RightStick_Up =0;
int RightStick_Down =0;

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

const static uint8_t RADIO_ID = 0;
const static uint8_t DESTINATION_RADIO_ID = 1;
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
uint32_t _lastMessageSendTime;

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
    while (_radio.hasData())
    {
        RadioPacket radioData;
        _radio.readData(&radioData);

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
        
            
        if (radioData.PacketType == BeginGetData)
        {

           
            Serial.println("Received data request, adding ACK packet");

            RadioPacket ackData;
            ackData.PacketType = ReceiverData;
            ackData.FromRadioId = RADIO_ID;
            ackData.OnTimeMillis = millis();

            // Add the data to send back to the transmitter.
            // The next packet we receive will be acknowledged with this data.
            _radio.addAckData(&ackData, sizeof(ackData));
        }
        else if (radioData.PacketType == EndGetData)
        {
            // The transmitter hopefully received the acknowledgement data packet at this point.
        }
    }
}

