#pragma once

#define LAYER_STATE_8BIT
#define NO_ACTION_ONESHOT
#define NO_MUSIC_MODE

/* 부트로더 진입 키: ANSI Enter 스위치를 누른 채로 USB 연결 */
#define BOOTMAGIC_ROW    0
#define BOOTMAGIC_COLUMN 2

/* 탭 판정: 이보다 짧게 누르면 탭(점), 길면 홀드(선) */
#define MORSE_DASH_MS      200
/* 레이어 0: 마지막 탭 후 이 시간 동안 다음 탭이 없으면 모아둔 엔터 전송 */
#define ENTER_TAP_GAP_MS   200
/* 레이어 1: 신호 사이 공백이 이보다 길면 한 글자 확정 */
#define MORSE_CHAR_GAP_MS  400
/* 레이어 1: 글자 확정 후 이 시간까지 입력이 없으면 스페이스 전송 */
#define MORSE_WORD_GAP_MS  1200
/* 이 횟수만큼 연속 탭하면 레이어 전환 */
#define LAYER_TOGGLE_TAPS  6
