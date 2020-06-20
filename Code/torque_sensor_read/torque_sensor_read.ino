int analogPin_A1 = A1; // potentiometer wiper (middle terminal) connected to analog pin 1
int analogPin_A0 = A0;                   // outside leads to ground and +5V
            
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);           //  setup serial  
  pinMode(analogPin_A0, OUTPUT); // sets the pin as output
  analogWrite(analogPin_A0, 0); // set pin A0 to ground 

  pinMode(analogPin_A1, INPUT);
}

void loop() {
  int val_A1 = analogRead(analogPin_A1);  // read the input pin
  Serial.println(val_A1);          // debug value
}
