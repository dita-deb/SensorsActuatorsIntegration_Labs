int servopin = 9;  // Select digital pin 9 for servo motor signal line
int potpin = A0;   // Potentiometer pin (analog input)
int adcValue;      // Variable to store ADC value
int angle;         // Servo angle
int pulsewidth;    // Initialize width variable

void servopulse(int servopin, int myangle) {  // Define a servo pulse function
    pulsewidth = (myangle * 11) + 500;  // Convert angle to 500-2500 pulse width
    digitalWrite(servopin, HIGH);  // Set the level of servo pin as “high”
    delayMicroseconds(pulsewidth);  // Delay microsecond of pulse width
    digitalWrite(servopin, LOW);   // Set the level of servo pin as “low”
    delay(20 - pulsewidth / 1000); // Maintain a 20ms period
}

void setup() {
    pinMode(servopin, OUTPUT);  // Set servo pin as “output”
    Serial.begin(9600);  // Connect to serial port, set baud rate at “9600”
    Serial.println("Servo control ready");
}

void loop() {
    // Read the potentiometer value (0-1023)
    adcValue = analogRead(potpin);
  
    // Map the ADC value to a servo angle (0-180 degrees)
    angle = map(adcValue, 0, 1023, 0, 180);
  
    // Print the ADC value and corresponding servo angle to the Serial Monitor
    Serial.print("ADC Value: ");
    Serial.print(adcValue);
    Serial.print(" | Servo Angle: ");
    Serial.println(angle);
  
    // Generate a PWM pulse to control the servo position
    servopulse(servopin, angle);
  
    delay(100);  // Small delay for stability
}
