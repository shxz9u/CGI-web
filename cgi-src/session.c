#include "session.h"
#include "util.h"
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

int ensure_session_dir(void){
    struct stat st;
    if(stat(SESS_DIR, &st) == 0){
        if(S_ISDIR(st.st_mode)) return 1;
        return 0;
    }
    if(mkdir(SESS_DIR, 0700) == 0) return 1;
    return 0;
}

static void make_path(const char *sid, char *out, size_t outlen){
    snprintf(out, outlen, "%s/%s", SESS_DIR, sid);
}

int session_create(const char *username, char *sid_out, size_t sid_len){
    if(!ensure_session_dir()) return 0;
    if(!gen_session_id(sid_out, sid_len)) return 0;

    char path[512];
    make_path(sid_out, path, sizeof(path));

    FILE *fp = fopen(path, "w");
    if(!fp) return 0;
    time_t now = time(NULL);
    time_t exp = now + SESS_TTL;
    // 파일 내용: username\nexpires\n
    fprintf(fp, "%s\n%ld\n", username, (long)exp);
    fclose(fp);
    return 1;
}

int session_validate(const char *sid, char *username_out, size_t ulen){
    if(!sid || !sid[0]) return 0;
    char path[512];
    make_path(sid, path, sizeof(path));

    FILE *fp = fopen(path, "r");
    if(!fp) return 0;

    char user[256]={0}, line2[256]={0};
    if(!fgets(user, sizeof(user), fp)){ fclose(fp); return 0; }
    if(!fgets(line2, sizeof(line2), fp)){ fclose(fp); return 0; }
    fclose(fp);

    // 개행 제거
    user[strcspn(user, "\r\n")] = 0;
    line2[strcspn(line2, "\r\n")] = 0;

    long exp = strtol(line2, NULL, 10);
    time_t now = time(NULL);
    if(exp < now){
        unlink(path);
        return 0;
    }
    // 연장(슬라이딩 만료)
    fp = fopen(path, "w");
    if(fp){
        fprintf(fp, "%s\n%ld\n", user, (long)(now + SESS_TTL));
        fclose(fp);
    }
    snprintf(username_out, ulen, "%s", user);
    return 1;
}

int session_destroy(const char *sid){
    if(!sid || !sid[0]) return 0;
    char path[512];
    make_path(sid, path, sizeof(path));
    return unlink(path) == 0;
}
