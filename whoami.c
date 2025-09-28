#include "util.h"
#include "session.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    const char *cookie = getenv("HTTP_COOKIE");
    char sid[256] = {0}, user[256] = {0};

    if (cookie && get_cookie(cookie, SESSION_COOKIE, sid, sizeof(sid)) &&
        session_validate(sid, user, sizeof(user)))
    {
        print_http_header_json();
        printf("{\"ok\":true,\"user\":\"%s\"}\n", user);
    }
    else
    {
        printf("Status: 401 Unauthorized\r\n");
        print_http_header_json();
        printf("{\"ok\":false}\n");
    }
    return 0;
}
