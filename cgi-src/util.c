#include "util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

void print_http_header_html(void){
    printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
}
void print_http_header_html_no_cache(void){
    printf("Content-Type: text/html; charset=utf-8\r\n");
    printf("Cache-Control: no-store\r\n\r\n");
}
void print_http_header_json(void){
    printf("Content-Type: application/json; charset=utf-8\r\n");
    printf("Cache-Control: no-store\r\n\r\n");
}

size_t read_post_body(char *buf, size_t buflen){
    buf[0] = '\0';
    const char *len_str = getenv("CONTENT_LENGTH");
    if(!len_str) return 0;
    long n = strtol(len_str, NULL, 10);
    if(n <= 0 || n >= (long)buflen) n = buflen - 1;
    size_t r = fread(buf, 1, (size_t)n, stdin);
    buf[r] = '\0';
    return r;
}

void url_decode(char *s){
    char *o = s;
    for(char *p=s; *p; p++){
        if(*p == '+') { *o++ = ' '; }
        else if(*p=='%' && isxdigit((unsigned char)p[1]) && isxdigit((unsigned char)p[2])){
            char hx[3] = {p[1], p[2], 0};
            *o++ = (char) strtol(hx, NULL, 16);
            p += 2;
        } else { *o++ = *p; }
    }
    *o = '\0';
}

int parse_kv_from_body(const char *body, const char *key, char *out, size_t outlen){
    size_t klen = strlen(key);
    const char *p = body;
    while(p && *p){
        const char *q = strstr(p, key);
        if(!q) break;
        if((q==body || q[-1]=='&') && q[klen]=='='){
            q += klen + 1;
            size_t i=0;
            while(q[i] && q[i] != '&' && i < outlen-1){
                out[i] = q[i];
                i++;
            }
            out[i] = '\0';
            url_decode(out);
            return 1;
        }
        p = q + 1;
    }
    return 0;
}

int get_cookie(const char *cookie_header, const char *name, char *out, size_t outlen){
    if(!cookie_header) return 0;
    size_t nlen = strlen(name);
    const char *p = cookie_header;
    while(*p){
        while(*p==' ') p++;
        if(!strncmp(p, name, nlen) && p[nlen]=='='){
            p += nlen + 1;
            size_t i=0;
            while(p[i] && p[i] != ';' && i < outlen-1){
                out[i] = p[i];
                i++;
            }
            out[i] = '\0';
            return 1;
        }
        const char *sc = strchr(p, ';');
        if(!sc) break;
        p = sc + 1;
    }
    return 0;
}

int gen_session_id(char *out, size_t outlen){
    if(outlen < 65) return 0;
    unsigned char rnd[32];
    int fd = open("/dev/urandom", O_RDONLY);
    if(fd < 0) return 0;
    ssize_t r = read(fd, rnd, sizeof(rnd));
    close(fd);
    if(r != (ssize_t)sizeof(rnd)) return 0;
    static const char *hx="0123456789abcdef";
    for(int i=0;i<32;i++){
        out[i*2]   = hx[(rnd[i]>>4)&0xF];
        out[i*2+1] = hx[rnd[i]&0xF];
    }
    out[64] = '\0';
    return 1;
}

void html_escape(const char *in, char *out, size_t outlen){
    size_t j=0;
    for(size_t i=0; in[i] && j+6 < outlen; i++){
        char c = in[i];
        if(c=='&'){ j += snprintf(out+j, outlen-j, "&amp;"); }
        else if(c=='<'){ j += snprintf(out+j, outlen-j, "&lt;"); }
        else if(c=='>'){ j += snprintf(out+j, outlen-j, "&gt;"); }
        else if(c=='"'){ j += snprintf(out+j, outlen-j, "&quot;"); }
        else if(c=='\''){ j += snprintf(out+j, outlen-j, "&#39;"); }
        else { out[j++] = c; }
    }
    out[j] = '\0';
}
