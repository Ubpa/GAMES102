/*
������е�����
*/
#pragma once

#include <UGM/UGM.h>

struct CanvasData {
	Ubpa::valf2 scrolling{ 0.f,0.f };
	std::vector<Ubpa::pointf2> points;
	std::vector<ImVec2> LagrangeResults;  // �������ղ�ֵ�ĵ�
	std::vector<ImVec2> GaussResults;  // ��˹��ֵ�ĵ�
	std::vector<ImVec2> LeastSquaresResults;  // ��С������ϵĵ�
	
	bool opt_enable_grid{ true };  // �Ƿ���ʾ����
	bool opt_enable_context_menu{ true };  // �Ƿ����Ҽ��˵���
	
	bool opt_lagrange{ true };  // �������ղ�ֵ
	bool opt_gauss{ true };  // ��˹��ֵ
	bool opt_least_squares{ true };  // ��С�������

	float GaussTheta;
	int LeastSquaresM;  // ����ʽ��
};

#include "details/CanvasData_AutoRefl.inl"
