#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF (1024 * 1024 * 5)

int main(void)
{
    char *method = getenv("REQUEST_METHOD");

    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
    printf("<!DOCTYPE html><html lang='ko'><head>");
    printf("<meta charset='UTF-8'><title>출력 기능</title>");
    printf("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css' rel='stylesheet'>");
    printf("</head><body class='bg-light'>");

    // ✅ 상단 여백 확보 (mt-5)
    printf("<div class='container py-4 mt-5'>");
    printf("<div class='card shadow-sm'><div class='card-body'>");

    if (!method || strcmp(method, "GET") == 0)
    {
        printf("<h2 class='mb-3'>파일 출력</h2>");
        printf("<form method='post' action='/cgi-bin/print.cgi' enctype='multipart/form-data'>");
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
                fread(buf, 1, len, stdin);
                buf[len] = '\0';
                free(buf);
                printf("<div class='alert alert-success'>🖨️ 업로드 성공!</div>");
            }
            else
            {
                printf("<div class='alert alert-danger'>메모리 부족</div>");
            }
        }
    }

    printf("</div></div></div>");
    printf("</body></html>");
    return 0;
}
