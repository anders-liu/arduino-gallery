void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);
}

void loop() {
  int n = 0;

  if (n = Serial.available()) {
    for (int i = 0; i < n; i++) {
      int x = Serial.read();
      Serial1.write((uint8_t)x);
    }
  }

  if (n = Serial1.available()) {
    for (int i = 0; i < n; i++) {
      int x = Serial1.read();
      Serial.write((uint8_t)x);
    }
  }
}
