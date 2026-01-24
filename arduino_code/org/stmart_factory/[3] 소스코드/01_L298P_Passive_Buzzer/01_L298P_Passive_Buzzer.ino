int Buz = 4;

void setup() {
  Serial.begin(9600);
  pinMode(Buz, OUTPUT);
}

void loop() {
  digitalWrite(Buz, HIGH);
  delay(1000);
  digitalWrite(Buz, LOW);
  delay(2000);

//  if (Serial.available()) {
//    char input = Serial.read();
//
//    if (input == 'h') {
//      digitalWrite(Buz, HIGH);
//      Serial.println("Buzzer ON");
//    }
//    else if (input == 'l') {
//      digitalWrite(Buz, LOW);
//      Serial.println("Buzzer OFF");
//    }
//  }
}
