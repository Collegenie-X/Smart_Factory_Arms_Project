/*
      조이스틱을 이용해 로봇팔 조작하기
*/
#include <Servo.h>
#include <Wire.h>
 
Servo bottom, arm1, arm2, grip;
Servo servo[4] = {bottom, arm1, arm2, grip};
 
int preVal[4] = {90, 90, 90, 5};
int pin[4] = {4, 5, 6, 7};
 
void setup() {
  for (int i = 0; i < 4; i++) {
    servo[i].attach(pin[i]);
  }
  Serial.begin(9600);
 
  for (int i = 0; i < 4; i++) {
    servo[i].write(preVal[i]);
  }
}
 
void loop() {
  int Val[4];
  for (int i = 0; i < 4; i++) {
    Val[i] = analogRead(14 + i);
 
    // 조이스틱 반대방향으로 움직이기(arm1, arm2)
    if ( i == 2 || i == 3) {
      Val[i] = 1024 - Val[i];
    }
    
    moveServo(i, Val[i]);
  }
  delay(20);
}
 
 
void moveServo(byte num, int joy) {
  if (joy > 1000) {
    preVal[num] += 1;
    if (num == 0) {
      if (preVal[num] > 130) {
        preVal[num] = 130;
      }
    }
    else if (num == 1) {
      if (preVal[num] > 120) {
        preVal[num] = 120;
      }
    }
    else if (num == 2) {
      if (preVal[num] > 120) {
        preVal[num] = 120;
      }
    }
    else if (num == 3) {
      if (preVal[num] > 50) {
        preVal[num] = 50;
      }
    }
  }
 
  else if (joy < 100) {
    preVal[num] -= 1;
    if (num == 0) {
      if (preVal[num] < 50) {
        preVal[num] = 50;
      }
    }
    else if (num == 1) {
      if (preVal[num] < 70) {
        preVal[num] = 70;
      }
    }
    else if (num == 2) {
      if (preVal[num] < 60) {
        preVal[num] = 60;
      }
    }
    else if (num == 3) {
      if (preVal[num] <= 5) {
        preVal[num] = 5;
      }
    }
  }
  servo[num].write(preVal[num]);
}
