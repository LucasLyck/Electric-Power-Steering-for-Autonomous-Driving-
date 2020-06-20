
#define ADC_ADRESS 0x00

#include <Wire.h> // I2C / TWI library (connectection to the ADC)
#include <SPI.h> // SPI library (connection to the CAN-tranciever)
#include <mcp_can.h> // can protocoll library

// dunno yet
MCP_CAN CAN(spiCSPin);

void setup() {

  // debug connection to USB
  Serial.begin(115200);
  
  // setup an interrupt at 100Hz
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (1610^6) / (11024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bits for 8 prescaler
  TCCR1B |= (1 << CS11);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  // validate k i2c line and see if we have a connection to the adc
  Wire.begin(); // join i2c bus as master

  // validate connection to the CAN transciever
    while (CAN_OK != CAN.begin(CAN_500KBPS))
    {
        Serial.println("CAN BUS init Failed");
        delay(100);
    }
    Serial.println("CAN BUS Shield Init OK!");
}

void loop() {
  // put your main code here, to run repeatedly:

}

void interrupt() {
  // request new adc reading (or recieve an automatic one)
    Wire.beginTransmission(ADC_ADRESS);
    Wire.write()
    error = Wire.endTransmission();

    // check for error
    if (error != 0) {
      // we have an error
    }

    // get the reading
    char buffer[16];
    Wire.requestFrom(ADC_ADRESS, 16);    // request 6 bytes from slave device #2

    for(i=0; i<16 && Wire.available(); i++){
      buffer[i] = Wire.read();
    }

    
  // send the adc reading to the CAN bus
  unsigned char stmp[8] = {ledHIGH, 1, 2, 3, ledLOW, 5, 6, 7}; // can data info
  Serial.println("In loop");
  CAN.sendMsgBuf(0x43, 0, 8, stmp);
}

uint8_t state = 0; 

ISR(TIMER0_COMPA_vect){//timer0 interrupt 100kHz 
  state++;
  if (state == 100) {
    Serial.print(micros());
    state = 0;
  }
