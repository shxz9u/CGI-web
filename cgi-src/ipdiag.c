#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    const char *qs = getenv("QUERY_STRING");
    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
    printf("<h1>IP 진단 결과</h1>");
    if (qs)
    {
        printf("<p>입력값: %s</p>", qs);
        printf("<p>(여기에 ping 결과를 나중에 붙일 수 있음)</p>");
    }
    else
    {
        printf("<p>IP가 입력되지 않았음</p>");
    }
    return 0;
}
