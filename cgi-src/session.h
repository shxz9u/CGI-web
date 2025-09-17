#ifndef SESSION_H
#define SESSION_H

#include <stddef.h>

// 세션 저장 디렉토리(없으면 자동 생성)
#ifndef SESSION_DIR
#define SESSION_DIR "C:\\\\xampp\\\\tmp\\\\sessions"
#endif

// 세션 유효 시간(초)
#ifndef SESSION_TTL
#define SESSION_TTL 3600
#endif

// 세션 생성: user → sid(hex64)
int session_create(const char *user, char *sid, size_t sidlen);

// 검증: sid → user (성공 시 1, 실패 0)
int session_validate(const char *sid, char *user_out, size_t user_len);

// 파기: sid 삭제
int session_destroy(const char *sid);

#endif
