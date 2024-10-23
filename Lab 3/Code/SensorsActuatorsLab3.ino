int redLedPin = 10;                        // Red LED pin (was previously green)
int greenLedPin = 11;                      // Green LED pin (was previously red)
int buttonPin = 7;                         // Button pin
int buttonState;                           // Variable to store button state

void setup() {
    pinMode(redLedPin, OUTPUT);            // Set red LED pin as output
    pinMode(greenLedPin, OUTPUT);          // Set green LED pin as output
    pinMode(buttonPin, INPUT);             // Set button pin as input
}

void loop() {
    buttonState = digitalRead(buttonPin);  // Read the button state

    if (buttonState == LOW) {              // Button is pressed
        digitalWrite(redLedPin, HIGH);     // Turn on red LED
        digitalWrite(greenLedPin, LOW);    // Turn off green LED
    } else {  // Button is released
        digitalWrite(redLedPin, LOW);      // Turn off red LED
        digitalWrite(greenLedPin, HIGH);   // Turn on green LED
    }
}
