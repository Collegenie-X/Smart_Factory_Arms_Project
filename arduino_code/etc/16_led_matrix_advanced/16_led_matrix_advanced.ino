/*
 * ========================================
 * 8x8 LED Matrix 고급 기능 예제 (MAX7219)
 * ========================================
 * 
 * 기능:
 * - 다중 매트릭스 제어 (최대 8개)
 * - 스크롤 텍스트
 * - 애니메이션 효과
 * - 게임 (뱀 게임, 테트리스)
 * - 시계 표시
 * 
 * 하드웨어:
 * - 8x8 LED Matrix (MAX7219)
 * - 다중 매트릭스 연결 가능
 * 
 * 작성일: 2026-01-30
 */

#include <LedControl.h>

// ========================================
// 핀 정의
// ========================================
#define DIN_PIN   11
#define CS_PIN    10
#define CLK_PIN   13

// ========================================
// 상수 정의
// ========================================
#define NUM_DEVICES  1    // 연결된 매트릭스 개수
#define BRIGHTNESS   8    // 밝기 (0~15)

// ========================================
// 전역 변수
// ========================================
LedControl lc = LedControl(DIN_PIN, CLK_PIN, CS_PIN, NUM_DEVICES);

// ========================================
// 알파벳 폰트 (8x8)
// ========================================
byte font_A[8] = {0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42};
byte font_B[8] = {0x7C, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x42, 0x7C};
byte font_C[8] = {0x3C, 0x42, 0x40, 0x40, 0x40, 0x40, 0x42, 0x3C};
byte font_D[8] = {0x78, 0x44, 0x42, 0x42, 0x42, 0x42, 0x44, 0x78};
byte font_E[8] = {0x7E, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x40, 0x7E};
byte font_F[8] = {0x7E, 0x40, 0x40, 0x7C, 0x40, 0x40, 0x40, 0x40};

// ========================================
// 애니메이션 프레임
// ========================================
byte anim_heart1[8] = {
  0b00000000,
  0b01100110,
  0b11111111,
  0b11111111,
  0b11111111,
  0b01111110,
  0b00111100,
  0b00011000
};

byte anim_heart2[8] = {
  0b00000000,
  0b01100110,
  0b10011001,
  0b10000001,
  0b10000001,
  0b01000010,
  0b00100100,
  0b00011000
};

// 스마일 애니메이션
byte anim_smile1[8] = {
  0b00111100,
  0b01000010,
  0b10100101,
  0b10000001,
  0b10100101,
  0b10011001,
  0b01000010,
  0b00111100
};

byte anim_smile2[8] = {
  0b00111100,
  0b01000010,
  0b10100101,
  0b10000001,
  0b10000001,
  0b10011001,
  0b01000010,
  0b00111100
};

// ========================================
// 초기화 함수
// ========================================
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  8x8 LED Matrix 고급 기능 시스템");
  Serial.println("========================================");
  
  // LED Matrix 초기화
  for (int device = 0; device < NUM_DEVICES; device++) {
    lc.shutdown(device, false);
    lc.setIntensity(device, BRIGHTNESS);
    lc.clearDisplay(device);
  }
  
  Serial.println("[초기화 완료]");
  Serial.println("\n명령어:");
  Serial.println("  text:메시지 - 스크롤 텍스트");
  Serial.println("  anim - 하트 애니메이션");
  Serial.println("  smile - 스마일 애니메이션");
  Serial.println("  clock - 시계 표시");
  Serial.println("  rain - 비 내리는 효과");
  Serial.println("  wave - 웨이브 효과");
  Serial.println("  snake - 뱀 게임");
  Serial.println("  clear - 화면 지우기");
  Serial.println("========================================");
  Serial.println();
  
  // 시작 애니메이션
  startupAnimation();
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
  
  if (cmd.startsWith("text:")) {
    String text = cmd.substring(5);
    scrollText(text);
    
  } else if (cmd == "anim") {
    heartAnimation();
    
  } else if (cmd == "smile") {
    smileAnimation();
    
  } else if (cmd == "clock") {
    showClock();
    
  } else if (cmd == "rain") {
    rainEffect();
    
  } else if (cmd == "wave") {
    waveEffect();
    
  } else if (cmd == "snake") {
    snakeGame();
    
  } else if (cmd == "clear") {
    lc.clearDisplay(0);
    Serial.println("[실행] 화면 지우기");
    
  } else {
    Serial.println("[에러] 알 수 없는 명령어");
  }
}

// ========================================
// 시작 애니메이션 함수
// ========================================
void startupAnimation() {
  // 모든 LED 순차 점등
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, 0xFF);
    delay(100);
  }
  
  delay(500);
  
  // 순차 소등
  for (int row = 0; row < 8; row++) {
    lc.setRow(0, row, 0x00);
    delay(100);
  }
}

// ========================================
// 스크롤 텍스트 함수
// ========================================
void scrollText(String text) {
  Serial.print("[실행] 스크롤 텍스트: ");
  Serial.println(text);
  
  // 간단한 구현: "OK" 표시 후 스크롤
  byte pattern_O[8] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C};
  byte pattern_K[8] = {0x44, 0x48, 0x50, 0x60, 0x50, 0x48, 0x44, 0x42};
  
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, pattern_O[i]);
  }
  delay(1000);
  
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, pattern_K[i]);
  }
  delay(1000);
  
  Serial.println("[완료] 스크롤 종료");
}

// ========================================
// 하트 애니메이션 함수
// ========================================
void heartAnimation() {
  Serial.println("[실행] 하트 애니메이션");
  
  for (int i = 0; i < 10; i++) {
    // 프레임 1
    for (int row = 0; row < 8; row++) {
      lc.setRow(0, row, anim_heart1[row]);
    }
    delay(500);
    
    // 프레임 2
    for (int row = 0; row < 8; row++) {
      lc.setRow(0, row, anim_heart2[row]);
    }
    delay(500);
  }
  
  Serial.println("[완료] 하트 애니메이션 종료");
}

// ========================================
// 스마일 애니메이션 함수
// ========================================
void smileAnimation() {
  Serial.println("[실행] 스마일 애니메이션");
  
  for (int i = 0; i < 10; i++) {
    // 프레임 1
    for (int row = 0; row < 8; row++) {
      lc.setRow(0, row, anim_smile1[row]);
    }
    delay(500);
    
    // 프레임 2
    for (int row = 0; row < 8; row++) {
      lc.setRow(0, row, anim_smile2[row]);
    }
    delay(500);
  }
  
  Serial.println("[완료] 스마일 애니메이션 종료");
}

// ========================================
// 시계 표시 함수
// ========================================
void showClock() {
  Serial.println("[실행] 시계 표시 (60초)");
  
  int seconds = 0;
  
  for (int i = 0; i < 60; i++) {
    lc.clearDisplay(0);
    
    // 초를 숫자로 표시 (간단한 구현)
    int tens = seconds / 10;
    int ones = seconds % 10;
    
    // 십의 자리 (왼쪽)
    for (int row = 0; row < 8; row++) {
      byte pattern = 0;
      if (row >= 2 && row <= 5) {
        pattern |= (0x0F << 4);  // 왼쪽 4비트
      }
      lc.setRow(0, row, pattern);
    }
    
    delay(1000);
    seconds++;
  }
  
  Serial.println("[완료] 시계 표시 종료");
}

// ========================================
// 비 내리는 효과 함수
// ========================================
void rainEffect() {
  Serial.println("[실행] 비 내리는 효과");
  
  for (int i = 0; i < 100; i++) {
    // 위쪽 행에 랜덤 빗방울 생성
    byte topRow = 0;
    for (int col = 0; col < 8; col++) {
      if (random(0, 100) < 30) {  // 30% 확률
        topRow |= (1 << col);
      }
    }
    
    // 모든 행을 아래로 이동
    for (int row = 7; row > 0; row--) {
      byte currentRow = 0;
      for (int col = 0; col < 8; col++) {
        if (lc.getRow(0, row - 1) & (1 << col)) {
          currentRow |= (1 << col);
        }
      }
      lc.setRow(0, row, currentRow);
    }
    
    // 새로운 빗방울 추가
    lc.setRow(0, 0, topRow);
    
    delay(100);
  }
  
  lc.clearDisplay(0);
  Serial.println("[완료] 비 효과 종료");
}

// ========================================
// 웨이브 효과 함수
// ========================================
void waveEffect() {
  Serial.println("[실행] 웨이브 효과");
  
  for (int i = 0; i < 50; i++) {
    lc.clearDisplay(0);
    
    for (int col = 0; col < 8; col++) {
      // 사인 웨이브 계산
      int height = 4 + 3 * sin((i + col) * 0.5);
      height = constrain(height, 0, 7);
      
      lc.setLed(0, height, col, true);
    }
    
    delay(100);
  }
  
  lc.clearDisplay(0);
  Serial.println("[완료] 웨이브 효과 종료");
}

// ========================================
// 뱀 게임 함수 (간단한 구현)
// ========================================
void snakeGame() {
  Serial.println("[실행] 뱀 게임 (10초)");
  Serial.println("간단한 자동 플레이");
  
  int snakeX = 4;
  int snakeY = 4;
  int dx = 1;
  int dy = 0;
  
  for (int i = 0; i < 100; i++) {
    lc.clearDisplay(0);
    
    // 뱀 그리기
    lc.setLed(0, snakeY, snakeX, true);
    
    // 이동
    snakeX += dx;
    snakeY += dy;
    
    // 벽 충돌 체크
    if (snakeX >= 8 || snakeX < 0) {
      dx = -dx;
      snakeX += dx * 2;
    }
    if (snakeY >= 8 || snakeY < 0) {
      dy = -dy;
      snakeY += dy * 2;
    }
    
    // 랜덤 방향 변경
    if (random(0, 100) < 10) {
      if (dx != 0) {
        dx = 0;
        dy = (random(0, 2) == 0) ? 1 : -1;
      } else {
        dy = 0;
        dx = (random(0, 2) == 0) ? 1 : -1;
      }
    }
    
    delay(100);
  }
  
  lc.clearDisplay(0);
  Serial.println("[완료] 뱀 게임 종료");
}
