#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 9;
const int LOADCELL_SCK_PIN = 4;

HX711 scale;

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing HX711...");

  // Initialize HX711
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  // Optional: Set scale calibration factor after initial calibration
  // Replace -7050 with your calibration factor
  scale.set_scale(1106);
  scale.set_offset(-62230);

  Serial.println("HX711 initialized. Place weight on scale...");
}

void loop() {
  scale.power_up();

  if (scale.wait_ready_timeout(1000)) {
    float weight = scale.get_units(10);  // Get an average of 10 readings
    Serial.print("Weight: ");
    Serial.print(weight);
    Serial.println(" g");
  } else {
    Serial.println("HX711 not ready.");
  }

  scale.power_down();

  delay(500);
}
