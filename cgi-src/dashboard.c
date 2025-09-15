#include <stdio.h>

int main(void)
{
    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
    printf("<h1>대시보드</h1>");
    printf("<p>여기는 프린터 관리 현황 요약을 보여주는 CGI 페이지입니다.</p>");
    return 0;
}
