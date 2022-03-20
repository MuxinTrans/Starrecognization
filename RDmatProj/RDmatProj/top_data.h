#pragma once

#include "starinfo.h"			// 获取星图中的星点坐标

#define STAR_NUM 7				// 星点数量（每幅图的星点个数）
#define MAX_PATR 455			// 径向特征中，单项的最多星点个数 (453+2)
#define STAR_NUM_LIB 1629		// 星等为5的星星总数	

#define RrDeg 10				// 径向特征模式半径选为10°
#define Rr 0.17453				// 径向特征模式半径的弧度表示；C语言计算结果为0.174533		// 上限
#define Nq 200					// 径向特征细分等级为200
#define Dr 0.00087265			// 径向特征带宽,Rr/Nq
#define MAX_AngDis 0.17453		// Rr
#define MIN_AngDis 0			// 下限