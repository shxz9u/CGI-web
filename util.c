#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

static void put_header_common_cache(const char *ctype, int nocache)
{
    // Apache CGI는 \r\n 권장
    printf("Content-Type: %s\r\n", ctype);
    if (nocache)
    {
        printf("Cache-Control: no-store, no-cache, must-revalidate\r\n");
        printf("Pragma: no-cache\r\n");
        printf("Expires: 0\r\n");
    }
    printf("\r\n");
}

void print_http_header_html(void) { put_header_common_cache("text/html; charset=utf-8", 0); }
void print_http_header_html_no_cache(void) { put_header_common_cache("text/html; charset=utf-8", 1); }
void print_http_header_json(void) { put_header_common_cache("application/json; charset=utf-8", 1); }

size_t read_post_body(char *buf, size_t buflen)
{
    buf[0] = '\0';
    const char *cl = getenv("CONTENT_LENGTH");
    if (!cl)
        return 0;
    size_t n = (size_t)atoi(cl);
    if (n >= buflen)
        n = buflen - 1;
    size_t r = fread(buf, 1, n, stdin);
    buf[r] = '\0';
    return r;
}

static int hex2(int c)
{
    if ('0' <= c && c <= '9')
        return c - '0';
    if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

void url_decode(char *s)
{
    char *o = s;
    for (; *s; s++)
    {
        if (*s == '+')
        {
            *o++ = ' ';
        }
        else if (*s == '%' && isxdigit((unsigned char)s[1]) && isxdigit((unsigned char)s[2]))
        {
            int hi = hex2(s[1]), lo = hex2(s[2]);
            if (hi >= 0 && lo >= 0)
            {
                *o++ = (char)((hi << 4) | lo);
                s += 2;
            }
        }
        else
        {
            *o++ = *s;
        }
    }
    *o = '\0';
}

int parse_kv_from_body(const char *body, const char *key, char *out, size_t outlen)
{
    size_t klen = strlen(key);
    const char *p = body;
    out[0] = '\0';
    while (p && *p)
    {
        const char *q = strstr(p, key);
        if (!q)
            break;
        if ((q == p || q[-1] == '&') && q[klen] == '=')
        {
            q += klen + 1;
            size_t i = 0;
            while (q[i] && q[i] != '&' && i + 1 < outlen)
            {
                out[i++] = q[i];
            }
            out[i] = '\0';
            url_decode(out);
            return 1;
        }
        p = strchr(q + 1, '&');
        if (p)
            p++;
    }
    return 0;
}

int get_cookie(const char *cookie_header, const char *name, char *out, size_t outlen)
{
    out[0] = '\0';
    if (!cookie_header || !name)
        return 0;
    size_t nlen = strlen(name);
    const char *p = cookie_header;
    while (*p)
    {
        while (*p == ' ' || *p == ';')
            p++;
        if (strncmp(p, name, nlen) == 0 && p[nlen] == '=')
        {
            p += nlen + 1;
            size_t i = 0;
            while (*p && *p != ';' && i + 1 < outlen)
                out[i++] = *p++;
            out[i] = '\0';
            return 1;
        }
        const char *sc = strchr(p, ';');
        if (!sc)
            break;
        p = sc + 1;
    }
    return 0;
}

int gen_session_id(char *out, size_t outlen)
{
    if (outlen < 65)
        return 0; // 32바이트 -> 64hex + NUL
    unsigned char rnd[32];
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0)
        return 0;
    ssize_t r = read(fd, rnd, sizeof(rnd));
    close(fd);
    if (r != (ssize_t)sizeof(rnd))
        return 0;

    static const char *hex = "0123456789abcdef";
    for (int i = 0; i < 32; i++)
    {
        out[i * 2] = hex[(rnd[i] >> 4) & 0xF];
        out[i * 2 + 1] = hex[rnd[i] & 0xF];
    }
    out[64] = '\0';
    return 1;
}

void html_escape(const char *in, char *out, size_t outlen)
{
    size_t i = 0;
    for (; *in && i + 6 < outlen; in++)
    {
        switch (*in)
        {
        case '&':
            memcpy(out + i, "&amp;", 5);
            i += 5;
            break;
        case '<':
            memcpy(out + i, "&lt;", 4);
            i += 4;
            break;
        case '>':
            memcpy(out + i, "&gt;", 4);
            i += 4;
            break;
        case '"':
            memcpy(out + i, "&quot;", 6);
            i += 6;
            break;
        case '\'':
            memcpy(out + i, "&#39;", 5);
            i += 5;
            break;
        default:
            out[i++] = *in;
        }
    }
    out[i] = '\0';
}

void trim(char *s)
{
    if (!s)
        return;
    size_t n = strlen(s);
    while (n && isspace((unsigned char)s[n - 1]))
        s[--n] = '\0';
    char *p = s;
    while (*p && isspace((unsigned char)*p))
        p++;
    if (p != s)
        memmove(s, p, strlen(p) + 1);
}
