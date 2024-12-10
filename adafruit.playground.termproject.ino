#include <PulseSensorPlayground.h>

#include <Adafruit_CircuitPlayground.h>

const int PULSE_SENSOR_PIN = A9;
const int NUM_LEDS = 10;
const int LOW_HR = 60;
const int MEDIUM_HR_LOW = 80;
const int MEDIUM_HR_HIGH = 99;
const int HIGH_HR = 100;

int heartRate = 0;
int rawPulseValue = 0;
unsigned long lastBeatTime = 0;
int beatCounter = 0;
const int BEAT_THRESHOLD = 550;
bool manualInputMode = false;

void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);
  Serial.println("Heart Rate Monitor with Breathing Exercises");
  Serial.println("Enter 'M' for manual input or 'S' for sensor mode");
}

void loop() {
  checkForModeChange();
  
  if (manualInputMode) {
    readManualInput();
  } else {
    readPulseSensor();
  }

  if (heartRate > 0) {
    displayHeartRate(heartRate);
    provideBreathingExercise(heartRate);
    heartRate = 0; // Reset for next reading
  }
}

void checkForModeChange() {
  if (Serial.available() > 0) {
    char input = Serial.read();
    if (input == 'M' || input == 'm') {
      manualInputMode = true;
      Serial.println("Switched to manual input mode. Enter heart rate:");
    } else if (input == 'S' || input == 's') {
      manualInputMode = false;
      Serial.println("Switched to sensor mode");
    }
  }
}

void readManualInput() {
  if (Serial.available() > 0) {
    int input = Serial.parseInt();
    if (input > 0) {
      heartRate = input;
      Serial.print("Manual heart rate input: ");
      Serial.println(heartRate);
    }
  }
}
 void readPulseSensor() {
   rawPulseValue = analogRead(PULSE_SENSOR_PIN);
   if (rawPulseValue > BEAT_THRESHOLD && millis() - lastBeatTime > 300) {
     beatCounter++;
     lastBeatTime = millis();
     Serial.print("Beat detected! Count: ");
     Serial.println(beatCounter);
  
     if (beatCounter == 10) {
       unsigned long tenBeatDuration = millis() - lastBeatTime;
       heartRate = (60000 / tenBeatDuration) * 10;
       beatCounter = 0;
     }
 }
}


void displayHeartRate(int rate) {
  Serial.print("Heart Rate: ");
  Serial.print(rate);
  Serial.println(" BPM");
  if (rate >= HIGH_HR) {
    displayStressLevel(255, 0, 0);  // Red for high heart rate (100 and above)
  } else if (rate >= MEDIUM_HR_LOW && rate <= MEDIUM_HR_HIGH) {
    displayStressLevel(255, 255, 0);  // Yellow for medium heart rate (80-99)
  } else if (rate < LOW_HR) {
    displayStressLevel(0, 255, 0);  // Green for low heart rate (below 60)
  } else {
    displayStressLevel(0, 0, 255);  // Blue for heart rate between 60-79
  }
}

void displayStressLevel(int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    CircuitPlayground.setPixelColor(i, r, g, b);
  }
}

void provideBreathingExercise(int rate) {
  int breathDuration;
  if (rate >= HIGH_HR) {
    breathDuration = 6000;  // 3 seconds for high stress
    Serial.println("Let's do some deep breathing to calm down.");
  } else if (rate >= MEDIUM_HR_LOW) {
    breathDuration = 4000;  // 4 seconds for medium stress
    Serial.println("Let's practice some relaxed breathing.");
  } else {
    breathDuration = 3000;  // 5 seconds for low stress
    Serial.println("Your heart rate is good. Let's maintain this calm state.");
  }
  for (int cycle = 0; cycle < 3; cycle++) {
    breatheIn(breathDuration);
    breatheOut(breathDuration);
  }
}

void breatheIn(int duration) {
  Serial.println("Breathe in...");
  for (int i = 0; i < NUM_LEDS; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 255);  // Blue for inhale
    CircuitPlayground.setPixelColor(NUM_LEDS - 1 - i, 0, 0, 255);
    delay(duration / NUM_LEDS);
  }
}

void breatheOut(int duration) {
  Serial.println("Breathe out...");
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    CircuitPlayground.setPixelColor(i, 0, 0, 0);  // Turn off LEDs for exhale
    CircuitPlayground.setPixelColor(NUM_LEDS - 1 - i, 0, 0, 0);
    delay(duration / NUM_LEDS);
  }
}