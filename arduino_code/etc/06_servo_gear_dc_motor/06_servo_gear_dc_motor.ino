/*
 * ========================================
 * 서보모터 + 기어모터 + DC모터 통합 예제
 * ========================================
 * 
 * 기능:
 * - 서보모터: 정밀 각도 제어 (0~180도)
 * - 기어모터: 감속비가 있는 DC모터 (속도 제어)
 * - DC모터: 일반 DC모터 (방향 + 속도 제어)
 * - Serial 명령으로 개별 제어
 * 
 * 하드웨어:
 * - 서보모터 (SG90 등)
 * - 기어모터 (L298N 드라이버)
 * - DC모터 (L298N 드라이버)
 * 
 * 작성일: 2026-01-30
 */

#include <Servo.h>

// ========================================
// 핀 정의
// ========================================
// 서보모터
#define SERVO_PIN       9     // 서보모터 PWM 핀

// 기어모터 (L298N 채널 A)
#define GEAR_IN1        2     // 방향 제어 1
#define GEAR_IN2        3     // 방향 제어 2
#define GEAR_ENA        5     // PWM 속도 제어 (ENA)

// DC모터 (L298N 채널 B)
#define DC_IN3          4     // 방향 제어 3
#define DC_IN4          7     // 방향 제어 4
#define DC_ENB          6     // PWM 속도 제어 (ENB)

// ========================================
// 상수 정의
// ========================================
#define SERVO_MIN_ANGLE   0     // 서보 최소 각도
#define SERVO_MAX_ANGLE   180   // 서보 최대 각도

#define MOTOR_SPEED_SLOW    100   // 느린 속도 (PWM)
#define MOTOR_SPEED_MEDIUM  150   // 보통 속도
#define MOTOR_SPEED_FAST    255   // 빠른 속도

// ========================================
// 전역 변수
// ========================================
Servo myServo;

int servoAngle = 90;          // 현재 서보 각도
int gearMotorSpeed = 0;       // 기어모터 속도
int dcMotorSpeed = 0;         // DC모터 속도

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  서보 + 기어모터 + DC모터 시스템");
  Serial.println("========================================");
  
  // 서보모터 초기화
  myServo.attach(SERVO_PIN);
  myServo.write(servoAngle);
  
  // 기어모터 핀 모드 설정
  pinMode(GEAR_IN1, OUTPUT);
  pinMode(GEAR_IN2, OUTPUT);
  pinMode(GEAR_ENA, OUTPUT);
  
  // DC모터 핀 모드 설정
  pinMode(DC_IN3, OUTPUT);
  pinMode(DC_IN4, OUTPUT);
  pinMode(DC_ENB, OUTPUT);
  
  // 초기 상태: 모든 모터 정지
  stopGearMotor();
  stopDCMotor();
  
  Serial.println("[초기화 완료]");
  Serial.println("\n명령어:");
  Serial.println("\n[서보모터]");
  Serial.println("  servo:각도 (0~180)");
  Serial.println("  예: servo:90");
  Serial.println("\n[기어모터]");
  Serial.println("  gear:forward:속도 (0~255)");
  Serial.println("  gear:backward:속도");
  Serial.println("  gear:stop");
  Serial.println("  예: gear:forward:150");
  Serial.println("\n[DC모터]");
  Serial.println("  dc:forward:속도 (0~255)");
  Serial.println("  dc:backward:속도");
  Serial.println("  dc:stop");
  Serial.println("  예: dc:backward:200");
  Serial.println("\n[통합 제어]");
  Serial.println("  all:stop - 모든 모터 정지");
  Serial.println("  demo - 데모 시연");
  Serial.println("========================================");
  Serial.println();
}

// ========================================
// 메인 루프
// ========================================
void loop() {
  // Serial 명령 처리
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    handleCommand(command);
  }
}

// ========================================
// 명령 처리 함수
// ========================================
void handleCommand(String cmd) {
  Serial.print("[명령 수신] ");
  Serial.println(cmd);
  
  // 서보모터 제어: servo:각도
  if (cmd.startsWith("servo:")) {
    int angle = cmd.substring(6).toInt();
    controlServo(angle);
    
  // 기어모터 제어: gear:방향:속도
  } else if (cmd.startsWith("gear:")) {
    handleGearMotorCommand(cmd);
    
  // DC모터 제어: dc:방향:속도
  } else if (cmd.startsWith("dc:")) {
    handleDCMotorCommand(cmd);
    
  // 모든 모터 정지
  } else if (cmd == "all:stop") {
    stopAllMotors();
    
  // 데모 시연
  } else if (cmd == "demo") {
    runDemo();
    
  } else {
    Serial.println("[에러] 알 수 없는 명령어");
  }
}

// ========================================
// 서보모터 제어 함수
// ========================================
void controlServo(int angle) {
  // 각도 범위 제한
  if (angle < SERVO_MIN_ANGLE) {
    angle = SERVO_MIN_ANGLE;
  }
  if (angle > SERVO_MAX_ANGLE) {
    angle = SERVO_MAX_ANGLE;
  }
  
  servoAngle = angle;
  myServo.write(servoAngle);
  
  Serial.print("[서보모터] 각도: ");
  Serial.print(servoAngle);
  Serial.println("°");
}

// ========================================
// 기어모터 명령 처리 함수
// ========================================
void handleGearMotorCommand(String cmd) {
  // gear:forward:속도
  if (cmd.startsWith("gear:forward:")) {
    int speed = cmd.substring(13).toInt();
    runGearMotorForward(speed);
    
  // gear:backward:속도
  } else if (cmd.startsWith("gear:backward:")) {
    int speed = cmd.substring(14).toInt();
    runGearMotorBackward(speed);
    
  // gear:stop
  } else if (cmd == "gear:stop") {
    stopGearMotor();
    
  } else {
    Serial.println("[에러] 잘못된 기어모터 명령어");
  }
}

// ========================================
// DC모터 명령 처리 함수
// ========================================
void handleDCMotorCommand(String cmd) {
  // dc:forward:속도
  if (cmd.startsWith("dc:forward:")) {
    int speed = cmd.substring(11).toInt();
    runDCMotorForward(speed);
    
  // dc:backward:속도
  } else if (cmd.startsWith("dc:backward:")) {
    int speed = cmd.substring(12).toInt();
    runDCMotorBackward(speed);
    
  // dc:stop
  } else if (cmd == "dc:stop") {
    stopDCMotor();
    
  } else {
    Serial.println("[에러] 잘못된 DC모터 명령어");
  }
}

// ========================================
// 기어모터 제어 함수
// ========================================
void runGearMotorForward(int speed) {
  // 속도 범위 제한
  speed = constrain(speed, 0, 255);
  
  digitalWrite(GEAR_IN1, HIGH);
  digitalWrite(GEAR_IN2, LOW);
  analogWrite(GEAR_ENA, speed);
  
  gearMotorSpeed = speed;
  
  Serial.print("[기어모터] 전진, 속도: ");
  Serial.println(speed);
}

void runGearMotorBackward(int speed) {
  speed = constrain(speed, 0, 255);
  
  digitalWrite(GEAR_IN1, LOW);
  digitalWrite(GEAR_IN2, HIGH);
  analogWrite(GEAR_ENA, speed);
  
  gearMotorSpeed = -speed;
  
  Serial.print("[기어모터] 후진, 속도: ");
  Serial.println(speed);
}

void stopGearMotor() {
  digitalWrite(GEAR_IN1, LOW);
  digitalWrite(GEAR_IN2, LOW);
  analogWrite(GEAR_ENA, 0);
  
  gearMotorSpeed = 0;
  
  Serial.println("[기어모터] 정지");
}

// ========================================
// DC모터 제어 함수
// ========================================
void runDCMotorForward(int speed) {
  speed = constrain(speed, 0, 255);
  
  digitalWrite(DC_IN3, HIGH);
  digitalWrite(DC_IN4, LOW);
  analogWrite(DC_ENB, speed);
  
  dcMotorSpeed = speed;
  
  Serial.print("[DC모터] 전진, 속도: ");
  Serial.println(speed);
}

void runDCMotorBackward(int speed) {
  speed = constrain(speed, 0, 255);
  
  digitalWrite(DC_IN3, LOW);
  digitalWrite(DC_IN4, HIGH);
  analogWrite(DC_ENB, speed);
  
  dcMotorSpeed = -speed;
  
  Serial.print("[DC모터] 후진, 속도: ");
  Serial.println(speed);
}

void stopDCMotor() {
  digitalWrite(DC_IN3, LOW);
  digitalWrite(DC_IN4, LOW);
  analogWrite(DC_ENB, 0);
  
  dcMotorSpeed = 0;
  
  Serial.println("[DC모터] 정지");
}

// ========================================
// 모든 모터 정지 함수
// ========================================
void stopAllMotors() {
  stopGearMotor();
  stopDCMotor();
  
  Serial.println("========================================");
  Serial.println("[전체 정지] 모든 모터 정지됨");
  Serial.println("========================================");
}

// ========================================
// 데모 시연 함수
// ========================================
void runDemo() {
  Serial.println("========================================");
  Serial.println("  데모 시연 시작");
  Serial.println("========================================");
  
  // 1. 서보모터 테스트
  Serial.println("\n[1단계] 서보모터 테스트");
  controlServo(0);
  delay(1000);
  controlServo(90);
  delay(1000);
  controlServo(180);
  delay(1000);
  controlServo(90);
  delay(1000);
  
  // 2. 기어모터 테스트
  Serial.println("\n[2단계] 기어모터 테스트");
  runGearMotorForward(MOTOR_SPEED_SLOW);
  delay(2000);
  runGearMotorForward(MOTOR_SPEED_FAST);
  delay(2000);
  stopGearMotor();
  delay(1000);
  runGearMotorBackward(MOTOR_SPEED_MEDIUM);
  delay(2000);
  stopGearMotor();
  delay(1000);
  
  // 3. DC모터 테스트
  Serial.println("\n[3단계] DC모터 테스트");
  runDCMotorForward(MOTOR_SPEED_SLOW);
  delay(2000);
  runDCMotorForward(MOTOR_SPEED_FAST);
  delay(2000);
  stopDCMotor();
  delay(1000);
  runDCMotorBackward(MOTOR_SPEED_MEDIUM);
  delay(2000);
  stopDCMotor();
  delay(1000);
  
  // 4. 통합 제어
  Serial.println("\n[4단계] 통합 제어");
  controlServo(45);
  runGearMotorForward(MOTOR_SPEED_MEDIUM);
  runDCMotorForward(MOTOR_SPEED_MEDIUM);
  delay(3000);
  
  stopAllMotors();
  controlServo(90);
  
  Serial.println("\n========================================");
  Serial.println("  데모 시연 완료");
  Serial.println("========================================");
}
