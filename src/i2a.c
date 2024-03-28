#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "i2a_array_loader.h"
#include "i2a_bmp.h"

static void print_bmp_info(const i2a_bmp_t *bmp) {
    assert(bmp);
    printf("/*\n");
    printf("file size: %d\n", bmp->head.size);
    printf("file data offset: %d\n", bmp->head.seek);
    printf("file info size: %d\n", bmp->info.size);
    printf("image width: %d\n", bmp->info.w);
    printf("image height: %d\n", bmp->info.h);
    printf("image bit: %d\n", bmp->info.bit);
    printf("image compression: %d\n", bmp->info.r1);
    printf("image color used: %d\n", bmp->info.r5);
    printf("image data size: %d\n", bmp->data_len);
    printf("*/\n");
}

static void print_array(const i2a_bmp_t *bmp, const i2a_array_t *arr) {
    assert(bmp);
    assert(arr);
    assert(arr->arr_len == bmp->info.h * bmp->info.w);
    printf("static const uint16_t bmp_arr[] = {\n");
    int i;
    for (i = 0; i < arr->arr_len; i++) {
        printf("0x%04x", arr->arr[i]);
        if (i != arr->arr_len - 1) {
            printf(", ");
        }
        if ((i + 1) % 14 == 0) {
            printf("\n");
        }
    }
    printf("};\n");
}

static int bmp2arr(const char *file_path) {
    i2a_bmp_t *bmp = i2a_load_bmp(file_path);
    assert(bmp);
    print_bmp_info(bmp);
    /* convert to array */
    i2a_bmp_conf_t conf = {.arr_type = I2A_ARR_TYPE_RGB16};
    i2a_array_t arr;
    memset(&arr, 0, sizeof(i2a_array_t));
    uint32_t r = i2a_bmp_to_array(bmp, &conf, &arr);
    assert(r > 0);
    /* printf("------ array ------\n"); */
    print_array(bmp, &arr);
    i2a_free(bmp);
    return r;
}

static int arr2bmp(const char *file_path, uint32_t width, uint32_t height, uint16_t bit_cnt) {
    int bit_cnt_bytes = 1;
    if (I2A_BMP_BIT_CNT_24 == bit_cnt) {
        bit_cnt_bytes = 3;
    }

    i2a_bmp_conf_t conf = {.arr_type = I2A_ARR_TYPE_RGB16};
    i2a_array_t arr;
    /* TODO: */
    arr.type = I2A_ARR_TYPE_RGB16;
    i2a_bmp_t bmp;
    memset(&bmp, 0, sizeof(i2a_bmp_t));
    bmp.head.type[0] = 'B';
    bmp.head.type[1] = 'M';
    bmp.head.size = I2A_BMP_HEAD_SIZE + I2A_BMP_INFO_SIZE + width * height * bit_cnt_bytes;
    bmp.head.seek = I2A_BMP_HEAD_SIZE + I2A_BMP_INFO_SIZE;

    bmp.info.size = I2A_BMP_INFO_SIZE;
    bmp.info.flag = 1;
    bmp.info.w = width;
    bmp.info.h = height;
    bmp.info.bit = bit_cnt;

    int r = i2a_array_to_bmp(&arr, &conf, &bmp);
    assert(r > 0);
    return r;
}

static int load_array(const char *file_path, int width, int height) {
    if (!file_path || width <= 0 || height <= 0) {
        return -1;
    }

    int arr_size = width * height;
    uint32_t *arr = NULL;
    int r = i2a_load_array(file_path, arr_size, &arr);
    if (r != arr_size || !arr) {
        printf("load array error. %s\n", file_path);
        return -1;
    }

    /* debug start */
    int i;
    for (i = 0; i < 8; i++) {
        printf("i:%d, 0x%04x\n", i, arr[i]);
    }
    assert(arr[0] == 0x18c3u);
    assert(arr[1] == 0x0861u);
    assert(arr[2] == 0x0862u);
    assert(arr[3] == 0x0863u);
    assert(arr[4] == 0x0864u);
    assert(arr[5] == 0x0865u);
    assert(arr[6] == 0x0866u);
    assert(arr[7] == 0x0000u);
    /* debug end */

    /* TODO: */
    return 0;
}

int main(int argc, char const *argv[]) {
    /* printf("start\n"); */
    if (argc != 3) {
        printf("Usage: i2a <file path> <mode>\n");
        return 1;
    }

    const char *file_path = argv[1];
    const char *mode = argv[2];

    if (strncasecmp("a2b", mode, 3) == 0) {
        arr2bmp(file_path, 240, 320, I2A_BMP_BIT_CNT_24);
    } else if (strncasecmp("b2a", mode, 3) == 0) {
        bmp2arr(argv[1]);
    } else {
        printf("invalide mode '%s', mode: a2b or b2a \n  Usage: i2a <file path> <mode>\n", mode);
        return 1;
    }

    /* printf("end\n"); */
    return 0;
}