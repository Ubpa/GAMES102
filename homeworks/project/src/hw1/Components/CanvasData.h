/*
������е�����
*/
#pragma once

#include <UGM/UGM.h>

struct CanvasData {
	std::vector<Ubpa::pointf2> points;
	Ubpa::valf2 scrolling{ 0.f,0.f };
	bool opt_enable_grid{ true };  // �Ƿ���ʾ����
	bool opt_enable_context_menu{ true };  // �Ƿ����Ҽ��˵���
	
	bool func_power{ true };  // �ݺ���
};

#include "details/CanvasData_AutoRefl.inl"
