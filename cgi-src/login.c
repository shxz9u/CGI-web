#include "util.h"
#include "session.h"
#include <stdio.h>
#include <string.h>

// 데모용 사용자 검증(실서비스라면 해시된 비밀번호/DB 사용)
static int verify_user(const char *id, const char *pw){
    // 예시: admin / pass1234
    return (strcmp(id, "admin")==0 && strcmp(pw, "pass1234")==0);
}

int main(void){
    char body[MAX_BODY]; 
    read_post_body(body, sizeof(body));

    char id[MAX_KV]={0}, pw[MAX_KV]={0};
    parse_kv_from_body(body, "id", id, sizeof(id));
    parse_kv_from_body(body, "pw", pw, sizeof(pw));

    if(!verify_user(id, pw)){
        print_http_header_html_no_cache();
        printf("<html><body>로그인 실패<br><a href=\"/html/login.html\">뒤로</a></body></html>");
        return 0;
    }

    char sid[65];
    if(!session_create(id, sid, sizeof(sid))){
        print_http_header_html_no_cache();
        printf("<html><body>서버 오류</body></html>");
        return 0;
    }

    // 쿠키 설정: HttpOnly; SameSite=Lax; Path=/
    printf("Status: 302 Found\r\n");
    printf("Set-Cookie: %s=%s; Path=/; HttpOnly; SameSite=Lax\r\n", SESSION_COOKIE, sid);
    // HTTPS면 ; Secure 추가
    printf("Location: /html/protected.html\r\n");
    printf("Cache-Control: no-store\r\n\r\n");
    return 0;
}
