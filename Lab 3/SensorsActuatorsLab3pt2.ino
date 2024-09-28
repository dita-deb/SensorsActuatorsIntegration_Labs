int redLedPin = 11;              // Red LED pin
int greenLedPin = 10;            // Green LED pin
int buttonPin = 7;               // Button pin
int buttonState;                 // Variable to store button state
int lastButtonState = HIGH;      // Variable to store the last state of the button
int pressCount = 0;              // Variable to count button presses
unsigned long duration = 1000;   // Initial duration of 1 second (in milliseconds)

void setup() {
    pinMode(redLedPin, OUTPUT);  // Set red LED pin as output
    pinMode(greenLedPin, OUTPUT);// Set green LED pin as output
    pinMode(buttonPin, INPUT);   // Set button pin as input

    Serial.begin(9600);          // Initialize serial communication at 9600 bps
}

void loop() {
    buttonState = digitalRead(buttonPin);  // Read the button state

    // Detect button press (LOW state)
    if (buttonState == LOW && lastButtonState == HIGH) {
        pressCount++;  // Increment press count

        // Reset the cycle after 5 presses
        if (pressCount > 5) {
            pressCount = 1;
        }

        // Calculate duration based on press count (1 to 5 seconds)
        duration = pressCount * 1000;

        // Log press count and duration to Serial Monitor
        Serial.print("Button Pressed: ");
        Serial.print(pressCount);
        Serial.print(", Duration: ");
        Serial.println(duration);

        // Light the green LED for 'duration' milliseconds when the button is pressed
        digitalWrite(greenLedPin, HIGH);
        delay(duration);
        digitalWrite(greenLedPin, LOW);
    }

    // Detect button release (HIGH state)
    if (buttonState == HIGH && lastButtonState == LOW) {
        // Light the red LED for 'duration' milliseconds when the button is released
        digitalWrite(redLedPin, HIGH);
        delay(duration);
        digitalWrite(redLedPin, LOW);

        // Log the red LED activity to Serial Monitor
        Serial.print("Button Released: ");
        Serial.print(pressCount);
        Serial.print(", Duration: ");
        Serial.println(duration);
    }

    // Save the current button state for the next loop
    lastButtonState = buttonState;
}
