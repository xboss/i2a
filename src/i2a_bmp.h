#ifndef _I2A_BMP_H
#define _I2A_BMP_H

#include <stdlib.h>

#define I2A_BMP_HEAD_SIZE 14u
#define I2A_BMP_INFO_SIZE 40u

/* 32 bit RGBA (4bytes/pixel) */
#define I2A_ARR_TYPE_RGBA32 0x01u
/* 32 bit BGRA (4bytes/pixel) */
#define I2A_ARR_TYPE_BGRA32 0x02u
/* 24bit RGB (3bytes/pixel) */
#define I2A_ARR_TYPE_RGB24 0x03u
/* RRRRRGGGGGGBBBBB (2byte/pixel) */
#define I2A_ARR_TYPE_RGB16 0x04u
/* BBBBBGGGGGGRRRRR (2byte/pixel) */
#define I2A_ARR_TYPE_BGR16 0x05u
/* RRRRRGGGGGBBBBBA (2byte/pixel) */
#define I2A_ARR_TYPE_RGBA16 0x06u
/* 8bit RRRGGGBB (1byte/pixel) */
#define I2A_ARR_TYPE_RGB8 0x07u
/* 8bit grayscale (1byte/pixel) */
#define I2A_ARR_TYPE_GRAY8 0x08u
/* 1bit line art (1bit/pixel) */
#define I2A_ARR_TYPE_BIT 0x09u

/* 像素点的位数 1（双色灰阶） */
#define I2A_BMP_BIT_CNT_1 1u
/* 像素点的位数 4（16色灰阶） */
#define I2A_BMP_BIT_CNT_4 4u
/* 像素点的位数 8（256色灰阶)  */
#define I2A_BMP_BIT_CNT_8 8u
/* 像素点的位数 24（彩色） */
#define I2A_BMP_BIT_CNT_24 24u

/* #pragma pack(1)  // 强制1个字节对齐 */

/* BMP的文件头 */
typedef struct {
    char type[2];  /* 图片的类型 "BM" */
    uint32_t size; /* 文件大小 */
    uint16_t r1;   /* 保留1 */
    uint16_t r2;   /* 保留2 */
    uint32_t seek; /* 数据偏移字节(真实像素点数据) */
} i2a_bmp_head_t;

/* BMP的参数信息 */
typedef struct {
    uint32_t size; /* 当前结构体大小 */
    uint32_t w;    /* 宽度 */
    uint32_t h;    /* 高度 */
    uint16_t flag; /* 固定为1 */
    uint16_t bit; /* 像素点的位数 常用值是1（双色灰阶）、4（16色灰阶）、8（256色灰阶）和24（彩色） */
    uint32_t r1; /* 压缩方式  0 */
    uint32_t r2; /* 水平分辨率 */
    uint32_t r3; /* 垂直分辨率 */
    uint32_t r4; /* 垂直分辨率 */
    uint32_t r5; /* 引用色彩 */
    uint32_t r6; /* 关键色彩 */
} i2a_bmp_info_t;

typedef struct {
    i2a_bmp_head_t head;
    i2a_bmp_info_t info;
    unsigned char *data;
    int data_len;
} i2a_bmp_t;

typedef struct {
    char arr_type;
    /* uint16_t bmp_bit_cnt; */
    i2a_bmp_head_t bmp_head;
    i2a_bmp_info_t bmp_info;
} i2a_bmp_conf_t;

typedef struct {
    uint32_t *arr;
    int arr_len;
    char type;
} i2a_array_t;

void i2a_free(i2a_bmp_t *bmp);
i2a_bmp_t *i2a_load_bmp(const char *file_path);
int i2a_write_bmp_file(const char *file_path, const i2a_bmp_t *bmp);

int i2a_bmp_to_array(const i2a_bmp_t *bmp, const i2a_bmp_conf_t *conf, i2a_array_t *arr);
int i2a_array_to_bmp(const i2a_array_t *arr, const i2a_bmp_conf_t *conf, i2a_bmp_t *bmp);

#endif /* I2A_BMP_H */