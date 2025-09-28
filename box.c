#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256

void print_table(const char *title, const char *status_filter)
{
    FILE *fp = fopen("queue.txt", "r");
    if (!fp)
    {
        printf("<p class='text-danger'>queue.txt를 열 수 없습니다.</p>");
        return;
    }

    char line[MAX_LINE];
    printf("<h5 class='mt-4'>%s</h5>", title);
    printf("<table class='table table-bordered table-sm'><thead>");
    printf("<tr><th>Job ID</th><th>사용자</th><th>문서</th><th>상태</th></tr></thead><tbody>");

    while (fgets(line, sizeof(line), fp))
    {
        char jobid[32], user[64], doc[128], status[32];
        if (sscanf(line, "%31[^,],%63[^,],%127[^,],%31s", jobid, user, doc, status) == 4)
        {
            if (!status_filter || strcmp(status, status_filter) == 0)
            {
                printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>",
                       jobid, user, doc, status);
            }
        }
    }

    printf("</tbody></table>");
    fclose(fp);
}

int main(void)
{
    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
    printf("<!DOCTYPE html><html lang='ko'><head>");
    printf("<meta charset='UTF-8'><title>프린터 박스</title>");
    printf("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css' rel='stylesheet'>");
    printf("</head><body class='bg-light'>");

    printf("<div class='container py-4 mt-5'>");
    printf("<div class='card shadow-sm'><div class='card-body'>");

    printf("<h2 class='mb-4'>프린터 박스</h2>");

    print_table("대기 중", "대기중");
    print_table("인쇄 중", "인쇄중");
    print_table("완료/취소됨", NULL);

    printf("</div></div></div></body></html>");
    return 0;
}
