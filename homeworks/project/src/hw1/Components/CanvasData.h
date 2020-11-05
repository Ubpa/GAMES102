/*
存放所有的数据
*/
#pragma once

#include <UGM/UGM.h>

struct CanvasData {
	Ubpa::valf2 scrolling{ 0.f,0.f };
	std::vector<Ubpa::pointf2> points;
	std::vector<ImVec2> LagrangeResults;  // 拉格朗日插值的点
	std::vector<ImVec2> GaussResults;  // 高斯插值的点
	std::vector<ImVec2> LeastSquaresResults;  // 最小二乘拟合的点
	std::vector<ImVec2> RidgeRegressionResults;  // 岭回归拟合的点
	
	bool opt_enable_grid{ true };  // 是否显示网格
	bool opt_enable_context_menu{ true };  // 是否开启右键菜单栏
	bool opt_lagrange{ true };  // 拉格朗日插值
	bool opt_gauss{ true };  // 高斯插值
	bool opt_least_squares{ true };  // 最小二乘拟合
	bool opt_ridge_regression{ true };
	float GaussTheta = 100;  // 高斯函数有多扁的程度
	int LeastSquaresM = 1;  // 多项式数  m-1为最高次项的幂值
	float RidgeRegressionLambda = 0.1;  // 岭回归中正则项的权重
	int RidgeRegressionM = 1;  // 多项式的个数
};

#include "details/CanvasData_AutoRefl.inl"
