int buzzer = 8;           // Digital IO pin for the buzzer
int potPin = A0;          // Analog input pin for the potentiometer
int frequency;            // Variable to store the frequency

void setup() {
    pinMode(buzzer, OUTPUT);  // Set buzzer pin as output
    Serial.begin(9600);       // Initialize Serial Monitor at 9600 baud
}

void loop() {
    // Read potentiometer value (0 to 1023) and map it to 60-10000 Hz range
    int potValue = analogRead(potPin);
    frequency = map(potValue, 0, 1023, 60, 10000);

    // Calculate delay in microseconds for the given frequency
    int delayTime = 1000000 / (2 * frequency); // Half-period delay in microseconds

    // Output the sound
    digitalWrite(buzzer, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(buzzer, LOW);
    delayMicroseconds(delayTime);

    // Print the current frequency to the Serial Monitor
    Serial.print("Frequency is ");
    Serial.print(frequency);
    Serial.println(" Hz");

    delay(100);  // Optional: Add a small delay to avoid flooding the Serial Monitor
}
