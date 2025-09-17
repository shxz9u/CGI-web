#include "util.h"
#include "session.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(void){
    const char *cookie = getenv("HTTP_COOKIE");
    char sid[256]={0};
    if(cookie && get_cookie(cookie, SESSION_COOKIE, sid, sizeof(sid))){
        session_destroy(sid);
    }
    // 만료쿠키로 덮어쓰기
    printf("Status: 302 Found\r\n");
    printf("Set-Cookie: %s=deleted; Path=/; HttpOnly; SameSite=Lax; Expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n", SESSION_COOKIE);
    printf("Location: /html/login.html\r\n\r\n");
    return 0;
}
