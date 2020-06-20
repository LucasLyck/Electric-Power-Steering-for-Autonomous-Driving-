// CAN Transmitter (arduino pro mini) 
#include <SPI.h>
#include <mcp2515.h>


const uint16_t ANGLE_ID = 0xF6;

/**
 * initializing can msg 1 and 2 
 * struct can_frame - basic CAN frame structure
 * @can_id:  the CAN ID of the frame
 * @can_dlc: the data length field of the CAN frame
 * @data:    the CAN frame payload.
*/
struct can_frame canMsg;

// Set to 10 since CS pin from MCP2515 D10 on arduino
MCP2515 mcp2515(10);


void setup() {
  // Can message 1
  canMsg.can_id  = ANGLE_ID; /* 32 bit CAN_ID + EFF/RTR/ERR flags */
  canMsg.can_dlc = 1;     /* data length code: 0 .. 8 */
  
  while (!Serial);
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode(); //setNormalMode() allows messages to be recieved/transmitted over CAN 
}

int8_t FULL_RIGHT = 0x78;
int8_t FULL_LEFT = 0x88;

void loop() {

  canMsg.data[0] = FULL_RIGHT;
  mcp2515.sendMessage(&canMsg);
  delay(2000);

  canMsg.data[0] = FULL_LEFT;
  mcp2515.sendMessage(&canMsg);
  delay(2000);
}
