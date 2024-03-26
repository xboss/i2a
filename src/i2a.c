#include <assert.h>
#include <stdio.h>
#include <string.h>

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

int main(int argc, char const *argv[]) {
    /* printf("start\n"); */
    if (argc != 2) {
        printf("传入的参数格式: i2a <文件名称>\n");
        return 1;
    }

    i2a_bmp_t *bmp = i2a_load_bmp(argv[1]);
    assert(bmp);
    print_bmp_info(bmp);

    /* convert to array */
    i2a_bmp_conf_t b2a_conf = {.arr_type = I2A_ARR_TYPE_RGB16};
    i2a_array_t arr;
    bzero(&arr, sizeof(i2a_array_t));
    uint32_t r = i2a_bmp2array(bmp, &b2a_conf, &arr);
    assert(r > 0);

    /* printf("------ array ------\n"); */

    print_array(bmp, &arr);

    i2a_free(bmp);
    /* printf("end\n"); */
    return 0;
}