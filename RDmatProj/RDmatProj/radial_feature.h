#pragma once

// 常量
#define FOC_LENGTH 5000		// focal length 焦距
#define PI 3.1416
#define pi 3.1416
/*
* 像元单位：5um，像元范围：2048x2048
* 焦距：25mm			===> 坐标范围：2048x2048，焦距5_000
*/

// 函数声明
// 基本参数计算
double Mod_3D(double* a);
double Dot_3D(double* a, double* b);
void Normold_3D(double* a, double* an);

// 坐标变换
void coor_translation(double(*a));
void coor_starsensor(double(*a), double(*as));

// 角度变换
float rad2deg(float rad);
float deg2rad(float deg);

// 特征计算
double Cal_Angdis(double* a, double* b);
