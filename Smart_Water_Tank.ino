const int TRIGGER_PIN = 12;
const int ECHO_PIN = 11;
const int RELAY_PIN = 8;

const float PUMP_START_DISTANCE = 12.50;
const float PUMP_STOP_DISTANCE = 3.0;

const unsigned long SENSOR_INTERVAL = 1000;
const unsigned long ECHO_TIMEOUT = 30000UL;

unsigned long previousMillis = 0;

bool pumpState = false; // Initially OFF

void pumpOn() {
  digitalWrite(RELAY_PIN, LOW); // Active-LOW relay ON
  pumpState = true;
  Serial.println("PUMP: ON");
}

void pumpOff() {
  digitalWrite(RELAY_PIN, HIGH); // Active-LOW relay OFF
  pumpState = false;
  Serial.println("PUMP: OFF");
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(100);

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(TRIGGER_PIN, LOW);
  digitalWrite(RELAY_PIN, HIGH); // Ensure pump starts OFF

  Serial.println("Automatic Water Tank System Started");
  Serial.println("Start pump at 60 cm; stop pump at 10 cm.");
}

void loop() {
  // Read commands from Python/PHP or Serial Monitor
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "PUMP_ON") {
      pumpOn();
    }
    else if (command == "PUMP_OFF") {
      pumpOff();
    }
    else if (command == "STATUS") {
      Serial.print("Pump Status: ");
      Serial.println(pumpState ? "ON" : "OFF");
    }
  }

  // Read the sensor every second
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= SENSOR_INTERVAL) {
    previousMillis = currentMillis;

    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);

    unsigned long duration =
        pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT);

    if (duration == 0) {
      Serial.println("Distance: INVALID - Check sensor");
      Serial.println("Pump state unchanged for safety");
    }
    else {
      float distance = duration * 0.0343 / 2.0;

      Serial.print("Distance: ");
      Serial.print(distance, 1);
      Serial.println(" cm");

      // Automatic pump control
      if (distance >= PUMP_START_DISTANCE) {
        if (!pumpState) {
          pumpOn();
        }
      }
      else if (distance <= PUMP_STOP_DISTANCE) {
        if (pumpState) {
          pumpOff();
        }
      }

      Serial.print("Pump Status: ");
      Serial.println(pumpState ? "ON" : "OFF");
      Serial.println("-------------------------");
    }
  }
}
