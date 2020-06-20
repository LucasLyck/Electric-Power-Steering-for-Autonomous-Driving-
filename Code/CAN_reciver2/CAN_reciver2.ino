// CAN Reciever (arduino pro mini) 

#include <SPI.h>
#include <mcp2515.h>

/**
 * initializing can msg 1 and 2 
 * struct can_frame - basic CAN frame structure
 * @can_id:  the CAN ID of the frame
 * @can_dlc: the data length field of the CAN frame
 * @data:    the CAN frame payload.
*/
struct can_frame canMsg;
MCP2515 mcp2515(10);


void setup() {
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();  //setNormalMode() allows messages to be recieved/transmitted over CAN 
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
}

void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.print(canMsg.can_id, HEX); // print ID
    Serial.print(" "); 
    Serial.print(canMsg.can_dlc, HEX); // print DLC
    Serial.print(" ");
    
    for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
      Serial.print(canMsg.data[i],HEX);
      Serial.print(" ");
    }

    Serial.println();      
  }
}
