#include "radial_feature.h"
#include <math.h>

/*
* 将星敏感器的坐标原点从左上角移动到成像平面中心.
* 原像元中心：(0,0)------>变换后像元中心：(1024,1024)
* input: 转换前的坐标&转换后的坐标
* output: void
*/
void coor_translation(double (*a)) {
	a[0] = a[0] - 1024;
	a[1] = -1.0 * (a[1] - 1024);
	
	return 0;
}

/*
* 将星敏感器成像平面坐标（二维）转换到星敏感器坐标系（三维）。
* -星敏感器坐标系：坐标原点：(1024,1024)，xyz轴：====
* input: a: 待转换的图像平面坐标。二维坐标。坐标原点：(0,0)
*		 as: 转换后的星敏感器坐标。三维坐标。坐标原点：(1024,1024,0)。mod=1。
* output: void
*/
void coor_starsensor(double(*a),double (* as)) {
	double a_tmp[3];
	coor_translation(a);	// 将图像坐标的中心移到平面中心
	a_tmp[0] = a[0];
	a_tmp[1] = a[1];
	a_tmp[2] = (-1) * FOC_LENGTH;	// 未归一化的星敏感器坐标系下的三维坐标
	Normold_3D(a_tmp, as);			// 对三维坐标进行归一化。
}

/*
* 计算向量a和向量b的角距
* input: a和b的向量坐标（三维直角坐标）
* output: 计算的角距
*/
double Cal_Angdis(double* a, double* b) {
	double moda, modb;	// a,b向量的模
	double dotab;		// 向量ab的点乘
	double dis_ab = 0;		// 向量ab的角距

	moda = Mod_3D(a);
	modb = Mod_3D(b);
	dotab = Dot_3D(a, b);
	dis_ab = acos(dotab / moda / modb);

	return dis_ab;
}

/*
* 计算向量a的模（a为三维向量）
* input: a的坐标（三维向量）
* output: 向量a的模
*/
double Mod_3D(double* a) {
	double moda;
	moda = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	return moda;
}

/*
* 计算两个三维向量的点积（点乘结果）。
* input: a,b向量的坐标（三维直角坐标）
* output: ab向量点积
*/
double Dot_3D(double* a, double* b) {
	double dotab;
	dotab = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	return dotab;
}

/*
* 将三维向量归一化。
* input: a: 输入的未归一化的三维直角坐标。
*		 an: 归一化后的三维直角坐标。
* output: void
*/
void Normold_3D(double* a, double* an) {
	double mod = 0;
	mod = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	if (mod != 0) {
		an[0] = a[0] / mod;
		an[1] = a[1] / mod;
		an[2] = a[2] / mod;
	}
	else {
		an[0] = 0; an[1] = 0; an[2] = 0;
	}
}

/*
* 将弧度表示转换为角度表示
* input: rad，弧度表示
* output: deg，角度表示
*/
float rad2deg(float rad) {
	float deg;
	deg = rad * 180.0 / PI;
	return deg;
}

/*
* 将角度表示转换为弧度表示
* input: deg，角度表示
* output: rad，弧度表示
*/
float deg2rad(float deg) {
	float rad;
	rad = deg * PI / 180.0;
	return rad;
}