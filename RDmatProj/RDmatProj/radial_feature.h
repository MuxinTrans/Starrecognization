#pragma once

// ����
#define FOC_LENGTH 5000		// focal length ����
#define PI 3.1416
#define pi 3.1416
/*
* ��Ԫ��λ��5um����Ԫ��Χ��2048x2048
* ���ࣺ25mm			===> ���귶Χ��2048x2048������5_000
*/

// ��������
// ������������
double Mod_3D(double* a);
double Dot_3D(double* a, double* b);
void Normold_3D(double* a, double* an);

// ����任
void coor_translation(double(*a));
void coor_starsensor(double(*a), double(*as));

// �Ƕȱ任
float rad2deg(float rad);
float deg2rad(float deg);

// ��������
double Cal_Angdis(double* a, double* b);
