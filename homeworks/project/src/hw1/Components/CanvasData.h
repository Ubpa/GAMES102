/*
存放所有的数据
*/
#pragma once

#include <UGM/UGM.h>

struct CanvasData {
	std::vector<Ubpa::pointf2> points;
	Ubpa::valf2 scrolling{ 0.f,0.f };
	bool opt_enable_grid{ true };  // 是否显示网格
	bool opt_enable_context_menu{ true };  // 是否开启右键菜单栏
	
	bool func_power{ true };  // 幂函数
};

#include "details/CanvasData_AutoRefl.inl"
