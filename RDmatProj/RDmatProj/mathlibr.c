#include "mathlibr.h"

/*
* 从大到小排序。
* input: a: 待排序数组
*        b: 待排序数组的下标数组（即从0~length顺序递增）
*        length: 待排序数组的长度
* output: none. 通过数组传递结果。其中，数组a为排序后的序列，数组b为序列下标。
*/
void sort(int* a, int length, int* b)
{
    int i, j, t1, t;
    for (j = 0; j < length; j++) {
        for (i = 0; i < length - 1 - j; i++) {
            if (a[i] < a[i + 1])
            {
                t = a[i];
                a[i] = a[i + 1];
                a[i + 1] = t;


                t1 = b[i];
                b[i] = b[i + 1];
                b[i + 1] = t1;
            }
        }
    }
}

/*
* 产生从0~length-1的顺序数组。
* input: length: 待初始化数组的长度。
*        a: 用来存储初始化后的值。
*/
void array_init(int* a, int length) {
    int i = 0;
    for (i = 0; i < length; i++) {
        a[i] = i;
    }
}