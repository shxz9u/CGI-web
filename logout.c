#include "util.h"
#include "session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    const char *cookie = getenv("HTTP_COOKIE");
    char sid[256] = {0};

    if (cookie && get_cookie(cookie, SESSION_COOKIE, sid, sizeof(sid)))
    {
        session_destroy(sid); // 세션 파일 삭제
    }

    // 쿠키 무효화 + 리다이렉트
    printf("Status: 302 Found\r\n");
    printf("Set-Cookie: %s=deleted; Path=/; HttpOnly; SameSite=Lax; Max-Age=0\r\n", SESSION_COOKIE);
    printf("Location: /cgi-bin/login.cgi\r\n");
    printf("Cache-Control: no-store\r\n\r\n");

    return 0;
}
