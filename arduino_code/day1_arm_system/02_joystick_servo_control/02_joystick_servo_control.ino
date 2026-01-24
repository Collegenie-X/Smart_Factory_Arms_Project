/**
 * 로봇 팔 - 2단계: 조이스틱 + 서보 모터 제어
 * 
 * 조이스틱으로 4개 서보 모터를 실시간 제어
 * 원본 robot_arm_control.ino를 단순화한 버전
 */

#include <Servo.h>

/* ===== 서보 설정 ===== */
Servo servo[4];
int pin[4] = {4, 5, 6, 7};

// 각도 범위 설정 (쉽게 수정 가능)
int minAngles[4] = {0, 70, 60, 5};     // 최소 각도: 베이스, 팔꿈치, 손목, 그립
int maxAngles[4] = {130, 120, 120, 50}; // 최대 각도: 베이스, 팔꿈치, 손목, 그립

int angles[4] = {90, 95, 90, 25};        // 현재 각도: 베이스, 팔꿈치, 손목, 그립(중간)
int prevAngles[4] = {90, 95, 90, 25};    // 이전 각도 (변경 감지용)

void setup() {
  Serial.begin(9600);
  Serial.println("조이스틱 제어 시작\n");
  
  // 각도 범위 출력
  Serial.println("각도 범위:");
  Serial.print("  베이스: ");
  Serial.print(minAngles[0]);
  Serial.print(" ~ ");
  Serial.println(maxAngles[0]);
  
  Serial.print("  팔꿈치: ");
  Serial.print(minAngles[1]);
  Serial.print(" ~ ");
  Serial.println(maxAngles[1]);
  
  Serial.print("  손목: ");
  Serial.print(minAngles[2]);
  Serial.print(" ~ ");
  Serial.println(maxAngles[2]);
  
  Serial.print("  그립: ");
  Serial.print(minAngles[3]);
  Serial.print(" ~ ");
  Serial.println(maxAngles[3]);
  
  Serial.println("\n초기 위치:");
  Serial.println("베이스:90 | 팔꿈치:90 | 손목:90 | 그립:5\n");
  
  // 서보 연결 및 초기화
  for (int i = 0; i < 4; i++) {
    servo[i].attach(pin[i]);
    servo[i].write(angles[i]);
  }
}

void loop() {
  // 조이스틱 값 읽기
  int val[4];
  val[0] = analogRead(14);  // A0 - 베이스
  val[1] = analogRead(15);  // A1 - 팔1
  val[2] = 1024 - analogRead(16);  // A2 - 팔2 (반대)
  val[3] = 1024 - analogRead(17);  // A3 - 그립 (반대)
  
  // 각 서보 제어
  for (int i = 0; i < 4; i++) {
    // 조이스틱이 위쪽 (증가)
    if (val[i] > 1000) {
      angles[i] += 1;
      
      // 최대값 제한
      if (angles[i] > maxAngles[i]) {
        angles[i] = maxAngles[i];
      }
      
      servo[i].write(angles[i]);
    }
    
    // 조이스틱이 아래쪽 (감소)
    else if (val[i] < 100) {
      angles[i] -= 1;
      
      // 최소값 제한
      if (angles[i] < minAngles[i]) {
        angles[i] = minAngles[i];
      }
      
      servo[i].write(angles[i]);
    }
  }
  
  // 변경된 서보만 출력
  const char* names[] = {"베이스", "팔꿈치", "손목", "그립"};
  
  for (int i = 0; i < 4; i++) {
    if (angles[i] != prevAngles[i]) {
      Serial.print(names[i]);
      Serial.print(": ");
      Serial.print(prevAngles[i]);
      Serial.print(" → ");
      Serial.println(angles[i]);
      prevAngles[i] = angles[i];
    }
  }
  
  delay(20);
}
