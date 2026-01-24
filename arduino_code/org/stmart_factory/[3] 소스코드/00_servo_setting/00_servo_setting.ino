#include <Servo.h>                      
#define SERVO_PIN 9                     
Servo servo;                            

void setup() {
  Serial.begin(9600);                   
  servo.attach(SERVO_PIN);              
  servo.write(0);
  delay(1000);
}
void loop() {
}
