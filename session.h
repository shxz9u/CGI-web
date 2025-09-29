#ifndef SESSION_H
#define SESSION_H

#include <stddef.h>

// 세션 저장 디렉토리 (웹 루트 하위에 노출)
#ifndef SESSION_DIR
#define SESSION_DIR "/tmp/sessions"
#endif

// 세션 유효 시간(초)
#ifndef SESSION_TTL
#define SESSION_TTL 3600
#endif

// 세션 쿠키 이름
#ifndef SESSION_COOKIE
#define SESSION_COOKIE "SID"
#endif

// 세션 함수
int session_create(const char *user, char *sid, size_t sidlen);
int session_validate(const char *sid, char *user_out, size_t user_len);
int session_destroy(const char *sid);

#endif
