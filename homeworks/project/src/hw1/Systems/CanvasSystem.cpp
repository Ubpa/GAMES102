#include "CanvasSystem.h"
#include "../Components/CanvasData.h"
#include <math.h>
#include <_deps/imgui/imgui.h>
#include <eigen3/Eigen/Eigen>
#include <atlstr.h>  // debug only

using namespace Ubpa;

float Lagrange(std::vector<Ubpa::pointf2>points, int x)
{
	float sum = 0;
	for (int k = 0; k < points.size(); k++)
	{
		float base = 1;
		for (int j = 0; j < points.size(); j++)
		{
			if (j == k) { continue; }
			base *= (x - points[j][0]) / (points[k][0] - points[j][0]);
		}
		sum += points[k][1] * base;
	}
	return sum;
}



void CanvasSystem::OnUpdate(Ubpa::UECS::Schedule& schedule) {
	schedule.RegisterCommand([](Ubpa::UECS::World* w) {
		auto data = w->entityMngr.GetSingleton<CanvasData>();
		if (!data)
			return;

		if (ImGui::Begin("Canvas")) {
			//ImGui::Checkbox("Enable grid", &data->opt_enable_grid);
			//ImGui::Checkbox("Enable context menu", &data->opt_enable_context_menu);
			ImGui::Checkbox("Lagrange", &data->opt_lagrange);

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

			static bool needReCalculate = true;
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
			if (needReCalculate)
			{
				// 找出要画的线条的起点和终点
				int minX = INT_MAX;
				int maxX = INT_MIN;
				for (int i = 0; i < data->points.size(); i++)
				{
					if (data->points[i][0] < minX) { minX = data->points[i][0]; }
					if (data->points[i][0] > maxX) { maxX = data->points[i][0]; }
				}

				// 重新计算所有点的坐标, 并保存到容器中
				int wrapLength = 10; // 在最小和最大以外多画的点数
				int step = 1;  // 每step个像素之间画一条线
				data->LagrangeResults.clear();
				for (int x = minX - wrapLength; x < maxX + wrapLength; x += step)
				{
					data->LagrangeResults.push_back(ImVec2(x, Lagrange(data->points, x)));
					//data->LagrangeResults.push_back(ImVec2(origin.x + x, origin.y + Lagrange(data->points, x)));
				}
				needReCalculate = false;
			}

			if (data->points.size() >= 2)  // 少于两个点, 插值无意义
			{
				// 画拉格朗日插值的线
				if (data->opt_lagrange)
				{
					/*draw_list->AddPolyline(data->LagrangeResults.data(), data->LagrangeResults.size(), IM_COL32(64, 128, 255, 255), false, 3.5f);*/
					for (int i = 0; i < data->LagrangeResults.size()-1; i++)
					{
						draw_list->AddLine(ImVec2(data->LagrangeResults[i][0] + origin.x, data->LagrangeResults[i][1] + origin.y), 
							ImVec2(data->LagrangeResults[i+1][0] + origin.x, data->LagrangeResults[i+1][1]), IM_COL32(64, 128, 255, 255), 3.5f);
					}
					
				}
			}
		}
		ImGui::End();
	});
}