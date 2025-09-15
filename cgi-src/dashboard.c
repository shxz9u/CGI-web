#include <stdio.h>
int main(void)
{
    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
    printf("<h1>대시보드</h1>");
    printf("<ul>");
    printf("<li><a href=\"/web-cgi/ipdiag.html\">IP 진단</a></li>");
    printf("<li><a href=\"/web-cgi/box.html\">프린터 박스</a></li>");
    printf("<li><a href=\"/web-cgi/print.html\">출력 기능</a></li>");
    printf("</ul>");
    return 0;
}
