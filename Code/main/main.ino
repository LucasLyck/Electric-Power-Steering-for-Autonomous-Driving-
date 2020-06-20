#include <SPI.h>
#include <mcp2515.h>

struct can_frame canMsg;
MCP2515 mcp2515(10);

// external angle ID 11-bit
const uint16_t ANGLE_ID = 0xF6;


int TORQUE_PIN = A1; // potentiometer wiper (middle terminal) connected to analog pin 1

const int8_t TORQUE_PIN_GND = 0;
bool motor_on = false;

int count1 = 0;
int count2 = 0;

// fixP_t is just an int32_t, but it is written wherever fixed points are used, as it clarifies the code.
#define fixP_t int32_t
#define DEC_BITS 14

// correction for 18.14 fixed point multiplication and division.
#define FIX14_MULT(a, b) ( ((a >> (DEC_BITS / 2)) * (b >> (DEC_BITS / 2))) )
#define FIX14_DIV(a, b)  ( ((a) << DEC_BITS) / b )

fixP_t zero_point = 0;
fixP_t speed = 0;

/* Example sketch to control a stepper motor with TB6600 stepper motor driver and Arduino without a library: continuous rotation. More info: https://www.makerguides.com */
// Define stepper motor connections:W
const int8_t RELAY_PIN = 2;
const int8_t DRIVER_PULS_GROUND = 4;
const int8_t DRIVER_PULS_SIGNAL = 5;
const int8_t DRIVER_DIRECTION_SIGNAL = 6;
const int8_t DRIVER_DIRECTION_GROUND = 7;


// 8-bit int representing the current wheel angle in degrees.
int16_t wheel_rot = 0;
int16_t wheel_rot_goal = 0;

void setup() {

  
  // put your setup code here, to run once:

  // Set the driver pins as outputs
  DDRD |= (1 << RELAY_PIN) | (1 << DRIVER_PULS_GROUND) | (1 << DRIVER_PULS_SIGNAL) | (1 << DRIVER_DIRECTION_SIGNAL) | (1 << DRIVER_DIRECTION_GROUND); // DDRD - The Port D Data Direction Register for pin digital pins 0 to 7

  // set pins high
  PORTD |= (1 << RELAY_PIN);
  // set pins low
  PORTD &= ~(1 << DRIVER_DIRECTION_SIGNAL) | (1 << DRIVER_PULS_GROUND) | (1 << DRIVER_DIRECTION_GROUND);

  calibrateTorque(); // Zero torque measurement
  Serial.begin(115200);
  
  mcp2515.reset();
  mcp2515.setBitrate(CAN_125KBPS);
  mcp2515.setNormalMode();  //setNormalMode() allows messages to be recieved/transmitted over CAN 
  
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
 
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

 // Set torque pin gnd as output
  DDRC |= (1 << TORQUE_PIN_GND);
 // set torque pin as input
  DDRC &= ~(1 << TORQUE_PIN);
  
  // set output to 0 Volts
  PORTC &= ~(1 << TORQUE_PIN_GND); // set pin to 0V analog
 
}

void calibrateTorque() {

  zero_point = 0;
  
  uint16_t samples = 1000;
  int32_t val = 0;
  
  for (int i = 0; i < samples; i++){
    val += get_torque_reading();
    delayMicroseconds(2000);
  }
  zero_point = val / samples;
}

fixP_t get_torque_reading() {

  const fixP_t TORQUE_OUT_RANGE = 5 << DEC_BITS; // +/-5 votls
  const fixP_t TORQUE_IN_RANGE = 50 << DEC_BITS; // +/-50 Nm
  const fixP_t ADC_RESOLUTION = 1024; // 1024 bits
  const fixP_t SUPPLY_VOLTAGE = 5; // 5 volts
  fixP_t arduino_pin_volts = ((static_cast<fixP_t>(analogRead(TORQUE_PIN)) << DEC_BITS) /  ADC_RESOLUTION) * SUPPLY_VOLTAGE;  // read the input pin
  //printFixP(arduino_pin_volts);
  //Serial.print(" ");
  fixP_t torque_voltage = static_cast<fixP_t>(-82466) + FIX14_MULT(static_cast<fixP_t>(33125), arduino_pin_volts); // in decimal: -5.033333335 + 2.021766666*arduino_pin_volts
  //printFixP(torque_voltage);
  //Serial.print(" ");
  return FIX14_MULT(FIX14_DIV(TORQUE_IN_RANGE, TORQUE_OUT_RANGE), torque_voltage);
}


ISR(TIMER1_COMPA_vect){
    fixP_t torque = get_torque_reading() - zero_point;
    
    Serial.print("Nm: ");
    printFixP(torque);          // debug value
    Serial.println();
    //torque = 0; // override
  
    if (torque < -static_cast<fixP_t>((3 << 11))) { // -0.10 in decimal
      wheel_rot_goal = wheel_rot - 1;
    } else if (torque > static_cast<fixP_t>((3 << 11))) { // 0.10 in decimal
      wheel_rot_goal = wheel_rot + 1;
    } else {
      if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
  
        // we get information about steering angle
        if (canMsg.can_id == ANGLE_ID &&  canMsg.can_dlc == 1) {
          Serial.println(canMsg.data[0]);
          wheel_rot_goal = static_cast<int16_t>(static_cast<int8_t>(canMsg.data[0])) * static_cast<int16_t>(20) / static_cast<int16_t>(36);
          Serial.println(wheel_rot_goal);
        }
      }
    }
  
//  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
//    Serial.print(canMsg.can_id, HEX); // print ID
//    Serial.print(" "); 
//    Serial.print(canMsg.can_dlc, HEX); // print DLC
//    Serial.print(" ");
//    // debug CAN, prints the data
//    for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
//      Serial.print(canMsg.data[i],HEX);
//      Serial.print(" ");
//    }
//    Serial.println();
//  }
    
}

void loop() {

    // counter-clockwise
    if (wheel_rot < wheel_rot_goal) {
      PORTD &= ~(1 << DRIVER_DIRECTION_SIGNAL); // set low
      PORTD |= (1 << DRIVER_PULS_SIGNAL); // set high
      delayMicroseconds(24000);
      PORTD &= ~(1 << DRIVER_PULS_SIGNAL); // set low
      delayMicroseconds(24000);
      wheel_rot++;
      //Serial.println(wheel_rot);
    // clockwise
    } else if(wheel_rot > wheel_rot_goal) {
      PORTD |= (1 << DRIVER_DIRECTION_SIGNAL); // set high
      PORTD |= (1 << DRIVER_PULS_SIGNAL); // set high
      delayMicroseconds(240000);
      PORTD &= ~(1 << DRIVER_PULS_SIGNAL); // set low
      delayMicroseconds(240000); 
      wheel_rot--;
    }
}



// DEBUG --------------------------------------
void printFixP(fixP_t num) {
    double decimal = num >> DEC_BITS;
    fixP_t mask;
    for (int i = 0; i < DEC_BITS; i++) {
        mask = static_cast<fixP_t>(1) << i;
        decimal += static_cast<double>((num & mask) >> i) / (static_cast<fixP_t>(1) << (DEC_BITS-i)); 
    }
    Serial.print(decimal, DEC_BITS);
}

fixP_t float_to_fixed(float x)
{
    return (fixP_t) (x * pow(2, DEC_BITS));
}
