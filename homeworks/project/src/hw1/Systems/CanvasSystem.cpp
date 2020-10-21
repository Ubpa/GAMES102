#include "CanvasSystem.h"
#include "../Components/CanvasData.h"
#include <math.h>
#include <_deps/imgui/imgui.h>
#include <eigen3/Eigen/Eigen>
#include <atlstr.h>  // debug only

using namespace Ubpa;

void CanvasSystem::OnUpdate(Ubpa::UECS::Schedule& schedule) {
	schedule.RegisterCommand([](Ubpa::UECS::World* w) {
		auto data = w->entityMngr.GetSingleton<CanvasData>();
		if (!data)
			return;

		if (ImGui::Begin("Canvas")) {
			//ImGui::Checkbox("Enable grid", &data->opt_enable_grid);
			//ImGui::Checkbox("Enable context menu", &data->opt_enable_context_menu);
			ImGui::Checkbox("Draw Power Function", &data->func_power);

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
			const ImVec2 origin(canvas_p0.x + data->scrolling[0], canvas_p0.y + data->scrolling[1]); // 
			const pointf2 mouse_pos_in_canvas(io.MousePos.x, io.MousePos.y);  // 运行窗口中Game中的坐标, 不是Canvas的坐标
			const pointf2 point_pos(io.MousePos.x - origin.x, io.MousePos.y - origin.y);  // 点相对Canvas的坐标


			static int points_size = -1;  // 通过点的数量来判断是否需要重新绘制线条(重新计算参数)
			// 添加点
			if (is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				data->points.push_back(point_pos);
			}

			// Draw points 画点
			for (int n = 0; n < data->points.size(); n++)
				draw_list->AddCircle(data->points[n] + origin, 2.0f, IM_COL32(0, 255, 255, 200), 0, 3);

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
				if (ImGui::MenuItem("Remove one", NULL, false, data->points.size() > 0)) { data->points.resize(data->points.size() - 1); }
				if (ImGui::MenuItem("Remove all", NULL, false, data->points.size() > 0)) { data->points.clear(); }
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


			// 多项式函数: 幂函数的线性组合
			if (data->points.size() >= 2)  // 少于两个点, 插值无意义
			{
				if (data->func_power)
				{
					if (points_size != data->points.size())
					{
						points_size = data->points.size();
						// need to recaculate the paramater 
						draw_func_power(draw_list, data->points, origin, true);
					}
					else
					{
						draw_func_power(draw_list, data->points, origin, false);
					}
				}
			}
		}
		ImGui::End();
		});
}

/*
draw_list: 用来画线
v: 用来取点的信息
origin: 用来确定画布的位置
needRecalculate: 是否需要重新计算参数a
*/
void CanvasSystem::draw_func_power(ImDrawList* draw_list, std::vector<Ubpa::pointf2> v, ImVec2 origin, bool needRecalculate)
{

	int size = v.size();
	Eigen::MatrixXf X(size, size);
	Eigen::VectorXf y(size);
	Eigen::VectorXf a(size);  // 参数 X dot a = y
	float thickness = 1.5f;
	ImU32 color = IM_COL32(255, 0, 0, 200);

	int min_x = v[0][0];  // 获得添加的点中最小的x值, 坐标都是整数, 所以用int
	int max_x = v[0][0];  // 获得添加的点中最大的x值
	for (int i = 0; i < size; i++)
	{
		if (v[i][0] < min_x) min_x = v[i][0];
		if (v[i][0] > max_x) max_x = v[i][0];
		for (int j = 0; j < size; j++)
		{
			X(i, j) = pow(v[i][0], j);  // x_i^j
		}
	}

	for (int i = 0; i < size; i++)
	{
		y[i] = v[i][1];
	}

	a = X.inverse() * y;

	///////////////////// 画线条 ////////////////////////////
	//int density = 200;  // 插值的点数
	//int interval = 1 > (max_x - min_x) / density ? 1 : (max_x - min_x) / density;  // 插值点之间的间隔, 最小值是1
	int interval = 1;
	static int y_tmp = -1;
	for (int x1 = min_x-20; x1 < max_x+20; x1 += interval)  // 前后多画一些, 防止线接近垂直于x轴时没过点
	{
		int x2 = x1 + interval;
		int y1;  // 由于是坐标点都是整型, 所以会有精度损失, 画出来的线条会锯齿
		int y2;

		// 计算y1
		if(y_tmp == -1)  // 如果是画曲线的第一个小片段(曲线是由很多小线段拼起来的), y1要重新算
		{
			Eigen::VectorXf x_v1(size);
			for (int i = 0; i < size; i++)
				x_v1(i) = pow(x1, i);
			y1 = a.dot(x_v1);
		}
		else  // y1可以直接用之前的
			y1 = y_tmp;  // 可以直接用之前计算好的

		// 计算y2
		Eigen::VectorXf x_v2(size);
		for (int i = 0; i < size; i++)
			x_v2(i) = pow(x2, i);
		y2 = a.dot(x_v2);

		y_tmp = y2; // 保存计算好的y2值, 这是下一个线段的起点, 用来画下一个线段

		draw_list->AddLine(ImVec2(x1 + origin.x, y1 + origin.y),
						   ImVec2(x2 + origin.x, y2 + origin.y), color, thickness);
	}
	y_tmp = -1;  // 要复位一下, 下一波更新的时候线条的起点才正常
}
