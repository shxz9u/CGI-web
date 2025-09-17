#ifndef SESSION_H
#define SESSION_H

#include <time.h>

#define SESS_DIR "../sessions"   // cgi-bin 기준에서 상대경로 조정 필요할 수 있음
#define SESS_TTL 3600            // 1시간

// 세션 생성: 사용자명으로 세션 파일 만들고 세션ID 반환
int session_create(const char *username, char *sid_out, size_t sid_len);

// 세션 검증: 유효하면 username_out에 사용자명 채움
int session_validate(const char *sid, char *username_out, size_t ulen);

// 세션 삭제
int session_destroy(const char *sid);

// 디렉토리 생성(존재해도 OK)
int ensure_session_dir(void);

#endif
