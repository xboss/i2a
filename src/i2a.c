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

static int bmp2arr(const char *file_path, char arr_type) {
    if (!file_path || arr_type <= 0) {
        return -1;
    }

    i2a_bmp_t *bmp = i2a_load_bmp(file_path);
    assert(bmp);
    print_bmp_info(bmp);
    /* convert to array */
    i2a_bmp_conf_t conf = {.arr_type = arr_type};
    i2a_array_t arr;
    memset(&arr, 0, sizeof(i2a_array_t));
    uint32_t r = i2a_bmp_to_array(bmp, &conf, &arr);
    assert(r > 0);
    /* printf("------ array ------\n"); */
    print_array(bmp, &arr);
    i2a_free(bmp);
    return r;
}

static int arr2bmp(const char *file_path, uint32_t width, uint32_t height, uint16_t bit_cnt, char arr_type) {
    if (!file_path || width <= 0 || height <= 0 || bit_cnt == 0 || arr_type == 0) {
        return -1;
    }

    int bit_cnt_bytes = 1;
    if (I2A_BMP_BIT_CNT_24 == bit_cnt) {
        bit_cnt_bytes = 3;
    }

    i2a_bmp_conf_t conf = {.arr_type = arr_type};
    i2a_array_t arr;
    arr.type = arr_type;

    int arr_size = width * height;
    arr.arr_len = i2a_load_array(file_path, arr_size, &arr.arr);
    if (arr.arr_len != arr_size || !arr.arr) {
        printf("load array error. %s\n", file_path);
        return -1;
    }

    /* int i = 0;
    printf("static const uint16_t bmp_arr[] = {");
    for (i = 0; i < arr.arr_len; i++) {
        printf("0x%04x, ", arr.arr[i]);
    }
    printf("}\n"); */

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
    if (r <= 0) {
        printf("convert array to BMP data error. %s\n", file_path);
        return -1;
    }

    int in_file_path_len = strlen(file_path);
    char bmp_file_ext[] = ".bmp";
    char *bmp_file_path = (char *)malloc(in_file_path_len + sizeof(bmp_file_ext));
    memset(bmp_file_path, 0, in_file_path_len + sizeof(bmp_file_ext));
    memcpy(bmp_file_path, file_path, in_file_path_len);
    strncat(bmp_file_path, bmp_file_ext, strlen(bmp_file_ext));
    r = i2a_write_bmp_file(bmp_file_path, &bmp);
    if (r <= 0) {
        printf("write BMP file error. %s\n", bmp_file_path);
        return -1;
    }
    printf("write BMP file ok. %s\n", bmp_file_path);

    return r;
}

static const char *usage =
    "Usage: i2a <mode> <file path> <color mode> <width> <height> <color bit count>\n"
    "    <mode>: a2b or b2a\n"
    "        a2b: Convert C array file to BMP file.\n"
    "        b2a: Convert BMP file to C array file.\n"
    "    <file path>: C array file or BMP file.\n"
    "    <color mode>: The color mode of each pixel in the array. Now only supports the following formats:\n"
    "        rgb16: RRRRRGGGGGGBBBBB (2byte/pixel).\n"
    "    <width>: Image's width. Mandatory in a2b mode.\n"
    "    <height>: Image's height. Mandatory in a2b mode.\n"
    "    <color bit count>: Color depth of BMP file. Mandatory in a2b mode. Now only supports the following formats:\n"
    /*     "        1: 1bit color.\n"
        "        16: 16 colors.\n"
        "        256: 256 colors.\n" */
    "        24: 24 bit full color.\n";

int main(int argc, char const *argv[]) {
    if (argc < 4) {
        printf("%s\n", usage);
        return 1;
    }

    const char *mode = argv[1];
    const char *file_path = argv[2];
    char arr_type = 0x00u;
    if (strncasecmp("rgb16", argv[3], 6) == 0) {
        arr_type = I2A_ARR_TYPE_RGB16;
    }

    uint32_t width = 0u, height = 0u;
    uint16_t bit_cnt = 0u;
    int r;
    if (strncasecmp("a2b", mode, 4) == 0) {
        if (argc < 7) {
            printf("%s\n", usage);
            return 1;
        }

        width = atoi(argv[4]);
        height = atoi(argv[5]);
        if (strncasecmp("1", argv[6], 4) == 0) {
            bit_cnt = I2A_BMP_BIT_CNT_1;
        } else if (strncasecmp("16", argv[6], 4) == 0) {
            bit_cnt = I2A_BMP_BIT_CNT_4;
        } else if (strncasecmp("256", argv[6], 4) == 0) {
            bit_cnt = I2A_BMP_BIT_CNT_8;
        } else if (strncasecmp("24", argv[6], 4) == 0) {
            bit_cnt = I2A_BMP_BIT_CNT_24;
        } else {
            printf("unsupported color depth.\n");
            return 1;
        }
        r = arr2bmp(file_path, width, height, bit_cnt, arr_type);
        if (r <= 0) {
            printf("ERROR: Convert C array file to BMP file.\n");
            return 1;
        }
    } else if (strncasecmp("b2a", mode, 4) == 0) {
        bmp2arr(file_path, arr_type);
    } else {
        printf("%s\n", usage);
        return 1;
    }

    return 0;
}