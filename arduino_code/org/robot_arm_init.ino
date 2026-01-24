#include <Servo.h>

Servo bottom;
Servo arm1;
Servo arm2;
Servo grip;

void setup() {
  bottom.attach(4);
  arm1.attach(5);
  arm2.attach(6);
  grip.attach(7);

  bottom.write(90);
  arm1.write(90);
  arm2.write(90);
  grip.write(5);
}

void loop() {
}
