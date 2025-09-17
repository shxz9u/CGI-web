#define _CRT_SECURE_NO_WARNINGS
#include "session.h"
#include "util.h"
#include <windows.h>
#include <direct.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static int ensure_session_dir(void)
{
    DWORD attr = GetFileAttributesA(SESSION_DIR);
    if (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY))
        return 1;
    // 없으면 생성
    if (_mkdir(SESSION_DIR) == 0)
        return 1;
    // 이미 존재하거나 실패
    attr = GetFileAttributesA(SESSION_DIR);
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

static void build_path(const char *sid, char *out, size_t outlen)
{
    snprintf(out, outlen, "%s\\%s.sess", SESSION_DIR, sid);
}

int session_create(const char *user, char *sid, size_t sidlen)
{
    if (!ensure_session_dir())
        return 0;
    if (!gen_session_id(sid, sidlen))
        return 0;

    char path[MAX_PATH];
    build_path(sid, path, sizeof(path));

    FILE *fp = fopen(path, "w");
    if (!fp)
        return 0;

    time_t now = time(NULL);
    time_t exp = now + SESSION_TTL;

    // 파일 포맷: first line=user, second line=expire_ts
    fprintf(fp, "%s\n%lld\n", user, (long long)exp);
    fclose(fp);
    return 1;
}

int session_validate(const char *sid, char *user_out, size_t user_len)
{
    if (!sid || strlen(sid) != 64)
        return 0;
    char path[MAX_PATH];
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

    long long exp = _atoi64(tsbuf);
    if (exp <= 0)
        return 0;

    time_t now = time(NULL);
    if ((long long)now > exp)
    {
        // 만료 → 삭제
        DeleteFileA(path);
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
    char path[MAX_PATH];
    build_path(sid, path, sizeof(path));
    return DeleteFileA(path) ? 1 : 0;
}
