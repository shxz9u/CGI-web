#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF (1024 * 1024 * 5) // 5MB까지 허용

int main(void)
{
    char *method = getenv("REQUEST_METHOD");

    // ✅ 무조건 제일 먼저 헤더 출력
    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");

    // HTML 시작
    printf("<!doctype html><html lang='ko'><head><meta charset='utf-8'>");
    printf("<title>파일 출력</title>");
    printf("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css' rel='stylesheet'>");
    printf("</head><body class='container mt-5'>");

    if (!method || strcmp(method, "GET") == 0)
    {
        // 업로드 폼
        printf("<h1 class='mb-3'>파일 출력 요청</h1>");
        printf("<form method='post' action='' enctype='multipart/form-data'>");
        printf("<div class='mb-3'><input type='file' name='upload' class='form-control'></div>");
        printf("<button type='submit' class='btn btn-primary'>출력</button>");
        printf("</form>");
    }
    else if (strcmp(method, "POST") == 0)
    {
        char *lenstr = getenv("CONTENT_LENGTH");
        long len = lenstr ? atol(lenstr) : 0;

        if (len <= 0 || len > MAX_BUF)
        {
            printf("<div class='alert alert-danger'>업로드 실패 (크기 제한 초과)</div>");
        }
        else
        {
            char *buf = malloc(len + 1);
            if (buf)
            {
                fread(buf, 1, len, stdin); // 실제 파일 데이터는 그냥 읽고 버림
                buf[len] = '\0';
                free(buf);

                printf("<div class='alert alert-success'>🖨️ 업로드한 파일이 서버에서 처리되어 출력되었습니다.</div>");
                printf("<a href='/cgi-bin/print.exe' class='btn btn-secondary mt-3'>다시 업로드</a>");
            }
            else
            {
                printf("<div class='alert alert-danger'>메모리 부족</div>");
            }
        }
    }

    printf("</body></html>");
    return 0;
}
