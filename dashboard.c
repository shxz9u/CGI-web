#include <stdio.h>

int main(void)
{
    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");

    printf("<!DOCTYPE html>\n");
    printf("<html lang='ko'>\n");
    printf("<head>\n");
    printf("  <meta charset='UTF-8'>\n");
    printf("  <meta name='viewport' content='width=device-width, initial-scale=1.0'>\n");
    printf("  <title>프린터 관리 대시보드</title>\n");
    printf("  <link href='https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css' rel='stylesheet'>\n");
    printf("  <style>\n");
    printf("    body { background-color: #f8f9fa; }\n");
    printf("    .sidebar { height: 100vh; position: fixed; top: 0; left: 0; width: 220px; ");
    printf("background-color: #e9ecef; padding-top: 60px; border-right: 1px solid #dee2e6; }\n");
    printf("    .sidebar a { display: block; padding: 12px 20px; color: #343a40; text-decoration: none; font-weight: 500; }\n");
    printf("    .sidebar a:hover { background-color: #dee2e6; }\n");
    printf("    .main-content { margin-left: 220px; padding: 20px; }\n");
    printf("    iframe { width: 100%%; height: 80vh; border: none; background: #fff; box-shadow: 0 0 10px rgba(0,0,0,0.1); border-radius: 6px; }\n");
    printf("    .topbar { position: fixed; top: 0; left: 0; right: 0; height: 60px; background-color: #343a40; color: #fff; display: flex; align-items: center; justify-content: space-between; padding: 0 20px; z-index: 1000; }\n");
    printf("    .user-info { font-size: 0.9rem; }\n");
    printf("  </style>\n");
    printf("</head>\n");
    printf("<body>\n");

    // 상단바
    printf("<div class='topbar'>\n");
    printf("  <h5 class='m-0'>프린터 관리 대시보드</h5>\n");
    printf("  <div class='user-info' id='user-area'>로그인 필요</div>\n");
    printf("</div>\n");

    // 사이드바
    printf("<div class='sidebar'>\n");
    printf("  <a href='/cgi-bin/ipdiag.cgi' target='mainFrame'>IP 진단</a>\n");
    printf("  <a href='/cgi-bin/box.cgi' target='mainFrame'>프린터 박스</a>\n");
    printf("  <a href='/cgi-bin/print.cgi' target='mainFrame'>출력 기능</a>\n");
    printf("  <a id='auth-link' href='/cgi-bin/login.cgi'>로그인</a>\n");
    printf("</div>\n");

    // 메인 영역
    printf("<div class='main-content'>\n");
    printf("  <iframe name='mainFrame' src='/cgi-bin/ipdiag.cgi'></iframe>\n");
    printf("</div>\n");

    // JS: 로그인 상태 확인
    printf("<script>\n");
    printf("fetch('/cgi-bin/whoami.cgi')\n");
    printf("  .then(r => { if (!r.ok) throw new Error('not logged in'); return r.json(); })\n");
    printf("  .then(j => {\n");
    printf("    if (j.ok) {\n");
    printf("      document.getElementById('user-area').innerText = j.user + '님 환영합니다';\n");
    printf("      document.getElementById('auth-link').innerText = '로그아웃';\n");
    printf("      document.getElementById('auth-link').setAttribute('href','/cgi-bin/logout.cgi');\n");
    printf("    }\n");
    printf("  })\n");
    printf("  .catch(err => { });\n");
    printf("</script>\n");

    printf("</body></html>\n");
    return 0;
}
