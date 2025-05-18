#include "DHT.h"

#define PULSE_PIN 32    // Pulse sensor analog pin
#define DHTPIN 15       // DHT11 data pin
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE);

int threshold = 600;
int pulseValue = 0;
bool pulseDetected = false;
unsigned long lastBeatTime = 0;
unsigned long beatStartTime = 0;
int beatCount = 0;
int bpm = 0;

unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PULSE_PIN, INPUT);
  dht.begin();
}

void loop() {
  unsigned long currentTime = millis();

  // Pulse Sensor BPM Calculation
  pulseValue = analogRead(PULSE_PIN);

  if (pulseValue > threshold && !pulseDetected) {
    pulseDetected = true;
    unsigned long beatInterval = currentTime - lastBeatTime;

    if (beatInterval > 400) { // ignore too fast pulses
      lastBeatTime = currentTime;

      if (beatCount == 0) {
        beatStartTime = currentTime;
      }

      beatCount++;

      if (beatCount == 10) {
        unsigned long duration = currentTime - beatStartTime;
        bpm = (10 * 60000UL) / duration;
        beatCount = 0;
      }
    }
  }

  if (pulseValue < threshold) {
    pulseDetected = false;
  }

  if (currentTime - lastPrintTime >= 2000) {
    lastPrintTime = currentTime;

    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    Serial.print("BPM: ");
    Serial.println(bpm);

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("Failed to read from DHT sensor!");
    } else {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" °C");

      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");
    }

    Serial.println("---------------------");
  }

  delay(5);
}
