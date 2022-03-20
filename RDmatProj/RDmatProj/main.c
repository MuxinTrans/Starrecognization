#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "top_define.h"
#include "top_data.h"

// ������ȡ���ݿ����ǵ�ľ���������
struct _PatrFeature {
	int num;
	int star_list[MAX_PATR];
}Patr[Nq];

// ������ͼ���ǵ���Ϣ�����ꡢ���������������������ǵ���Ϣ��
struct _Starsensor {
	double coor[3];		// �洢�ǵ���������������ϵ�µ���άֱ������
	unsigned char patr[Nq];		// ��������ģʽ��ʹ��string��ʽ�洢
	int list[STAR_NUM_LIB];		// �ǵ������
	int num;			// �ǵ��±�:��1��ʼ������STAR_NUM
	int starnum_list[STAR_NUM_LIB];		// �ǵ����������ֵ���ʹδ���ǵ����С�
}StarPic_Patr[STAR_NUM];

/*
* ��matlab���ɵ��ǿ�ľ�������.mat�ļ����س�����
*/
void LoadPatrData(void) {
	int i = 0, j = 0;
	//	struct _PatrFeature Patr[Nq];

		// step1: ��MAT�ļ�
	mat_t* matfp;
	const char* file;		// MAT�ļ���
	file = "patr_list_9p_5mag.mat";
	const int column = 2;	// MAT�ļ���Ӧ��struct�ı�������

	matfp = Mat_Open(file, MAT_ACC_RDONLY);
	if (NULL == matfp) {
		printf("Error opening MAT file %s!\n", file);
	}

	// step2: ��ȡMAT�ļ��б�������ݣ�double��matrix��cell��struct��
	matvar_t* structvar_patr = NULL, * starlist = NULL;			// ��ʼ���������ݵ�ָ�룺structvar_patr -- ��Ӧ������struct���ݡ�
																//					   starlist -- ��Ӧstruct�еĵڶ��У���һ�п���ֱ����idx���棩��
	structvar_patr = Mat_VarRead(matfp, "patr_list_9_5");
	if (!structvar_patr) {
		printf("Error read MatVar: patr_list_9_5!\n");
	}
	else if (structvar_patr->data_type == MAT_T_STRUCT) {		// �ж϶�ȡ���������Ƿ�Ϊ�ṹ��
		const int list_length = structvar_patr->nbytes / structvar_patr->data_size / column;
		for (i = 0; i < list_length; i++) {
			starlist = Mat_VarGetStructFieldByName(structvar_patr, "starlist", i);
			if (starlist) {
				const double* xData_star = (const double*)(starlist->data);
				const int iDataN = starlist->nbytes / starlist->data_size;
				for (j = 0; j < iDataN; j++) {
					Patr[i].star_list[j] = (int)(xData_star[j]);
				}
				Patr[i].num = i + 1;
				Patr[i].star_list[iDataN] = 0;
			}
			starlist = NULL;
		}
	}

	Mat_VarFree(structvar_patr);
	Mat_VarFree(starlist);
	Mat_Close(matfp);
}

/*
* ����������ͼ���ǵ�ľ������������ǵ�������_Starsensor�ṹ��
* input: _Starsensor�ṹ��ָ��
*		 star_idx�ǵ��±�
* output: ����Ĳ���ͨ���ṹ��ָ�봫��
* Version: 18th March 2022
*/
void Patr_PicStar(struct _Starsensor* str_addr) {
	int i = 0, j = 0;
	for (i = 0; i < STAR_NUM; i++) {
		if (i != ((str_addr->num) - 1)) {
			double dis_angle;		// �����ǵ��뵱ǰѭ���ǵ���ǶԽǾ�
			dis_angle = Cal_Angdis(str_addr->coor, StarPic_Patr[i].coor);
			if ((dis_angle <= MAX_AngDis) && (dis_angle > MIN_AngDis)) {
				double idx_double;
				unsigned char idx_n;
				idx_double = ceil(dis_angle / Dr);
				idx_n = (char)idx_double;
				char* find;				// ���Ҿ���ģʽ���Ƿ��������ʶ���ı�־
				find = strchr(str_addr->patr, idx_n);
				if (NULL == find) {
					char tmp[2] = { idx_n,'\0' };
					//				strcpy_s(tmp, 200, str_addr->patr);
					strcat_s(str_addr->patr, sizeof(str_addr->patr), tmp);
					int idx_int = (int)idx_n;
					for (j = 0; Patr[idx_int - 1].star_list[j] != 0; j++) {
						int star_num = Patr[idx_int - 1].star_list[j];	// �ڶ�Ӧ�ǻ��������ǵ��ǵ��±�
						str_addr->list[star_num - 1] = str_addr->list[star_num - 1] + 1;
					}
					if (idx_int != 1) {
						int idx_tmp = idx_int - 1;
						for (j = 0; Patr[idx_tmp - 1].star_list[j] != 0; j++) {
							int star_num = Patr[idx_tmp - 1].star_list[j];	// �ڶ�Ӧ�ǻ��������ǵ��ǵ��±�
							str_addr->list[star_num - 1] = str_addr->list[star_num - 1] + 1;
						}
					}
					if (idx_int != Nq) {
						int idx_tmp = idx_int + 1;
						for (j = 0; Patr[idx_tmp - 1].star_list[j] != 0; j++) {
							int star_num = Patr[idx_tmp - 1].star_list[j];	// �ڶ�Ӧ�ǻ��������ǵ��ǵ��±�
							str_addr->list[star_num - 1] = str_addr->list[star_num - 1] + 1;
						}
					}
				}
			}
		}
	}
}

int main() {
	int i = 0, j = 0;
	LoadPatrData();

	for (i = 0; i < STAR_NUM; i++) {
		double a[3], b[2];
		b[0] = pic1[i].x; b[1] = pic1[i].y;
		coor_starsensor(b, a);
		StarPic_Patr[i].coor[0] = a[0];
		StarPic_Patr[i].coor[1] = a[1];
		StarPic_Patr[i].coor[2] = a[2];
		StarPic_Patr[i].num = i + 1;
	}

	int offset_num = 0;
	for (offset_num = 0; offset_num < STAR_NUM; offset_num++) {		// ����ͼ���ϵ�ÿ���ǣ��ж����ǵ���ģʽ��
		Patr_PicStar(StarPic_Patr + offset_num);		// ���ݵĲ���Ϊ���ṹ��+index

		int array_index[STAR_NUM_LIB];
		array_init(array_index, STAR_NUM_LIB);
		sort(StarPic_Patr[offset_num].list, STAR_NUM_LIB, array_index);
		int maxcount = StarPic_Patr[offset_num].list[0];
		i = 0; j = 0;
		while (StarPic_Patr[offset_num].list[i]>maxcount-2) {
			StarPic_Patr[offset_num].starnum_list[j] = array_index[i]+1;
			i++; j++;
		}
//		printf("Sort\t\tIndex\n");
//		for (i = 0; i < STAR_NUM_LIB; i++) {
//			if (StarPic_Patr[offset_num].list[i] > 0)
//				printf("%d\t\t%d\n", StarPic_Patr[offset_num].list[i], array_index[i]);
//		}
	}

	printf("\n\nHello world!\n");

	return 0;
}
