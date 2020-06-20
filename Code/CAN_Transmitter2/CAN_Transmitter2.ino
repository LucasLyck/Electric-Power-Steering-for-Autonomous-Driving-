// CAN Transmitter (arduino pro mini) 
//#include <SPI.h>
#include <mcp2515.h>


/**
 * initializing can msg 1 and 2 
 * struct can_frame - basic CAN frame structure
 * @can_id:  the CAN ID of the frame
 * @can_dlc: the data length field of the CAN frame
 * @data:    the CAN frame payload.
*/
struct can_frame canMsg1;
struct can_frame canMsg2;
struct can_frame canMsg3;

int count = 0;


// Set to 10 since CS pin from MCP2515 is pin D10 on arduino
MCP2515 mcp2515(10);


void setup() {

  while (!Serial);
  Serial.begin(115200);
  
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  // OCR1A = 15624;// = (16*10^6) / (100*1024) - 1 (must be <65536) // 1 Hz
  OCR1A = 156;// = (16*10^6) / (100*1024) - 1 (must be <65536) // 100 Hz
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  // Can message 1
  canMsg1.can_id  = 0x00; /* 32 bit CAN_ID + EFF/RTR/ERR flags */
  canMsg1.can_dlc = 1;     /* data length code: 0 .. 8 */
  canMsg1.data[0] = 0xF0;
//  canMsg1.data[1] = 0x87;
//  canMsg1.data[2] = 0x32;
//  canMsg1.data[3] = 0xFA;
//  canMsg1.data[4] = 0x26;
//  canMsg1.data[5] = 0x8E;
//  canMsg1.data[6] = 0xBE;
//  canMsg1.data[7] = 0x86;

  // Can message 2
  canMsg2.can_id  = 0x00;
  canMsg2.can_dlc = 1;
  canMsg2.data[0] = 0x78;
//  canMsg2.data[1] = 0x00;
//  canMsg2.data[2] = 0x00;
//  canMsg2.data[3] = 0x08;
//  canMsg2.data[4] = 0x01;
//  canMsg2.data[5] = 0x00;
//  canMsg2.data[6] = 0x00;
//  canMsg2.data[7] = 0xA0;

  // Can message 3
  canMsg3.can_id  = 0x00;
  canMsg3.can_dlc = 1;
  canMsg3.data[0] = 0x00;
//  canMsg2.data[1] = 0x00;
//  canMsg2.data[2] = 0x00;
//  canMsg2.data[3] = 0x08;
//  canMsg2.data[4] = 0x01;
//  canMsg2.data[5] = 0x00;
//  canMsg2.data[6] = 0x00;
//  canMsg2.data[7] = 0xA0;
  
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode(); //setNormalMode() allows messages to be recieved/transmitted over CAN 
  
  Serial.println("Example: Write to CAN");
}

ISR(TIMER1_COMPA_vect){
  if (count < 100) {
  mcp2515.sendMessage(&canMsg1);
  count++;
  }
  if (count > 100 && count < 200) {
  mcp2515.sendMessage(&canMsg2);
  count++;
  }
  if (count > 200 && count < 300) {
  mcp2515.sendMessage(&canMsg3);
  count++; 
  }
  if (count == 300) {
    count = 0;
  }

  Serial.println("Messages sent"); // Checks if messages have been send 
  
//  delay(100);
}

void loop() {
  
 
}
