#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TARGET 128
#define MAX_CMD 256
#define BUF_SIZE 512

int is_valid_target(const char *t)
{
    size_t n = strlen(t);
    if (n == 0 || n > MAX_TARGET)
        return 0;
    for (size_t i = 0; i < n; i++)
    {
        char c = t[i];
        if (!(isalnum((unsigned char)c) || c == '.' || c == '-'))
            return 0;
    }
    return !(t[0] == '.' || t[0] == '-' || t[n - 1] == '.' || t[n - 1] == '-');
}

void extract_target(const char *qs, char *out, size_t outlen)
{
    out[0] = '\0';
    if (!qs)
        return;
    const char *p = strstr(qs, "target=");
    if (!p)
        return;
    p += 7;
    size_t i = 0;
    while (*p && *p != '&' && i + 1 < outlen)
    {
        out[i++] = (*p == '+' ? ' ' : *p);
        p++;
    }
    out[i] = '\0';
}

int main(void)
{
    char *qs = getenv("QUERY_STRING");
    char target[MAX_TARGET + 1] = "";
    char cmd[MAX_CMD];
    FILE *fp;
    char buf[BUF_SIZE];

    int transmitted = 0, received = 0;
    float loss = 0.0, min_ms = -1, avg_ms = -1, max_ms = -1, mdev = -1;

    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
    printf("<!DOCTYPE html><html lang='ko'><head>");
    printf("<meta charset='utf-8'><title>IP 진단</title>");
    printf("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css' rel='stylesheet'>");
    printf("</head><body class='bg-light'>");

    printf("<div class='container py-4 mt-5'>");
    printf("<div class='card shadow-sm'><div class='card-body'>");

    printf("<h2 class='mb-4'>IP / 도메인 진단</h2>");
    printf("<form method='get' action='' class='mb-4'>");
    printf("<div class='input-group'>");
    printf("<input type='text' name='target' class='form-control' placeholder='예: 8.8.8.8 또는 naver.com'>");
    printf("<button type='submit' class='btn btn-primary'>진단</button>");
    printf("</div></form>");

    if (qs)
        extract_target(qs, target, sizeof(target));

    if (target[0] == '\0')
    {
        printf("<div class='alert alert-info'>대상을 입력하세요.</div>");
        printf("</div></div></div></body></html>");
        return 0;
    }

    if (!is_valid_target(target))
    {
        printf("<div class='alert alert-danger'>잘못된 입력입니다.</div>");
        printf("</div></div></div></body></html>");
        return 0;
    }

    snprintf(cmd, sizeof(cmd), "/bin/ping -c 4 %s", target);
    fp = popen(cmd, "r");
    if (!fp)
    {
        printf("<div class='alert alert-danger'>ping 실행 실패</div>");
        printf("</div></div></div></body></html>");
        return 0;
    }

    printf("<h5>진단 대상: %s</h5>", target);
    printf("<pre class='bg-light p-3 rounded'>\n");
    while (fgets(buf, sizeof(buf), fp))
    {
        printf("%s", buf);
        if (strstr(buf, "packets transmitted"))
            sscanf(buf, "%d packets transmitted, %d received, %*d errors, %f%% packet loss",
                   &transmitted, &received, &loss);
        if (strstr(buf, "rtt min/avg/max"))
            sscanf(buf, "rtt min/avg/max/mdev = %f/%f/%f/%f ms",
                   &min_ms, &avg_ms, &max_ms, &mdev);
    }
    printf("</pre>");
    pclose(fp);

    if (received > 0)
        printf("<div class='alert alert-success mt-3'>✅ 연결 성공 (평균 %.2f ms, 손실률 %.1f%%)</div>", avg_ms, loss);
    else
        printf("<div class='alert alert-danger mt-3'>❌ 응답 없음</div>");

    printf("</div></div></div></body></html>");
    return 0;
}
