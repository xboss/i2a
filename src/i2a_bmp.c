#include "i2a_bmp.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef DEBUG
#ifndef _LOG
#define _LOG(fmt, ...)              \
    do {                            \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n");               \
    } while (0)
#endif
#else
#ifndef _LOG
#define _LOG(fmt, ...) \
    do {               \
    } while (0)
#endif
#endif

#define I2A_BMP_HEAD_INFO_SZ 54

#define _B32(_v_param_name)               \
    do {                                  \
        memcpy(b32, p, 4);                \
        memcpy(&(_v_param_name), b32, 4); \
        bzero(b32, 4);                    \
        p += 4;                           \
    } while (0)

#define _B16(_v_param_name)               \
    do {                                  \
        memcpy(b16, p, 2);                \
        memcpy(&(_v_param_name), b16, 2); \
        bzero(b16, 2);                    \
        p += 2;                           \
    } while (0)

i2a_bmp_t *i2a_load_bmp(const char *file_path) {
    FILE *fp = fopen(file_path, "rb");
    if (fp == NULL) {
        _LOG("file does not exist %s", file_path);
        return NULL;
    }

    unsigned char head_buf[I2A_BMP_HEAD_INFO_SZ];
    /* memset(head_buf, 0, I2A_BMP_HEAD_INFO_SZ); */
    bzero(head_buf, I2A_BMP_HEAD_INFO_SZ);
    int cnt = fread(head_buf, 1, I2A_BMP_HEAD_INFO_SZ, fp);
    if (cnt < 54) {
        _LOG("invalid BMP file format %s", file_path);
        fclose(fp);
        return NULL;
    }

    unsigned char *p = head_buf;
    unsigned char b16[2] = {0};
    unsigned char b32[4] = {0};

    /* read head */
    i2a_bmp_head_t head;
    head.type[0] = (char)*(p++);
    head.type[1] = (char)*(p++);
    _B32(head.size);
    _B16(head.r1);
    _B16(head.r2);
    _B32(head.seek);

    if (head.type[0] != 'B' || head.type[1] != 'M') {
        _LOG("invalid BMP file format %s", file_path);
        fclose(fp);
        return NULL;
    }

    /* read info */
    i2a_bmp_info_t info;
    _B32(info.size);
    _B32(info.w);
    _B32(info.h);
    _B16(info.flag);
    _B16(info.bit);
    _B32(info.r1);
    _B32(info.r2);
    _B32(info.r3);
    _B32(info.r4);
    _B32(info.r5);
    _B32(info.r6);

    /* read data */
    int data_len = (int)head.size - (int)head.seek;
    unsigned char *data = (unsigned char *)malloc(data_len);
    if (!data) {
        _LOG("load BMP file error, out of memory %s", file_path);
        fclose(fp);
        return NULL;
    }
    cnt = fread(data, 1, data_len, fp);
    if (cnt <= 0) {
        _LOG("load BMP file error %s", file_path);
        free(data);
        fclose(fp);
        return NULL;
    }

    i2a_bmp_t *bmp = (i2a_bmp_t *)malloc(sizeof(i2a_bmp_t));
    bmp->head = head;
    bmp->info = info;
    bmp->data = data;
    bmp->data_len = data_len;

    fclose(fp);
    return bmp;
}

void i2a_free(i2a_bmp_t *bmp) {
    if (!bmp) {
        return;
    }
    if (bmp->data) {
        free(bmp->data);
    }
    free(bmp);
}

static uint32_t bmp2rgb16(const i2a_bmp_t *bmp, const i2a_bmp_conf_t *conf, i2a_array_t *arr) {
    if (!bmp->data || bmp->data_len <= 0 || !conf || !arr) {
        return 0;
    }
    arr->type = I2A_ARR_TYPE_RGB16;

    unsigned char *p = bmp->data;
    int b_cnt = bmp->info.bit == 24 ? 3 : 1;

    if (bmp->data_len % b_cnt != 0) {
        _LOG("image data length error %d", bmp->data_len);
        return 0;
    }

    int len = bmp->data_len / b_cnt;

    arr->arr_len = len;
    arr->arr = (uint32_t *)malloc(sizeof(uint32_t) * len);

    int i, j;
    for (i = 0, j = 0; i < bmp->data_len; i += b_cnt, j++) {
        if (bmp->info.bit == 1) {
            /* TODO: */
        } else if (bmp->info.bit == 4) {
            /* TODO: */
        } else if (bmp->info.bit == 8) {
            /* TODO: */
        } else {
            uint32_t c = 0;
            memcpy(&c, p + i, b_cnt);
            c &= 0x00FFFFFFu;
            /* printf("c:0x%04x\n", c); */
            unsigned char r = (c & 0x00FF0000u) >> 16;
            unsigned char g = (c & 0x0000FF00u) >> 8;
            unsigned char b = c & 0x000000FFu;
            /* printf("r:0x%04x g:0x%04x b:0x%04x \n", r, g, b); */

            uint16_t r_new = r * 0x20u / 0x100u;
            uint16_t g_new = g * 0x40u / 0x100u;
            uint16_t b_new = b * 0x20u / 0x100u;
            /* printf("r_new:0x%04x g_new:0x%04x b_new:0x%04x \n", r_new, g_new, b_new);
            printf("r_new1:0x%04x g_new1:0x%04x b_new1:0x%04x \n", ((r_new << 11) & 0xF800u), ((g_new << 5) & 0x07E0u),
                   (b_new & 0x001Fu)); */

            uint16_t c_new = ((r_new << 11) & 0xF800u) | ((g_new << 5) & 0x07E0u) | (b_new & 0x001Fu);
            arr->arr[j] = c_new;
            /* printf("j: %d, c_new:0x%04x, rgb16:0x%04x\n", j, c_new, arr->arr[j].rgb16); */
            /* if (i == 3) {
                return 1;
            } */
        }
    }
    assert(arr->arr_len == j);

    return arr->arr_len;
}

int i2a_bmp2array(const i2a_bmp_t *bmp, const i2a_bmp_conf_t *conf, i2a_array_t *arr) {
    if (!bmp || !conf || !arr) {
        return 0;
    }

    if (bmp->info.bit != 1 && bmp->info.bit != 4 && bmp->info.bit != 8 && bmp->info.bit != 24) {
        _LOG("image bit error %d", bmp->info.bit);
        return 0;
    }

    if (conf->arr_type == I2A_ARR_TYPE_RGB16) {
        return bmp2rgb16(bmp, conf, arr);
    } else {
        _LOG("array type error %c", conf->arr_type);
        return 0;
    }

    return 0;
}
int i2a_array2bmp(const i2a_array_t *arr, const i2a_bmp_conf_t *conf, i2a_bmp_t *bmp) {
    if (!bmp || !conf || !arr) {
        return 0;
    }

    return 0;
}
