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
    if (t[0] == '.' || t[0] == '-' || t[n - 1] == '.' || t[n - 1] == '-')
        return 0;
    return 1;
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
        if (*p == '+')
            out[i++] = ' ';
        else
            out[i++] = *p;
        p++;
    }
    out[i] = '\0';
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0); // 출력 버퍼링 끄기

    char *qs = getenv("QUERY_STRING");
    char target[MAX_TARGET + 1] = "";
    char cmd[MAX_CMD];
    FILE *fp;
    char buf[BUF_SIZE];

    int sent = 0, received = 0, lost = 0;
    int min_ms = -1, max_ms = -1, avg_ms = -1;

    // CGI 헤더
    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");

    // HTML 시작
    printf("<!DOCTYPE html><html lang='ko'><head>");
    printf("<meta charset='utf-8'><title>IP 진단</title>");
    printf("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css' rel='stylesheet'>");
    printf("</head><body class='container mt-5'>");

    printf("<h1 class='mb-4'>IP / 도메인 진단</h1>");
    printf("<form method='get' action='' class='mb-4'>");
    printf("<div class='input-group'>");
    printf("<input type='text' name='target' class='form-control' placeholder='예: 8.8.8.8 또는 naver.com'>");
    printf("<button type='submit' class='btn btn-primary'>진단</button>");
    printf("</div></form>");

    if (qs)
        extract_target(qs, target, sizeof(target));

    if (target[0] == '\0')
    {
        printf("<div class='alert alert-info'>대상을 입력하고 진단 버튼을 눌러주세요.</div>");
        printf("</body></html>");
        return 0;
    }

    if (!is_valid_target(target))
    {
        printf("<div class='alert alert-danger'>잘못된 입력입니다. 영문, 숫자, '.', '-' 만 허용됩니다.</div>");
        printf("</body></html>");
        return 0;
    }

    // Windows ping.exe 직접 실행 (cmd 창 안 뜸)
    snprintf(cmd, sizeof(cmd), "C:\\\\Windows\\\\System32\\\\ping.exe -n 4 %s", target);

    fp = _popen(cmd, "r");
    if (!fp)
    {
        printf("<div class='alert alert-danger'>ping 실행 실패</div>");
        printf("</body></html>");
        return 0;
    }

    printf("<h2 class='h5'>진단 대상: %s</h2>", target);
    printf("<pre class='bg-light p-3 rounded'>\n");

    while (fgets(buf, sizeof(buf), fp))
    {
        printf("%s", buf);

        // 패킷 요약 줄 파싱
        if (strstr(buf, "Packets:"))
        {
            sscanf(buf, "    Packets: Sent = %d, Received = %d, Lost = %d", &sent, &received, &lost);
        }
        // 시간 요약 줄 파싱
        if (strstr(buf, "Minimum ="))
        {
            sscanf(buf, "    Minimum = %dms, Maximum = %dms, Average = %dms", &min_ms, &max_ms, &avg_ms);
        }
    }
    printf("</pre>\n");
    _pclose(fp);

    // 요약 결과 출력
    printf("<div class='mt-3'>");
    if (sent > 0)
    {
        int loss_percent = (lost * 100) / sent;
        if (received > 0)
        {
            printf("<div class='alert alert-success'>✅ 대상 <strong>%s</strong> 에 연결 성공했습니다.<br>", target);
            printf("왕복 지연 시간 평균: %dms (최소 %dms, 최대 %dms)<br>", avg_ms, min_ms, max_ms);
            printf("손실률: %d%%</div>", loss_percent);
        }
        else
        {
            printf("<div class='alert alert-danger'>❌ 대상 <strong>%s</strong> 에 응답이 없습니다.<br>", target);
            printf("패킷 손실률: 100%% (네트워크 차단 또는 서버 응답 없음)</div>");
        }
    }
    else
    {
        printf("<div class='alert alert-warning'>진단 결과를 해석할 수 없습니다.</div>");
    }
    printf("</div>");

    printf("<p><a href='/index.html' class='btn btn-secondary mt-3'>대시보드로 돌아가기</a></p>");
    printf("</body></html>");
    return 0;
}
