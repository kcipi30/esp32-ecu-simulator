#include <Bluepad32.h>

// ESP32 ECU Simulation using Xbox Controller (Bluepad32)
// - A button toggles engine ON/OFF
// - Right Trigger controls throttle (RPM)
// - Engine idle RPM is 800
// - Engine temperature increases with RPM
// - Cooling system turns ON above 100°C and OFF below 90°C

ControllerPtr myController;

// ===== Controller callbacks =====

// Called when a controller connects
void onConnectedController(ControllerPtr ctl) {
  myController = ctl;
  Serial.println("Controller connected");
}

// Called when a controller disconnects
void onDisconnectedController(ControllerPtr ctl) {
  if (myController == ctl) {
    myController = nullptr;
  }
  Serial.println("Controller disconnected");
}

// ===== Global variables =====

// Engine state
bool engineON = 0;

// A button state tracking for toggle logic
bool lastA = 0;
bool currentA = 0;

// Temperature model
float engineTemp = 0;
float ambientTemp = 0;

// RPM model
float rpmNorm = 0;
float rpm = 0;

// Serial input flag
bool ambientSet = 0;

// Cooling system state
bool coolingON = 0;

// ===== Cooling function =====
// Reduces engine temperature when cooling is active
float cooling(float temp) {
  temp = temp - 0.5;
  return temp;
}

// ===== Setup =====
// Initialize Serial communication and Bluetooth controller handling
void setup() {
  Serial.begin(115200);
  Serial.println("Start");

  // Reset Bluetooth pairing (used for stable reconnection during development)
    BP32.forgetBluetoothKeys();

  BP32.setup(&onConnectedController, &onDisconnectedController);
}

// ===== Main loop =====
void loop() {
  BP32.update();
  int RT;

  // Read ambient temperature once from Serial Monitor
  if (ambientSet == 0 && Serial.available() > 0) {
    ambientTemp = Serial.parseFloat();
    Serial.print("Ambient temp set to: ");
    Serial.println(ambientTemp);

    // Start engine temperature from ambient temperature
    engineTemp = ambientTemp;
    ambientSet = true;
  }

  if (myController && myController->isConnected()) {
    // ===== Controller input =====
    RT = myController->throttle();
    float throttle = RT / 1023.0;

    // Read A button state
    lastA = currentA;
    currentA = myController->a();

    // Toggle engine state when A button is pressed
    if (currentA == 1 && lastA == 0) {
      engineON = !engineON;
    }

    // ===== Engine OFF logic =====
    if (engineON == 0) {
      Serial.println("Engine OFF");

      rpm = 0;

      // Cool down engine towards ambient temperature
      if (engineTemp > ambientTemp) {
        engineTemp = engineTemp - 0.2;

        // Prevent temperature from going below ambient
        if (engineTemp < ambientTemp) {
          engineTemp = ambientTemp;
        }
      }
    }

    // ===== Engine ON logic =====
    else {
      Serial.println("Engine ON");

      // Calculate RPM from throttle input
      rpm = 800 + (6000 - 800) * throttle;

      // Normalize RPM between idle and max RPM
      rpmNorm = (rpm - 800) / 5200.0;

      // Activate cooling above 100°C
      if (engineTemp >= 100) {
        coolingON = 1;
        Serial.println("Cooling is ON");
      }

      // Cooling stays active until temperature drops below 90°C
      if (coolingON == 1) {
        engineTemp = cooling(engineTemp);

        if (engineTemp < 90) {
          coolingON = 0;
          Serial.println("Cooling is OFF");
        }
      }
      else {
        // Increase engine temperature based on normalized RPM
        engineTemp = engineTemp + rpmNorm * 0.5;
      }
    }

    // ===== Serial output =====
    Serial.print("RPM: ");
    Serial.print(rpm);
    Serial.print(" | Engine Temperature: ");
    Serial.println(engineTemp);

    delay(500);
  }
}