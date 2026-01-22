/**
 * @file bluetooth_protocol.cpp
 * @brief 블루투스 통신 프로토콜 파싱 구현
 * @date 2026-01-22
 */

#include "bluetooth_protocol.h"

// ============================================================
// 생성자
// ============================================================
BluetoothProtocol::BluetoothProtocol(Stream* serial) 
  : btSerial(serial), bufferIndex(0) {
  memset(buffer, 0, sizeof(buffer));
}

// ============================================================
// 초기화
// ============================================================
bool BluetoothProtocol::begin() {
  DEBUG_PRINTLN("[Bluetooth] 초기화 완료");
  bufferIndex = 0;
  return true;
}

// ============================================================
// 데이터 수신 가능 여부
// ============================================================
bool BluetoothProtocol::available() {
  return btSerial->available() > 0;
}

// ============================================================
// 파라미터 파싱 (쉼표 구분)
// ============================================================
bool BluetoothProtocol::parseParams(const char* input, 
                                     uint8_t* params, 
                                     uint8_t& count) {
  count = 0;
  char tempBuffer[64];
  strncpy(tempBuffer, input, sizeof(tempBuffer) - 1);
  tempBuffer[sizeof(tempBuffer) - 1] = '\0';
  
  char* token = strtok(tempBuffer, ",");
  
  while (token != NULL && count < 4) {
    int value = atoi(token);
    
    // 각도 범위 검증
    if (value < 0 || value > 180) {
      DEBUG_PRINTLN("[Bluetooth] 오류: 파라미터 범위 초과");
      return false;
    }
    
    params[count++] = (uint8_t)value;
    token = strtok(NULL, ",");
  }
  
  return count > 0;
}

// ============================================================
// 명령 파싱 알고리즘
// ============================================================
bool BluetoothProtocol::parseCommand(Command& cmd) {
  // 초기화
  cmd.valid = false;
  cmd.type = '\0';
  memset(cmd.params, 0, sizeof(cmd.params));
  
  // 1. 버퍼 확인 (Early return)
  if (!btSerial->available()) {
    return false;
  }
  
  // 2. 명령 타입 읽기
  char cmdType = btSerial->read();
  
  // 공백 무시
  if (cmdType == ' ' || cmdType == '\n' || cmdType == '\r') {
    return false;
  }
  
  cmd.type = cmdType;
  
  VERBOSE_PRINT("[Bluetooth] 명령 수신: ");
  VERBOSE_PRINTLN(cmdType);
  
  // 3. 명령 타입별 처리
  switch (cmdType) {
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 홈 포지션 (H)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'H':
    case 'h':
      cmd.valid = true;
      DEBUG_PRINTLN("[Bluetooth] 명령: HOME");
      break;
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 집기 (P)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'P':
    case 'p':
      cmd.valid = true;
      DEBUG_PRINTLN("[Bluetooth] 명령: PICK");
      break;
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 왼쪽 놓기 (L)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'L':
    case 'l':
      cmd.valid = true;
      DEBUG_PRINTLN("[Bluetooth] 명령: PLACE_LEFT");
      break;
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 오른쪽 놓기 (R)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'R':
    case 'r':
      cmd.valid = true;
      DEBUG_PRINTLN("[Bluetooth] 명령: PLACE_RIGHT");
      break;
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 수동 이동 (M base,shoulder,elbow,gripper)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'M':
    case 'm': {
      // 파라미터 읽기 (최대 100ms 대기)
      unsigned long startTime = millis();
      String paramStr = "";
      
      while (millis() - startTime < 100) {
        if (btSerial->available()) {
          char c = btSerial->read();
          if (c == '\n' || c == '\r') {
            break;
          }
          paramStr += c;
        }
      }
      
      // 파라미터 파싱
      uint8_t count = 0;
      if (parseParams(paramStr.c_str(), cmd.params, count)) {
        if (count == 4) {
          cmd.valid = true;
          DEBUG_PRINT("[Bluetooth] 명령: MANUAL_MOVE [");
          DEBUG_PRINT(cmd.params[0]); DEBUG_PRINT(",");
          DEBUG_PRINT(cmd.params[1]); DEBUG_PRINT(",");
          DEBUG_PRINT(cmd.params[2]); DEBUG_PRINT(",");
          DEBUG_PRINT(cmd.params[3]); DEBUG_PRINTLN("]");
        } else {
          DEBUG_PRINTLN("[Bluetooth] 오류: 파라미터 개수 부족 (4개 필요)");
        }
      }
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 저장 (S slot)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'S':
    case 's': {
      // 슬롯 번호 읽기
      if (btSerial->available()) {
        int slot = btSerial->parseInt();
        if (slot >= 1 && slot <= 3) {
          cmd.params[0] = (uint8_t)slot;
          cmd.valid = true;
          DEBUG_PRINT("[Bluetooth] 명령: SAVE slot=");
          DEBUG_PRINTLN(slot);
        } else {
          DEBUG_PRINTLN("[Bluetooth] 오류: 슬롯 번호 범위 (1-3)");
        }
      }
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 불러오기 (L slot)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'D':  // Load 대신 D 사용 (L과 충돌 방지)
    case 'd': {
      // 슬롯 번호 읽기
      if (btSerial->available()) {
        int slot = btSerial->parseInt();
        if (slot >= 1 && slot <= 3) {
          cmd.params[0] = (uint8_t)slot;
          cmd.valid = true;
          DEBUG_PRINT("[Bluetooth] 명령: LOAD slot=");
          DEBUG_PRINTLN(slot);
        } else {
          DEBUG_PRINTLN("[Bluetooth] 오류: 슬롯 번호 범위 (1-3)");
        }
      }
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 상태 조회 (?)
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case '?':
      cmd.valid = true;
      DEBUG_PRINTLN("[Bluetooth] 명령: STATUS");
      break;
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // AI 명령 (A color zone) - Day 2에서 사용
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    case 'A':
    case 'a': {
      // "AI_RED_1" 형식 파싱
      String aiCmd = "";
      while (btSerial->available()) {
        char c = btSerial->read();
        if (c == '\n' || c == '\r') break;
        aiCmd += c;
      }
      
      // 색상 파싱
      if (aiCmd.indexOf("RED") >= 0) {
        cmd.params[0] = COLOR_RED;
      } else if (aiCmd.indexOf("BLUE") >= 0) {
        cmd.params[0] = COLOR_BLUE;
      } else if (aiCmd.indexOf("YELLOW") >= 0) {
        cmd.params[0] = COLOR_YELLOW;
      }
      
      // 구역 파싱
      if (aiCmd.indexOf("_1") >= 0) {
        cmd.params[1] = ZONE_1;
      } else if (aiCmd.indexOf("_2") >= 0) {
        cmd.params[1] = ZONE_2;
      } else if (aiCmd.indexOf("_3") >= 0) {
        cmd.params[1] = ZONE_3;
      }
      
      if (cmd.params[0] != 0 && cmd.params[1] != 0) {
        cmd.valid = true;
        DEBUG_PRINT("[Bluetooth] 명령: AI_PICK color=");
        DEBUG_PRINT(cmd.params[0]);
        DEBUG_PRINT(" zone=");
        DEBUG_PRINTLN(cmd.params[1]);
      }
      break;
    }
    
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    // 알 수 없는 명령
    // ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
    default:
      DEBUG_PRINT("[Bluetooth] 오류: 알 수 없는 명령 '");
      DEBUG_PRINT(cmdType);
      DEBUG_PRINTLN("'");
      sendResponse("ERROR:UNKNOWN_COMMAND");
      break;
  }
  
  return cmd.valid;
}

// ============================================================
// 응답 전송
// ============================================================
void BluetoothProtocol::sendResponse(const char* response) {
  btSerial->println(response);
  VERBOSE_PRINT("[Bluetooth] 응답: ");
  VERBOSE_PRINTLN(response);
}

// ============================================================
// 상태 JSON 전송
// ============================================================
void BluetoothProtocol::sendStatus(SystemState state, 
                                    const RobotPosition& pos, 
                                    const Statistics& stats) {
  // JSON 포맷 생성
  btSerial->print("{");
  
  // 상태
  btSerial->print("\"state\":\"");
  btSerial->print(getStateName(state));
  btSerial->print("\",");
  
  // 각도
  btSerial->print("\"angles\":[");
  btSerial->print(pos.base); btSerial->print(",");
  btSerial->print(pos.shoulder); btSerial->print(",");
  btSerial->print(pos.elbow); btSerial->print(",");
  btSerial->print(pos.gripper);
  btSerial->print("],");
  
  // 통계
  btSerial->print("\"stats\":{");
  btSerial->print("\"red\":"); btSerial->print(stats.redCount); btSerial->print(",");
  btSerial->print("\"blue\":"); btSerial->print(stats.blueCount); btSerial->print(",");
  btSerial->print("\"yellow\":"); btSerial->print(stats.yellowCount); btSerial->print(",");
  btSerial->print("\"total\":"); btSerial->print(stats.totalCount);
  btSerial->print("},");
  
  // 타임스탬프
  btSerial->print("\"timestamp\":");
  btSerial->print(millis());
  
  btSerial->println("}");
  
  VERBOSE_PRINTLN("[Bluetooth] 상태 JSON 전송 완료");
}
