#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include "LCDControl.h"
#include "ServoControl.h"
#include "GameLogic.h"

// Pin assignments for the hardware components
const int pushButtonPin = 2;          
const int buzzerPin = 3;              
const int greenLedPin = 4;            
const int redLedPin = 5;         
const int ultrasonicTriggerPin = 6;  
const int ultrasonicEchoPin = 7;     

// Game variables initialization
volatile bool buttonPressed = false; // button press detection
bool gameRunning = false;            // check if the game is running
unsigned long previousMillis = 0;    // Store the last time the timer was updated
int gametime = 60;                   // Total game time (60 seconds ONLYY)
const int maxTime = 60;              // Maximum game time (60 seconds)
bool isGreen = true;                 // State of the light (Green or Red)
int lastDistance = 0;                // Store the last measured distance (for motion detection)
const int motionTolerance = 5;       // Motion detection in cm

// Random duration ranges (in milliseconds)
const int greenLightMin = 2000; // Green light minimum duration (2 seconds)
const int greenLightMax = 5000; // Green light maximum duration (5 seconds)
const int redLightMin = 3000;   // Red light minimum duration (3 seconds)
const int redLightMax = 6000;   // Red light maximum duration (6 seconds)

// Victory condition: player wins if they are within 5 cm of the sensor!!! RIGHT IN FRFONT OF SENSOR
const int victoryDistance = 5;    // Winning distance (5 cm)

unsigned long greenLightEndTime = 0;  // End time for the Green Light phase
unsigned long redLightEndTime = 0;    // End time for the Red Light phase

// Declare global variables for hardware components
LiquidCrystal_I2C lcd(0x27, 16, 2);   
Servo ultraServo;                     // Ultrasonic sensor servo
Servo eliminationServo;               // Elimination arm servo

// Setup function: Runs once when the board starts
void setup() {
    // Initialize the LCD display
    lcd.begin();   
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Press to Start");

    // Initialize hardware pins
    pinMode(pushButtonPin, INPUT_PULLUP); // Push button with pull-up resistor
    pinMode(buzzerPin, OUTPUT);          
    pinMode(greenLedPin, OUTPUT);     
    pinMode(redLedPin, OUTPUT);           
    pinMode(ultrasonicTriggerPin, OUTPUT); 
    pinMode(ultrasonicEchoPin, INPUT);    

    // Attach servos to their respective pins
    ultraServo.attach(33);       //pin 33
    eliminationServo.attach(31); // pin 31
    eliminationServo.write(180); // Move the arm to rest position (180 degrees)
    ultraServo.write(0);         // Move ultrasonic sensor servo to initial position

    // Attach interrupt for button press (onButtonPress is called when the button is pressed)
    attachInterrupt(digitalPinToInterrupt(pushButtonPin), onButtonPress, FALLING);

    Serial.begin(9600);           // Start serial communication for debugging
    randomSeed(analogRead(0));    // Initialize random number generator

    // Set up Timer2 for 1-second intervals (Timer 1 is being used already pin-wise)
    TCCR2A = 0;    
    TCCR2B = 0;           
    TCCR2B |= B00000011;  
    OCR2A = 156;          // Set Compare Match value for 1 second 
    TIMSK2 |= (1 << OCIE2A);  // Enable Timer2 Compare Match A interrupt
}

// Main game loop function
void loop() {
    gameLoop();  // Call the game loop function
}

// Interrupt service routine for button press (debounces and sets buttonPressed flag)
void onButtonPress() {
    static unsigned long lastDebounceTime = 0; // Debounce time tracking
    unsigned long currentTime = millis();      // Get current time

    // If enough time has passed (debounce time), register the button press
    if (currentTime - lastDebounceTime > 50) { 
        buttonPressed = true;
        lastDebounceTime = currentTime; // Update debounce time
    }
}

// Game loop: Handles game flow, light phases, and timer updates
void gameLoop() {
    // If the button is pressed, start the game
    if (buttonPressed) {
        buttonPressed = false;  // Reset button press
        gameRunning = true;     // Set game as running
        gametime = maxTime;     // Set game time to max (60 seconds)
        previousMillis = millis(); // Reset timer to current time in milliseconds
        isGreen = true;         // Start with Green Light phase
        greenLightEndTime = previousMillis + random(greenLightMin, greenLightMax); // Set green light end time

        lcd.clear();               // Clear the LCD
        lcd.setCursor(0, 0);       // Set cursor to top left
        lcd.print("Game Started!"); // Display game started message
        delay(2000);               // Wait for 2 seconds before starting game
    }

    // If the game is running, handle timing and light phases
    if (gameRunning) {
        unsigned long currentMillis = millis(); // Get current time in milliseconds

        // Update the game timer every second
        if (currentMillis - previousMillis >= 1000) {
            previousMillis = currentMillis; // Update the last time checked
            gametime--;  // Decrease the remaining game time by 1 second

            // If time is up, call timesUp() to handle game over
            if (gametime <= 0) {
                timesUp();  // Time is up, call the timesUp function
                return;  // Exit the function as game is over
            } else {
                displayTime();  // Update the timer display on the LCD
            }
        }

        // Handle the Green Light or Red Light phases
        if (isGreen) {
            // If current time exceeds the green light end time, switch to red light
            if (currentMillis >= greenLightEndTime) {
                isGreen = false;
                redLightEndTime = currentMillis + random(redLightMin, redLightMax); // Set red light duration
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Red Light!");  // Display Red Light message
            }
            greenLight(); // Run the green light phase logic
        } else {
            // If current time exceeds the red light end time, switch to green light
            if (currentMillis >= redLightEndTime) {
                isGreen = true;
                greenLightEndTime = currentMillis + random(greenLightMin, greenLightMax); // Set green light duration
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Green Light!"); // Display Green Light message
            }
            redLight(); // Run the red light phase logic
        }
    }
}

// Display the remaining time on the LCD screen
void displayTime() {
    lcd.setCursor(0, 1);         
    lcd.print("Time Left: ");     
    lcd.print(gametime);        
    lcd.print("s ");            
}

// Green Light phase logic
void greenLight() {
    digitalWrite(greenLedPin, HIGH); // Turn on green LED
    digitalWrite(redLedPin, LOW);    // Turn off red LED

    // Activate the buzzer for a short duration (50ms)
    tone(buzzerPin, 1000);  
    delay(400);              
    noTone(buzzerPin);      

    ultraServo.write(0);    // Rotate servo to face away from the player
}

// Red Light phase logic
void redLight() {
    ultraServo.write(180);  // Rotate servo to face the player
    delay(400);             // Small delay to stabilize servo

    int distance = getDistance(); 
    lcd.setCursor(0, 1);         
    lcd.print("Dist: ");       
    lcd.print(distance);      
    lcd.print("cm");          

    digitalWrite(greenLedPin, LOW); // Turn off green LED
    digitalWrite(redLedPin, HIGH);  // Turn on red LED

    // Activate the buzzer for a short duration (50ms)
    tone(buzzerPin, 800);  
    delay(200);           
    noTone(buzzerPin);    

    // Check for victory (if the player is too close to the sensor)
    if (distance < victoryDistance) {
        checkVictory();  // Player has won, call checkVictory function
    } else if (abs(distance - lastDistance) > motionTolerance) {
        eliminatePlayer(); // Player moved, eliminate them
    }

    lastDistance = distance;  // Update last distance for next check
}

// Get the distance using the ultrasonic sensor
int getDistance() {
    digitalWrite(ultrasonicTriggerPin, LOW);
    delayMicroseconds(2);                    
    digitalWrite(ultrasonicTriggerPin, HIGH); 
    delayMicroseconds(10);                    
    digitalWrite(ultrasonicTriggerPin, LOW); 

    long duration = pulseIn(ultrasonicEchoPin, HIGH); // Measure echo time
    return duration * 0.034 / 2; // Convert the duration to cm (speed of sound is 0.034 cm/micorsecond)
}

// Handle player elimination
void eliminatePlayer() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motion Detected");
    lcd.setCursor(0, 1);
    lcd.print("Game Over!");

    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    flashRedLight();
    playGameOverTone();

    eliminationServo.write(0);   // Sweep elimination arm
    delay(2000);
    eliminationServo.write(180); // Return to rest position

    resetGame();
}

// Handle the game end (time's up)
void timesUp() {
    // Reset Red LED and Buzzer state when time is up
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    noTone(buzzerPin);  // Turn off any ongoing buzzer sound

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time's Up!");
    flashRedLight();
    playGameOverTone();

    eliminationServo.write(0);   // Sweep elimination arm
    delay(2000);
    eliminationServo.write(180); // Return to rest position

    resetGame();
}

// Check if the player wins
void checkVictory() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("You Win!");
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    playVictoryTone();

    gameRunning = false;
    resetGame();
}

// Flash red light as an indicator
void flashRedLight() {
    for (int i = 0; i < 5; i++) {
        digitalWrite(redLedPin, HIGH);
        delay(200);
        digitalWrite(redLedPin, LOW);
        delay(200);
    }
}

// Play victory tone
void playVictoryTone() {
    tone(buzzerPin, 1000, 500);
    delay(500);
    tone(buzzerPin, 1200, 500);
    delay(500);
    tone(buzzerPin, 1500, 500);
    delay(500);
    noTone(buzzerPin);
}

// Play game over tone
void playGameOverTone() {
    tone(buzzerPin, 500, 1000);
    delay(1000);
    noTone(buzzerPin);
}

// Reset the game to its initial state
void resetGame() {
    gameRunning = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press to Start");
    digitalWrite(greenLedPin, LOW);  // Ensure Green LED is off
    digitalWrite(redLedPin, LOW);    // Ensure Red LED is off
    noTone(buzzerPin);               // Ensure buzzer is off
}
