#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#define MAX_BODY 65536
#define MAX_KV 256

// 쿠키 이름 / 세션 저장 위치 / TTL(초)
#define SESSION_COOKIE "SID"

void print_http_header_html(void);
void print_http_header_html_no_cache(void);
void print_http_header_json(void);

size_t read_post_body(char *buf, size_t buflen);
void url_decode(char *s);
int parse_kv_from_body(const char *body, const char *key, char *out, size_t outlen);
int get_cookie(const char *cookie_header, const char *name, char *out, size_t outlen);
int gen_session_id(char *out, size_t outlen);
void html_escape(const char *in, char *out, size_t outlen);
void trim(char *s);

#endif
