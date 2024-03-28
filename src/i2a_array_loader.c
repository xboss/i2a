#include "i2a_array_loader.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/* "0x12345678" */
#define _MAX_HEX_VAL_CHAR_SZ 10
#define _TMP_BUF_SZ 16

#define _BACKWARD                       \
    ctx->src--;                         \
    ctx->src_len++;                     \
    if (ctx->src_len <= 0) {            \
        printf("memory out of bounds"); \
        exit(1);                        \
    }

#define _FORWARD    \
    ctx->src++;     \
    ctx->src_len--; \
    if (ctx->src_len <= 0) break

#define _RESET_ARR_CTX_TMP_BUF memset(ctx->tmp_buf, 0, _TMP_BUF_SZ)

typedef struct {
    char *src;
    int src_len;
    char tmp_buf[_TMP_BUF_SZ];
    int is_continue;
    int is_comment_line;
    int is_comment_block;
    uint32_t *arr;
    int arr_idx;
    int arr_len;
} array_ctx_t;

static uint32_t hexstr2uint32(const char *str) {
    uint32_t val = 0;
    int len = strnlen(str, _MAX_HEX_VAL_CHAR_SZ + 1);
    assert(len > 2 && len <= _MAX_HEX_VAL_CHAR_SZ);
    int i;
    for (i = 2; i < len; i++) {
        val = val * 16 + (str[i] & 15) + (str[i] >= 'A' ? 9 : 0);
    }
    assert(len == i);
    return val;
}

static void next(array_ctx_t *ctx) {
    assert(ctx);
    assert(ctx->src);
    assert(ctx->src_len > 0);

    uint32_t val = 0;
    int hex_val_idx = 0, len = 0;
    while (ctx->src_len > 0) {
        if (ctx->is_comment_line) {
            if (*ctx->src == '\n' || *ctx->src == '\r') {
                ctx->is_comment_line = 0;
            }
            _FORWARD;
            continue;
        }

        if (ctx->is_comment_block) {
            len = strnlen(ctx->tmp_buf, 2);
            if (len == 1 && ctx->tmp_buf[0] == '*') {
                if (*ctx->src == '/') {
                    ctx->is_comment_block = 0;
                }
                _RESET_ARR_CTX_TMP_BUF;
                _FORWARD;
                continue;
            }
            assert(len == 0);
            if (*ctx->src == '*') {
                if (ctx->src_len - 1 <= 0) {
                    ctx->tmp_buf[0] = '*';
                    _FORWARD;
                    continue;
                }
                _FORWARD;
                if (*ctx->src == '/') {
                    ctx->is_comment_block = 0;
                }
            }
            _FORWARD;
            continue;
        }

        if (*ctx->src == '*') {
            len = strnlen(ctx->tmp_buf, 2);
            if (len == 1 && ctx->tmp_buf[0] == '/') {
                ctx->is_comment_block = 1;
                _RESET_ARR_CTX_TMP_BUF;
                _FORWARD;
                continue;
            }
        }

        if (*ctx->src == '/') {
            len = strnlen(ctx->tmp_buf, 2);
            if (len == 1 && ctx->tmp_buf[0] == '/') {
                ctx->is_comment_line = 1;
                _RESET_ARR_CTX_TMP_BUF;
                _FORWARD;
                continue;
            }
            assert(len == 0);

            if (ctx->src_len - 1 <= 0) {
                ctx->tmp_buf[0] = '/';
                _FORWARD;
                continue;
            }
            _FORWARD;
            if (*ctx->src == '/') {
                ctx->is_comment_line = 1;
                _FORWARD;
                continue;
            } else if (*ctx->src == '*') {
                ctx->is_comment_block = 1;
                _FORWARD;
                continue;
            } else {
                _BACKWARD;
            }
        }

        if (ctx->is_continue) {
            hex_val_idx = strnlen(ctx->tmp_buf, _MAX_HEX_VAL_CHAR_SZ + 1);
            assert(hex_val_idx <= _MAX_HEX_VAL_CHAR_SZ);
            if (hex_val_idx < 2) {
                if (*ctx->src != 'x' && *ctx->src != 'X') {
                    ctx->is_continue = 0;
                    _RESET_ARR_CTX_TMP_BUF;
                    _FORWARD;
                    continue;
                }
                ctx->tmp_buf[hex_val_idx++] = *ctx->src;
                _FORWARD;
                continue;
            }

            while ((*ctx->src >= '0' && *ctx->src <= '9') || (*ctx->src >= 'a' && *ctx->src <= 'f') ||
                   (*ctx->src >= 'A' && *ctx->src <= 'F')) {
                if (strnlen(ctx->tmp_buf, _MAX_HEX_VAL_CHAR_SZ + 1) > _MAX_HEX_VAL_CHAR_SZ) {
                    printf("invalid hex value, must be <= 32 bits.\n");
                    return;
                }
                ctx->tmp_buf[hex_val_idx++] = *ctx->src;
                _FORWARD;
            }
            if (ctx->src_len <= 0) {
                break;
            }
            assert(hex_val_idx <= _MAX_HEX_VAL_CHAR_SZ);
            ctx->is_continue = 0;
            if (ctx->arr_idx >= ctx->arr_len) {
                printf("array index (%d) > array length (%d).\n", ctx->arr_idx, ctx->arr_len);
                return;
            }
            ctx->arr[ctx->arr_idx++] = hexstr2uint32(ctx->tmp_buf);
            _RESET_ARR_CTX_TMP_BUF;
            _FORWARD;
            continue;
        }

        if (*ctx->src == '0') {
            ctx->is_continue = 1;
            hex_val_idx = 0;
            _RESET_ARR_CTX_TMP_BUF;
            ctx->tmp_buf[hex_val_idx++] = *ctx->src;
            _FORWARD;
            if (*ctx->src == 'x' || *ctx->src == 'X') {
                ctx->tmp_buf[hex_val_idx++] = *ctx->src;
                _FORWARD;
                while ((*ctx->src >= '0' && *ctx->src <= '9') || (*ctx->src >= 'a' && *ctx->src <= 'f') ||
                       (*ctx->src >= 'A' && *ctx->src <= 'F')) {
                    if (strnlen(ctx->tmp_buf, _MAX_HEX_VAL_CHAR_SZ + 1) > _MAX_HEX_VAL_CHAR_SZ) {
                        printf("invalid hex value, must be <= 32 bits.\n");
                        return;
                    }
                    ctx->tmp_buf[hex_val_idx++] = *ctx->src;
                    _FORWARD;
                }
                if (ctx->src_len <= 0) {
                    break;
                }
                assert(hex_val_idx <= _MAX_HEX_VAL_CHAR_SZ);
                ctx->is_continue = 0;
                if (ctx->arr_idx >= ctx->arr_len) {
                    printf("array index (%d) > array length (%d).\n", ctx->arr_idx, ctx->arr_len);
                    return;
                }
                ctx->arr[ctx->arr_idx++] = hexstr2uint32(ctx->tmp_buf);
                _RESET_ARR_CTX_TMP_BUF;
                _FORWARD;
                continue;
            }
        }
        _RESET_ARR_CTX_TMP_BUF;
        _FORWARD;
    }
}

int i2a_load_array(const char *file_path, const int arr_size, uint32_t **arr) {
    if (!file_path || arr_size <= 0) {
        return -1;
    }

    FILE *fp = fopen(file_path, "rb");
    if (fp == NULL) {
        printf("file does not exist %s\n", file_path);
        return -1;
    }

    const int buf_sz = 1; /* TODO: */
    char buf[buf_sz + 1];
    memset(buf, 0, buf_sz + 1);

    array_ctx_t arr_ctx;
    memset(&arr_ctx, 0, sizeof(arr_ctx));
    arr_ctx.arr_len = arr_size;
    arr_ctx.arr = (uint32_t *)malloc(arr_ctx.arr_len);
    if (!arr_ctx.arr) {
        perror("out of memory.");
        return -1;
    }

    int r = 0;
    while (!feof(fp)) {
        r = fread(buf, sizeof(char), buf_sz, fp);
        printf("read size:%d\n", r);
        printf("buf:%s\n", buf);
        if (r <= 0) {
            break;
        }

        arr_ctx.src = buf;
        arr_ctx.src_len = r;
        next(&arr_ctx);

        memset(buf, 0, buf_sz + 1);
    }

    /* printf("buf size:%d read size:%d src size:%lu\n", src_sz, r, strlen(src)); */
    /* printf("%d\n", feof(fp)); */

    fclose(fp);

    *arr = arr_ctx.arr;
    return arr_ctx.arr_len;
}
