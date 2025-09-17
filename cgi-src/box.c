#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256

void print_table_header(const char *title, const char *extra)
{
    printf("<h2>%s</h2>", title);
    printf("<table class='table table-sm table-bordered'>");
    printf("<thead><tr><th>Job ID</th><th>사용자</th><th>문서</th><th>상태</th></tr></thead><tbody>");
}

void print_table_footer()
{
    printf("</tbody></table>");
}

int main(void)
{
    FILE *fp;
    char line[MAX_LINE];

    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
    printf("<!DOCTYPE html><html lang=\"ko\"><head>");
    printf("<meta charset=\"UTF-8\"><title>프린터 박스</title>");
    printf("<link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css' rel='stylesheet'>");
    printf("</head><body class='container mt-5'>");

    printf("<h1 class='mb-4 text-center'>프린터 박스</h1>");

    // 파일 열기
    fp = fopen("queue.txt", "r");
    if (!fp)
    {
        printf("<p><strong>에러:</strong> queue.txt를 열 수 없습니다.</p>");
        printf("</body></html>");
        return 0;
    }

    // 대기 중
    print_table_header("대기 중", "");
    rewind(fp);
    while (fgets(line, sizeof(line), fp))
    {
        char jobid[32], user[64], doc[128], status[32];
        if (sscanf(line, "%31[^,],%63[^,],%127[^,],%31s", jobid, user, doc, status) == 4)
        {
            if (strcmp(status, "대기중") == 0)
            {
                printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>",
                       jobid, user, doc, status);
            }
        }
    }
    print_table_footer();

    // 인쇄 중
    print_table_header("인쇄 중", "");
    rewind(fp);
    while (fgets(line, sizeof(line), fp))
    {
        char jobid[32], user[64], doc[128], status[32];
        if (sscanf(line, "%31[^,],%63[^,],%127[^,],%31s", jobid, user, doc, status) == 4)
        {
            if (strcmp(status, "인쇄중") == 0)
            {
                printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>",
                       jobid, user, doc, status);
            }
        }
    }
    print_table_footer();

    // 완료됨 / 취소됨
    print_table_header("완료됨", "");
    rewind(fp);
    while (fgets(line, sizeof(line), fp))
    {
        char jobid[32], user[64], doc[128], status[32];
        if (sscanf(line, "%31[^,],%63[^,],%127[^,],%31s", jobid, user, doc, status) == 4)
        {
            if (strcmp(status, "완료") == 0 || strcmp(status, "취소됨") == 0)
            {
                printf("<tr><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>",
                       jobid, user, doc, status);
            }
        }
    }
    print_table_footer();

    fclose(fp);

    printf("<p class='mt-4'><a href='../web-cgi/index.html'>← 대시보드로 돌아가기</a></p>");
    printf("</body></html>");

    return 0;
}
