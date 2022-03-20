#include "mathlibr.h"

/*
* �Ӵ�С����
* input: a: ����������
*        b: ������������±����飨����0~length˳�������
*        length: ����������ĳ���
* output: none. ͨ�����鴫�ݽ�������У�����aΪ���������У�����bΪ�����±ꡣ
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
* ������0~length-1��˳�����顣
* input: length: ����ʼ������ĳ��ȡ�
*        a: �����洢��ʼ�����ֵ��
*/
void array_init(int* a, int length) {
    int i = 0;
    for (i = 0; i < length; i++) {
        a[i] = i;
    }
}