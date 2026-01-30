/*
 * ========================================
 * 8x8 LED Matrix 제어 예제 (MAX7219)
 * ========================================
 * 
 * 기능:
 * - 8x8 LED Matrix에 다양한 패턴 표시
 * - 숫자, 문자, 아이콘 애니메이션
 * - Serial 명령으로 패턴 변경
 * 
 * 하드웨어:
 * - 8x8 LED Matrix (MAX7219 드라이버)
 * - 5핀 연결: VCC, GND, DIN, CS, CLK
 * 
 * 작성일: 2026-01-30
 */

#include <LedControl.h>

// ========================================
// 핀 정의 (MAX7219 5핀 연결)
// ========================================
#define DIN_PIN   11    // Data In (MOSI)
#define CS_PIN    10    // Chip Select (SS)
#define CLK_PIN   13    // Clock (SCK)

// ========================================
// 상수 정의
// ========================================
#define NUM_DEVICES  1        // LED Matrix 개수
#define BRIGHTNESS   8        // 밝기 (0~15)

// ========================================
// 전역 변수
// ========================================
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICES);

// ========================================
// 패턴 정의 (8x8 비트맵)
// ========================================

// 숫자 0
byte pattern_0[8] = {
  B00111100,
  B01000010,
  B01000110,
  B01001010,
  B01010010,
  B01100010,
  B01000010,
  B00111100
};

// 숫자 1
byte pattern_1[8] = {
  B00011000,
  B00101000,
  B00001000,
  B00001000,
  B00001000,
  B00001000,
  B00001000,
  B00111110
};

// 하트 모양
byte pattern_heart[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};

// 웃는 얼굴
byte pattern_smile[8] = {
  B00111100,
  B01000010,
  B10100101,
  B10000001,
  B10100101,
  B10011001,
  B01000010,
  B00111100
};

// 화살표 (위)
byte pattern_arrow_up[8] = {
  B00011000,
  B00111100,
  B01111110,
  B11011011,
  B00011000,
  B00011000,
  B00011000,
  B00011000
};

// 체크 마크
byte pattern_check[8] = {
  B00000000,
  B00000001,
  B00000011,
  B00000110,
  B01001100,
  B11111000,
  B01110000,
  B00100000
};

// X 표시
byte pattern_x[8] = {
  B10000001,
  B01000010,
  B00100100,
  B00011000,
  B00011000,
  B00100100,
  B01000010,
  B10000001
};

// 전체 켜짐
byte pattern_full[8] = {
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B11111111
};

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  8x8 LED Matrix 제어 시스템");
  Serial.println("========================================");
  
  // LED Matrix 초기화
  lc.shutdown(0, false);           // 절전 모드 해제
  lc.setIntensity(0, BRIGHTNESS);  // 밝기 설정
  lc.clearDisplay(0);              // 화면 지우기
  
  Serial.println("[초기화 완료]");
  Serial.println("\n명령어:");
  Serial.println("  0 - 숫자 0");
  Serial.println("  1 - 숫자 1");
  Serial.println("  h - 하트");
  Serial.println("  s - 웃는 얼굴");
  Serial.println("  u - 화살표 위");
  Serial.println("  c - 체크 마크");
  Serial.println("  x - X 표시");
  Serial.println("  f - 전체 켜짐");
  Serial.println("  a - 자동 애니메이션");
  Serial.println("  clear - 화면 지우기");
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
}

// ========================================
// 명령 처리 함수
// ========================================
void handleCommand(String cmd) {
  Serial.print("[명령 수신] ");
  Serial.println(cmd);
  
  if (cmd == "0") {
    displayPattern(pattern_0, "숫자 0");
    
  } else if (cmd == "1") {
    displayPattern(pattern_1, "숫자 1");
    
  } else if (cmd == "h") {
    displayPattern(pattern_heart, "하트");
    
  } else if (cmd == "s") {
    displayPattern(pattern_smile, "웃는 얼굴");
    
  } else if (cmd == "u") {
    displayPattern(pattern_arrow_up, "화살표 위");
    
  } else if (cmd == "c") {
    displayPattern(pattern_check, "체크 마크");
    
  } else if (cmd == "x") {
    displayPattern(pattern_x, "X 표시");
    
  } else if (cmd == "f") {
    displayPattern(pattern_full, "전체 켜짐");
    
  } else if (cmd == "clear") {
    lc.clearDisplay(0);
    Serial.println("[실행] 화면 지우기");
    
  } else if (cmd == "a") {
    runAnimation();
    
  } else {
    Serial.println("[에러] 알 수 없는 명령어");
  }
}

// ========================================
// 패턴 표시 함수
// ========================================
void displayPattern(byte pattern[8], String name) {
  Serial.print("[표시] ");
  Serial.println(name);
  
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, pattern[row]);
  }
}

// ========================================
// 애니메이션 실행 함수
// ========================================
void runAnimation() {
  Serial.println("[실행] 자동 애니메이션 시작");
  
  // 1. 숫자 카운트다운
  displayPattern(pattern_1, "숫자 1");
  delay(1000);
  displayPattern(pattern_0, "숫자 0");
  delay(1000);
  
  // 2. 체크 마크
  displayPattern(pattern_check, "체크 마크");
  delay(1000);
  
  // 3. 하트 깜빡임
  for (int i = 0; i < 3; i++) {
    displayPattern(pattern_heart, "하트");
    delay(500);
    lc.clearDisplay(0);
    delay(500);
  }
  
  // 4. 웃는 얼굴
  displayPattern(pattern_smile, "웃는 얼굴");
  delay(1500);
  
  // 5. 화면 지우기
  lc.clearDisplay(0);
  
  Serial.println("[완료] 애니메이션 종료");
}

// ========================================
// 스크롤 텍스트 함수 (선택 사항)
// ========================================
void scrollText(String text) {
  // 추후 확장 가능: 텍스트 스크롤 기능
  Serial.println("[알림] 스크롤 기능은 추후 구현 예정");
}
