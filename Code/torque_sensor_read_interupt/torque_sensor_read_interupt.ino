int analogPin_A1 = A1; // potentiometer wiper (middle terminal) connected to analog pin 1
int analogPin_A0 = A0; // outside leads to ground and +5V
int count1 = 0;
int count2 = 0;

void setup() {
  // put your setup code here, to run once:

  cli();
 
 //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 156;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();

  Serial.begin(9600);           //  setup serial  
  pinMode(analogPin_A0, OUTPUT); // sets the pin as output
  analogWrite(analogPin_A0, 0); // set pin A0 to ground 

  pinMode(analogPin_A1, INPUT);
  //calibrateTorque();
}

int calibrateTorque() {

  int samples = 20;
  double val = 0.0;
  
  for (int i = 0; i < samples; i++){
    val += analogRead(analogPin_A1);
  }
  return (int) round(val/20);
  
}

ISR(TIMER1_COMPA_vect){
 
  interrupt();
//  count1++; 
//  if (count1 > 100) {
//    count1 = 1;
//  }
//  
//  if (count1 % 100 == 0) {
//    count2++;
//    Serial.println(count2);
//  }
    
}

void interrupt() {
  int val_A1 = analogRead(analogPin_A1);  // read the input pin
  //double val_A1_voltage = (5.0/1024)*val_A1; // Converts Arduino pin output to voltage
  //Serial.print("val_A1: ");
  Serial.println(val_A1);
  //Serial.println("Voltage: " + val_A1_voltage);          // debug value

//  int adc_res = 1024; // Arduino ADC resolution
//  int tau_zero = calibrateTorque(); // Zero torque measurement
//  double tau_max = 50.0; // maximum torque the sensor can measure (in Nm)
//  double val_A1_Nm = 2*tau_max/adc_res*(val_A1-tau_zero); // torque measurement in Nm
//  Serial.print("Nm: ");
//  Serial.println(val_A1_Nm);          // debug value
}

void loop() {
  // put your main code here, to run repeatedly:


}
