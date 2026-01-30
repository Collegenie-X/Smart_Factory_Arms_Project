/*
 * ========================================
 * LCD1602 I2C 제어 예제
 * ========================================
 * 
 * 기능:
 * - 16x2 LCD에 텍스트 표시
 * - I2C 통신으로 간단한 배선
 * - Serial 명령으로 텍스트 변경
 * - 실시간 시간 표시 (초 단위)
 * 
 * 하드웨어:
 * - LCD1602 I2C 모듈
 * - 4핀 연결: VCC, GND, SDA, SCL
 * 
 * 작성일: 2026-01-30
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ========================================
// I2C 주소 정의
// ========================================
// 일반적인 I2C 주소: 0x27 또는 0x3F
// 주소를 모르면 I2C 스캐너로 확인 필요
#define LCD_ADDRESS  0x27

// ========================================
// LCD 크기 정의
// ========================================
#define LCD_COLS  16    // 열 (가로)
#define LCD_ROWS  2     // 행 (세로)

// ========================================
// 전역 변수
// ========================================
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

unsigned long previousMillis = 0;
unsigned long elapsedSeconds = 0;

String line1Text = "Smart Factory";
String line2Text = "Ready...";

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  LCD1602 I2C 제어 시스템");
  Serial.println("========================================");
  
  // LCD 초기화
  lcd.init();
  lcd.backlight();  // 백라이트 켜기
  
  // 초기 메시지 표시
  lcd.clear();
  lcd.setCursor(0, 0);  // 첫 번째 줄
  lcd.print(line1Text);
  lcd.setCursor(0, 1);  // 두 번째 줄
  lcd.print(line2Text);
  
  Serial.println("[초기화 완료]");
  Serial.println("\n명령어:");
  Serial.println("  line1:텍스트 - 첫 번째 줄 변경");
  Serial.println("  line2:텍스트 - 두 번째 줄 변경");
  Serial.println("  clear - 화면 지우기");
  Serial.println("  time - 시간 표시 모드");
  Serial.println("  backlight:on - 백라이트 켜기");
  Serial.println("  backlight:off - 백라이트 끄기");
  Serial.println("\n예시:");
  Serial.println("  line1:Hello World");
  Serial.println("  line2:Arduino LCD");
  Serial.println("========================================");
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
  
  // 시간 업데이트 (1초마다)
  updateTime();
}

// ========================================
// 명령 처리 함수
// ========================================
void handleCommand(String cmd) {
  Serial.print("[명령 수신] ");
  Serial.println(cmd);
  
  // line1:텍스트
  if (cmd.startsWith("line1:")) {
    String text = cmd.substring(6);
    updateLine1(text);
    
  // line2:텍스트
  } else if (cmd.startsWith("line2:")) {
    String text = cmd.substring(6);
    updateLine2(text);
    
  // clear
  } else if (cmd == "clear") {
    lcd.clear();
    line1Text = "";
    line2Text = "";
    Serial.println("[실행] 화면 지우기");
    
  // time
  } else if (cmd == "time") {
    displayTimeMode();
    
  // backlight:on
  } else if (cmd == "backlight:on") {
    lcd.backlight();
    Serial.println("[실행] 백라이트 켜기");
    
  // backlight:off
  } else if (cmd == "backlight:off") {
    lcd.noBacklight();
    Serial.println("[실행] 백라이트 끄기");
    
  } else {
    Serial.println("[에러] 알 수 없는 명령어");
  }
}

// ========================================
// 첫 번째 줄 업데이트 함수
// ========================================
void updateLine1(String text) {
  // 16자 제한
  if (text.length() > LCD_COLS) {
    text = text.substring(0, LCD_COLS);
  }
  
  line1Text = text;
  
  lcd.setCursor(0, 0);
  lcd.print("                ");  // 기존 텍스트 지우기
  lcd.setCursor(0, 0);
  lcd.print(text);
  
  Serial.print("[업데이트] 첫 번째 줄: ");
  Serial.println(text);
}

// ========================================
// 두 번째 줄 업데이트 함수
// ========================================
void updateLine2(String text) {
  // 16자 제한
  if (text.length() > LCD_COLS) {
    text = text.substring(0, LCD_COLS);
  }
  
  line2Text = text;
  
  lcd.setCursor(0, 1);
  lcd.print("                ");  // 기존 텍스트 지우기
  lcd.setCursor(0, 1);
  lcd.print(text);
  
  Serial.print("[업데이트] 두 번째 줄: ");
  Serial.println(text);
}

// ========================================
// 시간 표시 모드 함수
// ========================================
void displayTimeMode() {
  Serial.println("[실행] 시간 표시 모드");
  
  elapsedSeconds = 0;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Elapsed Time:");
  
  // 10초 동안 시간 표시
  for (int i = 0; i < 10; i++) {
    lcd.setCursor(0, 1);
    lcd.print("                ");  // 지우기
    lcd.setCursor(0, 1);
    lcd.print(i);
    lcd.print(" seconds");
    
    delay(1000);
  }
  
  // 원래 텍스트로 복원
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1Text);
  lcd.setCursor(0, 1);
  lcd.print(line2Text);
  
  Serial.println("[완료] 시간 표시 종료");
}

// ========================================
// 시간 업데이트 함수 (비동기)
// ========================================
void updateTime() {
  unsigned long currentMillis = millis();
  
  // 1초마다 업데이트
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    elapsedSeconds++;
    
    // 시간 표시가 활성화된 경우에만 업데이트
    // (추후 확장 가능)
  }
}

// ========================================
// I2C 주소 스캔 함수 (디버깅용)
// ========================================
void scanI2C() {
  Serial.println("[I2C 스캔 시작]");
  
  byte error, address;
  int deviceCount = 0;
  
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C 장치 발견: 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      deviceCount++;
    }
  }
  
  if (deviceCount == 0) {
    Serial.println("[에러] I2C 장치를 찾을 수 없습니다");
  } else {
    Serial.print("[완료] ");
    Serial.print(deviceCount);
    Serial.println("개의 I2C 장치 발견");
  }
}
