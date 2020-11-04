/*
������е�����
*/
#pragma once

#include <UGM/UGM.h>

struct CanvasData {
	Ubpa::valf2 scrolling{ 0.f,0.f };
	std::vector<Ubpa::pointf2> points;
	std::vector<ImVec2> LagrangeResults;  // �������ղ�ֵ�ĵ�
	
	bool opt_enable_grid{ true };  // �Ƿ���ʾ����
	bool opt_enable_context_menu{ true };  // �Ƿ����Ҽ��˵���
	
	bool opt_lagrange{ true };  // �������ղ�ֵ
};

#include "details/CanvasData_AutoRefl.inl"
