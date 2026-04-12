#include <PDM.h>
#include <Arduino_BMI270_BMM150.h>
#include <Arduino_APDS9960.h>

// Microphone Setup
short sampleBuffer[256];
volatile int samplesRead = 0;
void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(sampleBuffer, bytesAvailable);
  samplesRead = bytesAvailable / 2;
}

// Setup Function (runs 1x @ beginning of program)
void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Microphone
  PDM.onReceive(onPDMdata);
  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }

  // IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // Light and Proximity
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS9960!");
    while (1);
  }
}

// Loop Function
//   - Read all four sensing modalities continuously
//   - Computes one binary decision for each modality
//   - Combine all binary decisions using rule-based logic only
//   - Prints the final situation label to Serial Monitor
void loop() {
  // Modality 1: Microphone (audio activity)
  long micLevel = 0;
  for (int i = 0; i < samplesRead; i++) {
    micLevel += abs(sampleBuffer[i]);
  }
  micLevel = micLevel / samplesRead;
  bool sound = (micLevel > 750);       // microphone threshold

  // Modality 2: Light (ambient brightness)
  int r = 0, g = 0, b = 0, clear = 0;
  if (APDS.colorAvailable()) {
    APDS.readColor(r, g, b, clear);
  }
  bool dark = (clear < 200);            // light threshold

  // Modality 3: IMU (physical motion)
  float x, y, z;
  float motion = 0;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    motion = abs(x) + abs(y) + abs(z);
  }
  bool moving = (motion > 2.0);         // IMU threshold

  // Modality 4: Proximity (presence near board)
  int prox = 0;
  if (APDS.proximityAvailable()) {
    prox = APDS.readProximity();
  }
  bool near = (prox > 100);             // proximity threshold


  // State based on overall system
  String label = "";

  // Priority-based mapping to always pick one of the four labels
  if (sound && !dark && moving && near) {
    label = "NOISY_BRIGHT_MOVING_NEAR";
  }
  else if (!sound && dark && !moving && near) {
    label = "QUIET_DARK_STEADY_NEAR";
  }
  else if (sound && !dark && !moving && !near) {
    label = "NOISY_BRIGHT_STEADY_FAR";
  }
  else if (!sound && !dark && !moving && !near) {
    label = "QUIET_BRIGHT_STEADY_FAR";
  }
  

  // Final print statement (format follows specifications in lab document
  Serial.print("mic=");
  Serial.print(micLevel);
  Serial.print(" clear=");
  Serial.print(clear);
  Serial.print(" motion=");
  Serial.print(motion);
  Serial.print(" prox=");
  Serial.println(prox);

  Serial.print("sound=");
  Serial.print(sound);
  Serial.print(" dark=");
  Serial.print(dark);
  Serial.print(" moving=");
  Serial.print(moving);
  Serial.print(" near=");
  Serial.println(near);

  Serial.print("FINAL_LABEL=");
  Serial.println(label);
  Serial.println();

  delay(200);
}