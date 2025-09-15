#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    const char *cl = getenv("CONTENT_LENGTH");
    int len = cl ? atoi(cl) : 0;
    char buf[1024] = {0};

    if (len > 0 && len < sizeof(buf))
    {
        fread(buf, 1, len, stdin);
    }

    char user[128] = {0}, pass[128] = {0};
    sscanf(buf, "username=%127[^&]&password=%127s", user, pass);

    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");

    if (strcmp(user, "admin") == 0 && strcmp(pass, "1234") == 0)
    {
        printf("<h1>로그인 성공</h1>");
        printf("<a href=\"/cgi-bin/dashboard.cgi\">대시보드 이동</a>");
    }
    else
    {
        printf("<h1>로그인 실패</h1>");
        printf("<a href=\"../web-cgi/login.html\">다시 시도</a>");
    }
    return 0;
}
