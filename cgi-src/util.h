#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#define MAX_BODY 8192
#define MAX_COOKIE 4096
#define MAX_KV 256
#define SESSION_COOKIE "SID"

// 공통 출력 헤더
void print_http_header_html(void);
void print_http_header_html_no_cache(void);
void print_http_header_json(void);

// POST 본문 읽기 (application/x-www-form-urlencoded 가정)
size_t read_post_body(char *buf, size_t buflen);

// URL 디코딩
void url_decode(char *s);

// name=... 형태를 파싱해서 값 복사
int parse_kv_from_body(const char *body, const char *key, char *out, size_t outlen);

// 쿠키에서 특정 이름 검색
int get_cookie(const char *cookie_header, const char *name, char *out, size_t outlen);

// 난수 세션ID 생성(32바이트 → 64hex)
int gen_session_id(char *out, size_t outlen);

// 안전한 문자열 출력용 HTML 이스케이프(간단)
void html_escape(const char *in, char *out, size_t outlen);

// 트림
void trim(char *s);

// Windows: UTF-8 with BOM 방지용 안내(코드 아님)

#endif
//