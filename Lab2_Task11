#include <Arduino_APDS9960.h>
#include <Arduino_HS300x.h>
#include <Arduino_LSM9DS1.h>
#include <Arduino_BMI270_BMM150.h>

// Initial values (used to detect changes)
float prev_rh = 0;
float prev_temp = 0;
float prev_mag = 0;
int prev_clear = 0;

// Cooldown variables
unsigned long lastEventTime = 0;
const int cooldown = 2000;        // 2 seconds
String lastLabel = "";

// Setup Function (runs 1x @ beginning of program)
void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Humidity and temperature
  if (!HS300x.begin()) {
    Serial.println("Failed to initialize humidity/temp sensor!");
    while (1);
  }

  // Light
  if (!APDS.begin()) {
    Serial.println("Failed to initialize light sensor!");
    while (1);
  }

  // Magnetic
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

// Loop Function
//   - Read all required sensing modalities continuously
//   - Detect event conditions using rule-based logic only
//   - Print a single final event label to Serial Monitor
//   - Avoid repeated rapid re-triggering of the same event by using a
//     short cooldown or debounce logic
void loop() {
  // Read sensor values
  float rh = HS300x.readHumidity();
  float temp = HS300x.readTemperature();
  
  float mx = 0, my = 0, mz = 0;
  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(mx, my, mz);
  }

  // magnitude for magnetic field
  float mag = sqrt(mx * mx + my * my + mz * mz);

  int r = 0, g = 0, b = 0, clear = 0;
  if (APDS.colorAvailable()) {
    APDS.readColor(r, g, b, clear);
  }

  // Calculate thresholds
  bool humid_jump = abs(rh - prev_rh) > 3.0;
  bool temp_rise = (temp - prev_temp) > 1.0;
  bool mag_shift = abs(mag - prev_mag) > 5.0;
  bool light_or_color_change = abs(clear - prev_clear) > 500;

  // Classify
  String label = "BASELINE_NORMAL";

  if (mag_shift) {
    label = "MAGNETIC_DISTURBANCE_EVENT";
  }
  else if (light_or_color_change) {
    label = "LIGHT_OR_COLOR_CHANGE_EVENT";
  }
  else if (humid_jump || temp_rise) {
    label = "BREATH_OR_WARM_AIR_EVENT";
  }
  else {
    label = "BASELINE_NORMAL";
  }

// Print statements for current values and final label
    // Line 1: current values
    Serial.print("rh=");
    Serial.print(rh);
    Serial.print(" temp=");
    Serial.print(temp);
    Serial.print(" mag=");
    Serial.print(mag);
    Serial.print(" r=");
    Serial.print(r);
    Serial.print(" g=");
    Serial.print(g);
    Serial.print(" b=");
    Serial.print(b);
    Serial.print(" clear=");
    Serial.println(clear);

    // Line 2: indicators
    Serial.print("humid_jump=");
    Serial.print(humid_jump);
    Serial.print(" temp_rise=");
    Serial.print(temp_rise);
    Serial.print(" mag_shift=");
    Serial.print(mag_shift);
    Serial.print(" light_or_color_change=");
    Serial.println(light_or_color_change);

    // Line 3: final label
    Serial.print("FINAL_LABEL=");
    Serial.println(label);
    Serial.println();
  
  // update values at end
  prev_rh = rh;
  prev_temp = temp;
  prev_mag = mag;
  prev_clear = clear;

  // short delay for cool down
  delay(200);
}
