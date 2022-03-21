#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "top_define.h"
#include "top_data.h"

// 用来存储原始星点坐标
struct _StarCoor {
	int num;		// num取值：1~STAR_NUM_LIB
	double coor[3];
}STAR[STAR_NUM_LIB];

// 用来读取数据库里星点的径向特征。
struct _PatrFeature {
	int num;		// num取值：1~Nq
	int star_list[MAX_PATR];		// star_list的取值：为星点序号[1~STAR_NUM_LIB]，即为下标+1
}Patr[Nq];

// 拍摄星图的星点信息：坐标、径向特征、径向特征的星点信息。
struct _Starsensor {
	int num;			// 星点下标:从1开始计数到STAR_NUM，表明这是图中第几颗星星的模式。
	double coor[3];		// 存储星点在星敏感器坐标系下的三维直角坐标
	unsigned char patr[Nq];		// 用来保存模式，使用string形式存储【遍历星图中的其他星点，判断他们所在圈数并存储】
	int list[STAR_NUM_LIB];		// 星点计数器【判断在patr[Nq]给出的圈数中，有邻域的主星的出现次数】
	int starnum_list[STAR_NUM_LIB];		// 星点计数器计数值最大和次大的星点序列。		// 即：可能是num星的星们
	int starnum_sum;					// 上述星点序列的星点个数
}StarPic_Patr[STAR_NUM];

/*
* 将matlab生成的星点信息.mat文件加载出来。信息为：star_num[num],x,y,z[coordinate]
*/
void LoadStarData(void) {
	int i = 0, j = 0;

	// step1: 打开MAT文件
	mat_t* matfp;
	const char* file;		// MAT文件名
	file = "star_data_raw_5mag_new.mat";
	const int column = 2;	// MAT文件对应的struct的变量数。
							// ·num		·coordinate

	matfp = Mat_Open(file, MAT_ACC_RDONLY);
	if (NULL == matfp) {
		printf("Error opening MAT file %s!\n", file);
	}

	// step2: 读取MAT文件中保存的数据（double、matrix、cell、struct）
	matvar_t* structvar_patr = NULL, * starcoor = NULL;			// 初始化保存数据的指针：structvar_patr -- 对应于整个struct数据。
																//					   starcoor -- 对应struct中的第二列（第一列可以直接用idx代替）。
	structvar_patr = Mat_VarRead(matfp, "star_data_5mag_reload");
	if (!structvar_patr) {
		printf("Error read MatVar: star_data_5mag_reload!\n");
	}
	else if (structvar_patr->data_type == MAT_T_STRUCT) {		// 判断读取到的内容是否为结构体
		const int list_length = structvar_patr->nbytes / structvar_patr->data_size / column;
		for (i = 0; i < list_length; i++) {
			starcoor = Mat_VarGetStructFieldByName(structvar_patr, "coordinate", i);
			if (starcoor) {
				const double* xData_star = (const double*)(starcoor->data);
				const int iDataN = starcoor->nbytes / starcoor->data_size;
				for (j = 0; j < iDataN; j++) {
					STAR[i].coor[j] = xData_star[j];
				}
				STAR[i].num = i + 1;
			}
			starcoor = NULL;
		}
	}

	Mat_VarFree(starcoor);						// 释放空间，关闭文件
	Mat_VarFree(structvar_patr);
	Mat_Close(matfp);
}

/*
* 将matlab生成的星库的径向特征.mat文件加载出来。
*/
void LoadPatrData(void) {
	int i = 0, j = 0;
	//	struct _PatrFeature Patr[Nq];

		// step1: 打开MAT文件
	mat_t* matfp;
	const char* file;		// MAT文件名
	file = "patr_list_5mag.mat";
	const int column = 2;	// MAT文件对应的struct的变量数。

	matfp = Mat_Open(file, MAT_ACC_RDONLY);
	if (NULL == matfp) {
		printf("Error opening MAT file %s!\n", file);
	}

	// step2: 读取MAT文件中保存的数据（double、matrix、cell、struct）
	matvar_t* structvar_patr = NULL, * starlist = NULL;			// 初始化保存数据的指针：structvar_patr -- 对应于整个struct数据。
																//					   starlist -- 对应struct中的第二列（第一列可以直接用idx代替）。
	structvar_patr = Mat_VarRead(matfp, "patr_list_5mag");
	if (!structvar_patr) {
		printf("Error read MatVar: patr_list_5mag!\n");
	}
	else if (structvar_patr->data_type == MAT_T_STRUCT) {		// 判断读取到的内容是否为结构体
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
* 获得拍摄的星图上星点的径向特征，将星点结果存入_Starsensor结构体
* input: _Starsensor结构体指针
*		 star_idx星点下标
* output: 输出的参数通过结构体指针传递
* Version: 18th March 2022
*/
void Patr_PicStar(struct _Starsensor* str_addr) {
	int i = 0, j = 0, x = 0;
	for (i = 0; i < STAR_NUM; i++) {
		if (i != ((str_addr->num) - 1)) {		// 遍历星星
			double dis_angle;		// 输入星点与当前循环星点的星对角距
			dis_angle = Cal_Angdis(str_addr->coor, StarPic_Patr[i].coor);
			if ((dis_angle <= MAX_AngDis) && (dis_angle > MIN_AngDis)) {
				double idx_double;
				unsigned char idx_n;
				idx_double = ceil(dis_angle / Dr);
				idx_n = (char)idx_double;
				char* find;				// 查找径向模式中是否有这个标识符的标志
				find = strchr(str_addr->patr, idx_n);
				if (NULL == find) {
					char tmp[2] = { idx_n,'\0' };
					strcat_s(str_addr->patr, sizeof(str_addr->patr), tmp);
					int idx_int = (int)idx_n;		// 表示第idx_int圈有星星
					int idx_min, idx_max;
					idx_min = idx_int - 10; idx_max = idx_int + 10;
					idx_min = (idx_min < 1) ? 1 : idx_min;
					idx_max = (idx_max > Nq) ? Nq : idx_max;
					for (x = idx_min; x <= idx_max; x++) {
						for (j = 0; Patr[x - 1].star_list[j] != 0; j++) {
							int star_num = Patr[x - 1].star_list[j];	// 在对应星环存在星星的星点标号，标号为下标+1
							str_addr->list[star_num - 1] = str_addr->list[star_num - 1] + 1;
						}
					}

//					for (j = 0; Patr[idx_int - 1].star_list[j] != 0; j++) {
//						int star_num = Patr[idx_int - 1].star_list[j];	// 在对应星环存在星星的星点标号，标号为下标+1
//						str_addr->list[star_num - 1] = str_addr->list[star_num - 1] + 1;
//					}
//					if (idx_int != 1) {
//						int idx_tmp = idx_int - 1;
//						for (j = 0; Patr[idx_tmp - 1].star_list[j] != 0; j++) {
//							int star_num = Patr[idx_tmp - 1].star_list[j];	// 在对应星环存在星星的星点下标
//							str_addr->list[star_num - 1] = str_addr->list[star_num - 1] + 1;
//						}
//					}
//					if (idx_int != Nq) {
//						int idx_tmp = idx_int + 1;
//						for (j = 0; Patr[idx_tmp - 1].star_list[j] != 0; j++) {
//							int star_num = Patr[idx_tmp - 1].star_list[j];	// 在对应星环存在星星的星点下标
//							str_addr->list[star_num - 1] = str_addr->list[star_num - 1] + 1;
//						}
//					}
				}
			}
		}
	}
}

/*
* 对拍摄的星图上星点的径向特征的星序进行排列，并选出待识别星点的待选星序列，将待选星序列与待选星数量都存入_Starsensor结构体中
* input: _Starsensor结构体指针
*		 star_idx星点下标
* output: 输出的参数通过结构体指针传递
* Version: 20th March 2022
*/
void PatrStarlist_PicStar(struct _Starsensor* str_addr) {
	int i = 0, j = 0;
	int array_index[STAR_NUM_LIB];					// 初始化为从0到STAR_NUM_LIB-1的顺序序列，后面用来存储排列后的星序。
	array_init(array_index, STAR_NUM_LIB);
	sort(str_addr->list, STAR_NUM_LIB, array_index);// 排序后，list中存储值为星点计数器从大到小计数值，array_index为该计数值对应的星点的下标（为实际星序-1）
	int maxcount = str_addr->list[0];				// 获得最大计数值
//	while (str_addr->list[i] > maxcount - 2) {		// 取得计数值为最大计数值及次大计数值的星星序列
	while (str_addr->list[i] > 0) {					// 取得计数值为最大计数值及次大计数值的星星序列
		str_addr->starnum_list[j] = array_index[i] + 1;
		i++; j++;
	}
	str_addr->starnum_list[j] = 0;
	str_addr->starnum_sum = j;
}

int main() {
	int i = 0, j = 0;
	LoadStarData();		// struct _StarCoor STAR[STAR_NUM_LIB]	||	加载星库信息，含有：星序（比下标多1）、星点坐标（以double数组形式存储）
	LoadPatrData();		// struct _PatrFeature Patr[Nq]			||	星库中所有星点的径向特征，含有：星序（比下标多1）、在第i圈有星星的所有星点的序列
						//											（以int数组形式存储，以0作为结束标志）。

	// 遍历拍摄的图像上的每一颗星，将其坐标从xyz形式转换为double数组形式，并存入结构体中
	// struct _Starsensor StarPic_Patr
	for (i = 0; i < STAR_NUM; i++) {
		double a[3], b[2];
		b[0] = pic1[i].x; b[1] = pic1[i].y;
		coor_starsensor(b, a);			// 将坐标从拍摄的星图照片的二维平面坐标转为星敏感器坐标系下的三维坐标。
		StarPic_Patr[i].coor[0] = a[0];
		StarPic_Patr[i].coor[1] = a[1];
		StarPic_Patr[i].coor[2] = a[2];
		StarPic_Patr[i].num = i + 1;	// 存储星号，取值范围为1~STAR_NUM
	}

	// 遍历图像上的每颗星，判断它们的星模式。
	int offset_num = 0;		// 表示现在正在遍历结构体中的第offset_num个元素。
	for (offset_num = 0; offset_num < STAR_NUM; offset_num++) {	
		Patr_PicStar(StarPic_Patr + offset_num);			// 传递的参数为：结构名+index		// 构建图像上每个星点的径向特征
		PatrStarlist_PicStar(StarPic_Patr + offset_num);	// 由径向特征得到其可能是那些星点（待选序列）
	}

	int m = 0, cnt = 0;
	int star1, star2, star3;
	for (i = 0; i < StarPic_Patr[0].starnum_sum; i++) {
		double d12;			// 拍摄的星图上两颗星的角距。
		d12 = Cal_Angdis(StarPic_Patr[0].coor, StarPic_Patr[1].coor);
		double d23 = Cal_Angdis(StarPic_Patr[2].coor, StarPic_Patr[1].coor);
		double d13 = Cal_Angdis(StarPic_Patr[2].coor, StarPic_Patr[0].coor);
		for (j = 0; j < StarPic_Patr[1].starnum_sum; j++) {
			double dij;		// 星库中两颗星的角距。
//			int star1, star2;
			star1 = StarPic_Patr[0].starnum_list[i];
			star2 = StarPic_Patr[1].starnum_list[j];
			dij = Cal_Angdis(STAR[star1 - 1].coor, STAR[star2 - 1].coor);
			if (fabs(dij - d12) <= DeltDrad) {
//				printf("The Star Pair is %d\t and %d\n", star1, star2);
				for (m = 0; m < StarPic_Patr[2].starnum_sum; m++) {
					star3 = StarPic_Patr[2].starnum_list[m];
					double djk = Cal_Angdis(STAR[star3 - 1].coor, STAR[star2 - 1].coor);
					if (fabs(djk - d23) <= DeltDrad) {
						double dki = Cal_Angdis(STAR[star3 - 1].coor, STAR[star1 - 1].coor);
						if (fabs(dki - d13) <= DeltDrad) {
							printf("The Star Pair is %d,\t %d\t and %d\n", star1, star2, star3);
							cnt++;
						}
					}
				}
			}
		}
	}

//	int star1 = 78, star2 = 303, star3 = 549;
//	int star4;
//	int star5, star6, star7;
//	int m = 0, cnt = 0;
//	for (i = 0; i < StarPic_Patr[3].starnum_sum; i++) {
//		double d14;			// 拍摄的星图上两颗星的角距。
//		d14 = Cal_Angdis(StarPic_Patr[0].coor, StarPic_Patr[3].coor);
//		double d24 = Cal_Angdis(StarPic_Patr[1].coor, StarPic_Patr[3].coor);
//		double d34 = Cal_Angdis(StarPic_Patr[2].coor, StarPic_Patr[3].coor);
//		double dij;		// 星库中两颗星的角距。
//		star4 = StarPic_Patr[3].starnum_list[i];
//		dij = Cal_Angdis(STAR[star4 - 1].coor, STAR[star3 - 1].coor);
////		if (fabs(dij - d14) <= DeltDrad) {
//		if (fabs(dij - d34) <= 0.01) {
//			printf("The Star Pair is %d\t and %d\n", star3, star4);
//			//for (m = 0; m < StarPic_Patr[2].starnum_sum; m++) {
//			//	star3 = StarPic_Patr[2].starnum_list[m];
//			//	double djk = Cal_Angdis(STAR[star3 - 1].coor, STAR[star2 - 1].coor);
//			//	if (fabs(djk - d23) <= DeltDrad) {
//			//		double dki = Cal_Angdis(STAR[star3 - 1].coor, STAR[star1 - 1].coor);
//			//		if (fabs(dki - d13) <= DeltDrad) {
//			//			printf("The Star Pair is %d,\t %d\t and %d\n", star1, star2, star3);
//			//			cnt++;
//			//		}
//			//	}
//			//}
//		}
//		
//	}


//	if (cnt != 0) {
//
//	}

	printf("\n\nHello world!\n");

	return 0;
}
