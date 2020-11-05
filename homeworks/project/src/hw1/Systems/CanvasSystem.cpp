#include "CanvasSystem.h"
#include "../Components/CanvasData.h"
#include <math.h>
#include <_deps/imgui/imgui.h>
#include <eigen3/Eigen/Eigen>
#include <atlstr.h>  // debug only

using namespace Ubpa;

float Lagrange(std::vector<Ubpa::pointf2>points, int x)
{
	int size = points.size();

	float sum = 0;
	for (int k = 0; k < size; k++)
	{
		float base = 1;
		for (int j = 0; j < size; j++)
		{
			if (j == k) { continue; }
			base *= (x - points[j][0]) / (points[k][0] - points[j][0]);
		}
		sum += points[k][1] * base;
	}
	return sum;
}

float Gauss(std::vector<Ubpa::pointf2>points, int x, float theta)
{
	int size = points.size();
	
	// Gb =y  ->  b = G^{-1}b
	Eigen::MatrixXf G(size, size);
	Eigen::VectorXf y(size);
	Eigen::VectorXf b(size);
	for (int row = 0; row < size; row++)
		for (int col = 0; col < size; col++)
			G(row, col) = exp(-((points[row][0] - points[col][0]) * (points[row][0] - points[col][0])) / (2 * theta * theta));
	for (int i = 0; i < size; i++)
		y(i) = points[i][1];

	b = G.inverse() * y;

	float ret = 0;
	for (int i = 0; i < size; i++)
		ret += b[i] * exp(-((x - points[i][0]) * (x - points[i][0])) / (2 * theta * theta));
	
	return ret;
}

float LeastSquares(std::vector<Ubpa::pointf2>points, int x, int m)
{
	int size = points.size();

	// B^{T}Y = B^{T}B\alpha  ->  \alpha = (B^{T}B)^{-1}B^{T}Y
	Eigen::MatrixXf B(size, m);
	Eigen::VectorXf Y(size);
	Eigen::VectorXf alpha(m);
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < m; j++)
			B(i, j) = pow(points[i][0], j);
		Y(i) = points[i][1];
	}
	alpha = (B.transpose() * B).inverse() * B.transpose() * Y;
	
	float ret = 0;
	for (int i = 0; i < m; i++)
		ret += alpha[i] * pow(x, i);

	return ret;
}

float RidgeRegression(std::vector<Ubpa::pointf2>points, int x, int m, float lambda)
{
	int size = points.size();


	return 0;
}

/*
offset: 偏移量, 一般是画布的原点位置
*/
void addLine(ImDrawList* draw_list, ImVec2 offset, std::vector<ImVec2> results, ImU32 color, float thickness)
{
	for (int i = 0; i < results.size() - 1; i++)
		draw_list->AddLine(ImVec2(results[i][0]   + offset.x, results[i][1] + offset.y),
			               ImVec2(results[i+1][0] + offset.x, results[i+1][1] + offset.y), color, thickness);
}

void CanvasSystem::OnUpdate(Ubpa::UECS::Schedule& schedule) {
	schedule.RegisterCommand([](Ubpa::UECS::World* w) {
		auto data = w->entityMngr.GetSingleton<CanvasData>();
		if (!data)
			return;

		if (ImGui::Begin("Canvas")) {
			static bool needReCalculate = true;

			//ImGui::Checkbox("Enable grid", &data->opt_enable_grid);
			//ImGui::Checkbox("Enable context menu", &data->opt_enable_context_menu);
			ImGui::Checkbox("Lagrange", &data->opt_lagrange);

			ImGui::Checkbox("Gauss", &data->opt_gauss);ImGui::SameLine(200); 
			if (ImGui::SliderFloat("Theta", &data->GaussTheta, 0.0f, 300.0f)) { needReCalculate = true; }

			ImGui::Checkbox("Least Squares", &data->opt_least_squares); ImGui::SameLine(200);
			if (ImGui::InputInt("least squares m", &data->LeastSquaresM, 1)) { needReCalculate = true; }
			if (data->LeastSquaresM < 1) { data->LeastSquaresM = 1; } // set the range
			if (data->LeastSquaresM > data->points.size()) { data->LeastSquaresM = data->points.size(); }

			ImGui::Checkbox("Ridge Regression", &data->opt_ridge_regression); ImGui::SameLine(200);
			if (ImGui::InputFloat("lamda", &data->RidgeRegressionLambda, 0.01, 1, 3)) { needReCalculate = true; }  ImGui::Indent(192);
			if (ImGui::InputInt("ridge regression m", &data->RidgeRegressionM, 1)) { needReCalculate = true; }
			if (data->RidgeRegressionM < 1) { data->RidgeRegressionM = 1; }  // set the range
			if (data->RidgeRegressionM > data->points.size()) { data->RidgeRegressionM = data->points.size(); }
			//if (data->RidgeRegressionLambda < 1) data->LeastSquaresM = 1;
			//if (data->RidgeRegressionLambda > data->points.size()) data->LeastSquaresM = data->points.size();
			ImGui::Unindent(192);

			// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
			ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
			ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
			if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
			if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
			ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

			// Draw border and background color
			ImGuiIO& io = ImGui::GetIO();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));  // background color
			draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(0, 0, 255, 255));  // border

			// This will catch our interactions
			ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			const bool is_hovered = ImGui::IsItemHovered(); // Hovered
			const bool is_active = ImGui::IsItemActive();   // Held
			const ImVec2 origin(canvas_p0.x + data->scrolling[0], canvas_p0.y + data->scrolling[1]); // canvas的位置
			const pointf2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);  // 鼠标点击位置减去canvas位置, 得到相对于canvas的位置

			
			// 添加点
			if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				data->points.push_back(mouse_pos_in_canvas);
				needReCalculate = true;  // 需要重新计算绘制的点
			}

			// Draw points 画点
			for (int n = 0; n < data->points.size(); n++)
				draw_list->AddCircleFilled(data->points[n] + origin, 5.0f, IM_COL32(0, 255, 255, 200));

			// Pan (we use a zero mouse threshold when there's no context menu)
			// You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
			const float mouse_threshold_for_pan = data->opt_enable_context_menu ? -1.0f : 0.0f;
			if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
			{
				data->scrolling[0] += io.MouseDelta.x;  // ?
				data->scrolling[1] += io.MouseDelta.y;
			}

			// Context menu (under default mouse threshold)
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
			if (data->opt_enable_context_menu && ImGui::IsMouseReleased(ImGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
				ImGui::OpenPopupContextItem("context");
			if (ImGui::BeginPopup("context"))
			{
				if (ImGui::MenuItem("Remove one", NULL, false, data->points.size() > 0)) { data->points.resize(data->points.size() - 1); needReCalculate = true; }
				if (ImGui::MenuItem("Remove all", NULL, false, data->points.size() > 0)) { data->points.clear(); needReCalculate = true; }
				ImGui::EndPopup();
			}

			// Draw grid + all lines in the canvas
			draw_list->PushClipRect(canvas_p0, canvas_p1, true);
			if (data->opt_enable_grid)
			{
				const float GRID_STEP = 64.0f;
				for (float x = fmodf(data->scrolling[0], GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
					draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(0, 200, 0, 40));
				for (float y = fmodf(data->scrolling[1], GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
					draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(0, 200, 0, 40));
			}
			draw_list->PopClipRect();

			// 计算与绘制
			// if add new points or remove points, 则需要更新要绘制的点
			if (needReCalculate && data->points.size() >= 2)
			{
				// 找出要画的线条的起点和终点
				int minX = FLT_MAX;
				int maxX = FLT_MIN;
				for (int i = 0; i < data->points.size(); i++)
				{
					if (data->points[i][0] < minX) { minX = data->points[i][0]; }
					if (data->points[i][0] > maxX) { maxX = data->points[i][0]; }
				}

				// 重新计算所有点的坐标, 并保存到容器中
				int wrapLength = 10; // 在最小和最大以外多画的点数
				int step = 1;  // 每step个像素之间画一条线
				data->LagrangeResults.clear();
				data->GaussResults.clear();
				data->LeastSquaresResults.clear();
				data->RidgeRegressionResults.clear();
				for (int x = minX - wrapLength; x < maxX + wrapLength; x += step)
				{
					data->LagrangeResults.push_back(ImVec2(x, Lagrange(data->points, x)));
					data->GaussResults.push_back(ImVec2(x, Gauss(data->points, x, data->GaussTheta)));
					data->LeastSquaresResults.push_back(ImVec2(x, LeastSquares(data->points, x, data->LeastSquaresM)));
					data->RidgeRegressionResults.push_back(ImVec2(x, RidgeRegression(data->points, x, data->RidgeRegressionM, data->RidgeRegressionLambda)));
				}
				needReCalculate = false;
			}

			if (data->points.size() >= 2)  // 少于两个点, 画线无意义
			{
				static float thickness = 2.0f;
				if (data->opt_lagrange) addLine(draw_list, origin, data->LagrangeResults, IM_COL32(64, 128, 255, 255), thickness);  // 画拉格朗日插值的线
				if (data->opt_gauss) addLine(draw_list, origin, data->GaussResults, IM_COL32(128, 255, 255, 255), thickness);  // 画高斯插值的线
				if (data->opt_least_squares) addLine(draw_list, origin, data->LeastSquaresResults, IM_COL32(255, 128, 128, 255), thickness);  // 画最小二乘拟合的线
				if (data->opt_ridge_regression) addLine(draw_list, origin, data->RidgeRegressionResults, IM_COL32(255, 64, 64, 255), thickness);  // 画最岭回归拟合的线
			}
		}
		ImGui::End();
	});
}