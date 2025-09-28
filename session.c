#include "session.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

static int ensure_session_dir(void)
{
    struct stat st;
    if (stat(SESSION_DIR, &st) == 0)
    {
        return S_ISDIR(st.st_mode);
    }
    // 없으면 생성
    if (mkdir(SESSION_DIR, 0700) == 0)
    {
        return 1;
    }
    // race로 다른 프로세스가 먼저 만들었을 수도 있음
    if (stat(SESSION_DIR, &st) == 0 && S_ISDIR(st.st_mode))
    {
        return 1;
    }
    return 0;
}

static void build_path(const char *sid, char *out, size_t outlen)
{
    // 리눅스 경로 구분자 '/'
    snprintf(out, outlen, "%s/%s.sess", SESSION_DIR, sid);
}

int session_create(const char *user, char *sid, size_t sidlen)
{
    if (!ensure_session_dir())
        return 0;
    if (!gen_session_id(sid, sidlen))
        return 0;

    char path[4096];
    build_path(sid, path, sizeof(path));

    FILE *fp = fopen(path, "w");
    if (!fp)
        return 0;

    time_t now = time(NULL);
    long long exp = (long long)now + (long long)SESSION_TTL;

    // 1행: user, 2행: expire_ts
    fprintf(fp, "%s\n%lld\n", user ? user : "", exp);
    fclose(fp);
    return 1;
}

int session_validate(const char *sid, char *user_out, size_t user_len)
{
    if (!sid || strlen(sid) != 64)
        return 0;

    char path[4096];
    build_path(sid, path, sizeof(path));

    FILE *fp = fopen(path, "r");
    if (!fp)
        return 0;

    char user[256] = {0};
    char tsbuf[64] = {0};

    if (!fgets(user, sizeof(user), fp))
    {
        fclose(fp);
        return 0;
    }
    if (!fgets(tsbuf, sizeof(tsbuf), fp))
    {
        fclose(fp);
        return 0;
    }
    fclose(fp);

    trim(user);
    trim(tsbuf);

    long long exp = atoll(tsbuf);
    if (exp <= 0)
        return 0;

    time_t now = time(NULL);
    if ((long long)now > exp)
    {
        // 만료 → 삭제
        unlink(path);
        return 0;
    }

    if (user_out && user_len)
    {
        strncpy(user_out, user, user_len - 1);
        user_out[user_len - 1] = '\0';
    }
    return 1;
}

int session_destroy(const char *sid)
{
    if (!sid || strlen(sid) != 64)
        return 0;
    char path[4096];
    build_path(sid, path, sizeof(path));
    return unlink(path) == 0 ? 1 : 0;
}
