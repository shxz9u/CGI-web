#define _CRT_SECURE_NO_WARNINGS
#include "util.h"
#include "session.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 데모용 사용자 검증 (admin/pass1234만 허용)
static int verify_user(const char *id, const char *pw)
{
    return (strcmp(id, "admin") == 0 && strcmp(pw, "pass1234") == 0);
}

// POST + GET body 합치기
static void read_any_body(char *out, size_t outlen)
{
    char post[MAX_BODY] = {0};
    read_post_body(post, sizeof(post));

    const char *qs = getenv("QUERY_STRING");
    if (qs && qs[0])
        snprintf(out, outlen, "%s&%s", post, qs);
    else
        strncpy(out, post, outlen - 1);
}

int main(void)
{
    const char *method = getenv("REQUEST_METHOD");

    // GET → 로그인 폼
    if (!method || strcmp(method, "GET") == 0)
    {
        printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
        printf("<!DOCTYPE html><html lang='ko'><head>");
        printf("<meta charset='UTF-8'><title>로그인</title>");
        printf("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css' rel='stylesheet'>");
        printf("</head><body class='bg-light'>");

        printf("<div class='container py-4 mt-5'>");
        printf("<div class='card shadow-sm'><div class='card-body'>");

        printf("<h2 class='mb-4'>로그인</h2>");
        printf("<form method='post' action='/cgi-bin/login.cgi'>");
        printf("<div class='mb-3'><label class='form-label'>아이디</label>");
        printf("<input type='text' name='id' class='form-control' required></div>");
        printf("<div class='mb-3'><label class='form-label'>비밀번호</label>");
        printf("<input type='password' name='pw' class='form-control' required></div>");
        printf("<button type='submit' class='btn btn-primary w-100'>로그인</button>");
        printf("</form>");

        printf("</div></div></div></body></html>");
        return 0;
    }

    // POST → 로그인 처리
    if (strcmp(method, "POST") == 0)
    {
        char body[MAX_BODY] = {0};
        read_any_body(body, sizeof(body)); // POST+GET 합치기

        // 고의 취약: 작은 버퍼 + strcpy
        char id[16] = {0}, pw[16] = {0};
        parse_kv_from_body(body, "id", id, sizeof(id));
        parse_kv_from_body(body, "pw", pw, sizeof(pw));

        char small_id[8], small_pw[8];
        strcpy(small_id, id); // 크기 검증 없음
        strcpy(small_pw, pw);

        if (!verify_user(small_id, small_pw))
        {
            printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
            printf("<!DOCTYPE html><html lang='ko'><head>");
            printf("<meta charset='UTF-8'><title>로그인 실패</title>");
            printf("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css' rel='stylesheet'>");
            printf("</head><body class='bg-light'>");

            printf("<div class='container py-4 mt-5'>");
            printf("<div class='card shadow-sm'><div class='card-body'>");

            printf("<div class='alert alert-danger'>로그인 실패</div>");
            printf("<a href='/cgi-bin/login.cgi' class='btn btn-secondary'>다시 시도</a>");

            printf("</div></div></div></body></html>");
            return 0;
        }

        // 세션 픽세이션: sid 파라미터가 있으면 그대로 사용
        char sid_param[256] = {0};
        parse_kv_from_body(body, "sid", sid_param, sizeof(sid_param));

        char sid[256] = {0};
        if (sid_param[0])
        {
            strcpy(sid, sid_param); // 외부 sid 신뢰
        }
        else
        {
            if (!session_create(small_id, sid, sizeof(sid)))
            {
                printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
                printf("<div class='alert alert-danger'>세션 생성 실패</div>");
                return 0;
            }
        }

        // 쿠키 발급
        printf("Status: 302 Found\r\n");
        printf("Set-Cookie: %s=%s; Path=/\r\n", SESSION_COOKIE, sid);
        printf("Location: /cgi-bin/dashboard.cgi\r\n");
        printf("Cache-Control: no-store\r\n\r\n");
        return 0;
    }

    printf("Status: 405 Method Not Allowed\r\n");
    printf("Content-Type: text/plain; charset=utf-8\r\n\r\n");
    printf("허용되지 않은 요청 방식입니다.\n");
    return 0;
}
