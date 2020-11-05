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
	std::vector<ImVec2> RidgeRegressionResults;  // ��ع���ϵĵ�
	
	bool opt_enable_grid{ true };  // �Ƿ���ʾ����
	bool opt_enable_context_menu{ true };  // �Ƿ����Ҽ��˵���
	bool opt_lagrange{ true };  // �������ղ�ֵ
	bool opt_gauss{ true };  // ��˹��ֵ
	bool opt_least_squares{ true };  // ��С�������
	bool opt_ridge_regression{ true };
	float GaussTheta = 100;  // ��˹�����ж��ĳ̶�
	int LeastSquaresM = 1;  // ����ʽ��  m-1Ϊ��ߴ������ֵ
	float RidgeRegressionLambda = 0.1;  // ��ع����������Ȩ��
	int RidgeRegressionM = 1;  // ����ʽ�ĸ���
};

#include "details/CanvasData_AutoRefl.inl"
