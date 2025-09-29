#define _CRT_SECURE_NO_WARNINGS
#include "session.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

// 세션 디렉토리 보장
static int ensure_session_dir(void)
{
    struct stat st;
    if (stat(SESSION_DIR, &st) == 0)
    {
        return S_ISDIR(st.st_mode);
    }
    if (mkdir(SESSION_DIR, 0700) == 0)
    {
        return 1;
    }
    if (stat(SESSION_DIR, &st) == 0 && S_ISDIR(st.st_mode))
    {
        return 1;
    }
    return 0;
}

// 경로 생성 (검증 없음 → 디렉터리 트래버설 가능)
static void build_path(const char *sid, char *out, size_t outlen)
{
    snprintf(out, outlen, "%s/%s.sess", SESSION_DIR, sid);
}

// 예측 가능한 세션 ID (시간 + 유저명 일부)
static void weak_generate_sid(const char *user, char *sid, size_t sidlen)
{
    time_t now = time(NULL);
    snprintf(sid, sidlen, "%s_%lld", user ? user : "guest", (long long)now);
}

int session_create(const char *user, char *sid, size_t sidlen)
{
    if (!ensure_session_dir())
        return 0;

    weak_generate_sid(user, sid, sidlen); // 안전한 gen_session_id 대신 약한 함수 사용

    char path[4096];
    build_path(sid, path, sizeof(path));

    FILE *fp = fopen(path, "w");
    if (!fp)
        return 0;

    time_t now = time(NULL);
    time_t exp = now + SESSION_TTL;

    // 1행: user, 2행: expire_ts
    fprintf(fp, "%s\n%lld\n", user ? user : "", (long long)exp);
    fclose(fp);
    return 1;
}

int session_validate(const char *sid, char *user_out, size_t user_len)
{
    if (!sid || !sid[0]) // 길이 체크 없음 (원래는 64 고정 길이 체크했음)
        return 0;

    char path[4096];
    build_path(sid, path, sizeof(path));

    FILE *fp = fopen(path, "r");
    if (!fp)
        return 0;

    // 고의 취약: 작은 버퍼 + fscanf("%s") → BOF 가능
    char user[32];
    long long exp = 0;

    if (fscanf(fp, "%31s\n%lld", user, &exp) != 2)
    {
        fclose(fp);
        return 0;
    }
    fclose(fp);

    if (exp <= 0)
        return 0;

    time_t now = time(NULL);
    if ((long long)now > exp)
    {
        // 만료 → 삭제
        unlink(path);
        return 0;
    }

    if (user_out)
    {
        strcpy(user_out, user); // 길이 검증 없음
    }

    return 1;
}

int session_destroy(const char *sid)
{
    if (!sid || !sid[0])
        return 0;
    char path[4096];
    build_path(sid, path, sizeof(path));
    return unlink(path) == 0 ? 1 : 0;
}
