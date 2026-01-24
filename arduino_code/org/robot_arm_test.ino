#include <Servo.h>

Servo bottom;
Servo arm1;
Servo arm2;
Servo grip;

int angle = 0;

void setup() {
  bottom.attach(4);
  arm1.attach(5);
  arm2.attach(6);
  grip.attach(7);
}

void loop() {
  for(angle=45; angle<=90; angle++){
    bottom.write(angle);
    arm1.write(angle);
    arm2.write(angle);
    grip.write(angle);
    delay(100);
  }
}
