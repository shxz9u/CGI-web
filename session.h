#ifndef SESSION_H
#define SESSION_H
#define SESSION_DIR "/tmp/sessions"
#define SESSION_TTL 3600
#define SESSION_COOKIE "SID"

#include <stddef.h>

// 세션 함수
int session_create(const char *user, char *sid, size_t sidlen);
int session_validate(const char *sid, char *user_out, size_t user_len);
int session_destroy(const char *sid);

#endif
