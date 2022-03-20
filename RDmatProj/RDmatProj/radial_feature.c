#include "radial_feature.h"
#include <math.h>

/*
* ����������������ԭ������Ͻ��ƶ�������ƽ������.
* ԭ��Ԫ���ģ�(0,0)------>�任����Ԫ���ģ�(1024,1024)
* input: ת��ǰ������&ת���������
* output: void
*/
void coor_translation(double (*a)) {
	a[0] = a[0] - 1024;
	a[1] = -1.0 * (a[1] - 1024);
	
	return 0;
}

/*
* ��������������ƽ�����꣨��ά��ת����������������ϵ����ά����
* -������������ϵ������ԭ�㣺(1024,1024)��xyz�᣺====
* input: a: ��ת����ͼ��ƽ�����ꡣ��ά���ꡣ����ԭ�㣺(0,0)
*		 as: ת����������������ꡣ��ά���ꡣ����ԭ�㣺(1024,1024,0)��mod=1��
* output: void
*/
void coor_starsensor(double(*a),double (* as)) {
	double a_tmp[3];
	coor_translation(a);	// ��ͼ������������Ƶ�ƽ������
	a_tmp[0] = a[0];
	a_tmp[1] = a[1];
	a_tmp[2] = (-1) * FOC_LENGTH;	// δ��һ����������������ϵ�µ���ά����
	Normold_3D(a_tmp, as);			// ����ά������й�һ����
}

/*
* ��������a������b�ĽǾ�
* input: a��b���������꣨��άֱ�����꣩
* output: ����ĽǾ�
*/
double Cal_Angdis(double* a, double* b) {
	double moda, modb;	// a,b������ģ
	double dotab;		// ����ab�ĵ��
	double dis_ab = 0;		// ����ab�ĽǾ�

	moda = Mod_3D(a);
	modb = Mod_3D(b);
	dotab = Dot_3D(a, b);
	dis_ab = acos(dotab / moda / modb);

	return dis_ab;
}

/*
* ��������a��ģ��aΪ��ά������
* input: a�����꣨��ά������
* output: ����a��ģ
*/
double Mod_3D(double* a) {
	double moda;
	moda = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	return moda;
}

/*
* ����������ά�����ĵ������˽������
* input: a,b���������꣨��άֱ�����꣩
* output: ab�������
*/
double Dot_3D(double* a, double* b) {
	double dotab;
	dotab = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	return dotab;
}

/*
* ����ά������һ����
* input: a: �����δ��һ������άֱ�����ꡣ
*		 an: ��һ�������άֱ�����ꡣ
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
* �����ȱ�ʾת��Ϊ�Ƕȱ�ʾ
* input: rad�����ȱ�ʾ
* output: deg���Ƕȱ�ʾ
*/
float rad2deg(float rad) {
	float deg;
	deg = rad * 180.0 / PI;
	return deg;
}

/*
* ���Ƕȱ�ʾת��Ϊ���ȱ�ʾ
* input: deg���Ƕȱ�ʾ
* output: rad�����ȱ�ʾ
*/
float deg2rad(float deg) {
	float rad;
	rad = deg * PI / 180.0;
	return rad;
}