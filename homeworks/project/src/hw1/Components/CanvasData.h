/*
存放所有的数据
*/
#pragma once

#include <UGM/UGM.h>

struct CanvasData {
	Ubpa::valf2 scrolling{ 0.f,0.f };
	std::vector<Ubpa::pointf2> points;
	std::vector<ImVec2> LagrangeResults;  // 拉格朗日插值的点
	
	bool opt_enable_grid{ true };  // 是否显示网格
	bool opt_enable_context_menu{ true };  // 是否开启右键菜单栏
	
	bool opt_lagrange{ true };  // 拉格朗日插值
};

#include "details/CanvasData_AutoRefl.inl"
