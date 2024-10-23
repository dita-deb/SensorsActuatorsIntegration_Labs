#include <dht11.h>

dht11 DHT;
#define DHT11_PIN 4

float lastTempC = -999;  // Store the last temperature to check for changes
int timeCounter = 0;     // Counter to track elapsed time in seconds

void setup(){
  Serial.begin(9600);
  Serial.println("Time (s),\tTemperature (C),\tTemperature (F),\tTemperature (K)");
}

void loop(){
  int chk = DHT.read(DHT11_PIN);  // Read data from DHT11 sensor

  if (chk == DHTLIB_OK) {
    float tempC = DHT.temperature;        // Temperature in Celsius

    if (tempC != lastTempC) {  // Display only if temperature changes
      float tempF = tempC * 9.0 / 5.0 + 32;  // Convert to Fahrenheit
      float tempK = tempC + 273.15;          // Convert to Kelvin

      // Display temperature data
      Serial.print(timeCounter);
      Serial.print(" s,\t");
      Serial.print(tempC, 1);
      Serial.print(" °C,\t");
      Serial.print(tempF, 1);
      Serial.print(" °F,\t");
      Serial.print(tempK, 1);
      Serial.println(" K");

      lastTempC = tempC;  // Update lastTempC to current temperature
    }

    timeCounter++;
  } else {
    Serial.println("Error reading DHT11 sensor data.");
  }

  delay(1000);  // Wait 1 second for the next reading
}
