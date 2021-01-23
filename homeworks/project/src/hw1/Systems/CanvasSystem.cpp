#include "CanvasSystem.h"

#include "../Components/CanvasData.h"

#include <_deps/imgui/imgui.h>

#include <fstream>

#include <Dense>

using namespace Ubpa;


void polygon_interpolation(std::vector<Ubpa::pointf2>& input_points, std::vector<float>& panel_x, std::vector<float>& output_y) {
	//Using power base
	if (input_points.size() < 2) return;
	Eigen::MatrixXf Van(input_points.size(), input_points.size());
	Eigen::VectorXf as(input_points.size());
	Eigen::VectorXf y(input_points.size());
	for (size_t i = 0; i < input_points.size(); ++i) {
		for (size_t j = 0; j < input_points.size(); ++j) {
			Van(i, j) = std::pow(input_points[i][0], j);
		}
		y(i) = input_points[i][1];
	}
	as = Van.householderQr().solve(y);

	Eigen::VectorXf xx(input_points.size());
	for (auto x : panel_x) {
		for (size_t i = 0; i < xx.size(); ++i) {
			xx(i) = std::pow(x, i);
		}
		output_y.push_back(xx.dot(as));
	}
}
void Lagrange_interpolation(std::vector<Ubpa::pointf2>& input_points, std::vector<float>& panel_x, std::vector<float>& output_y) {
	//Using Lagrange
	if (input_points.size() < 2) return;
	for (auto x : panel_x) {
		float y = 0.0f;
		for (size_t i = 0; i < input_points.size(); ++i) {
			float L = input_points[i][1];
			for (size_t j = 0; j < input_points.size(); ++j) {
				if (i == j) continue;
				L *= (x - input_points[j][0]);
				L /= (input_points[i][0] - input_points[j][0]);
			}
			y += L;
		}
		output_y.push_back(y);
	}

}

void Guass_interpolation(std::vector<Ubpa::pointf2>& input_points, std::vector<float>& panel_x, std::vector<float>& output_y, float sigma, float b0) {
	//Using Guass
	if (input_points.size() < 2) return;
	Eigen::MatrixXf G(input_points.size(), input_points.size());
	Eigen::VectorXf bs(input_points.size());
	Eigen::VectorXf y(input_points.size());
	for (size_t i = 0; i < input_points.size(); ++i) {
		for (size_t j = 0; j < input_points.size(); ++j) {
			G(i, j) = std::exp(-0.5 * std::powf((input_points[i][0] - input_points[j][0]), 2) / sigma);
		}
		y(i) = input_points[i][1] - b0;
	}
	bs = G.householderQr().solve(y);

	for (auto x : panel_x) {
		float y = b0;
		for (size_t i = 0; i < input_points.size(); ++i) {
			y += bs(i) * std::exp(-0.5 * std::powf((x - input_points[i][0]), 2) / sigma);
		}
		output_y.push_back(y);
	}
}

void LSM_approximation(std::vector<Ubpa::pointf2>& input_points, std::vector<float>& panel_x, std::vector<float>& output_y, int m,float b0,float sigma) {
	//Using LSM
	if (input_points.size() < 3) return;
	if (m + 1 > input_points.size()) m = input_points.size() - 1;
	Eigen::MatrixXf A(input_points.size(), m + 1);
	Eigen::VectorXf as(m + 1);
	Eigen::VectorXf y(input_points.size());
	for (int i = 0; i < input_points.size(); ++i) {
		for (int j = 0; j < m + 1; ++j) {
			A(i, j) = std::exp(-0.5 * std::powf((input_points[i][0] - input_points[j][0]), 2) / sigma);
		}
		y(i) = input_points[i][1]-b0;
	}
	as = (A.transpose() * A).inverse() * A.transpose() * y;
	
	for (auto x : panel_x) {
		float y = b0;
		for (size_t i = 0; i < m+1; ++i) {
			y += as(i) * std::exp(-0.5 * std::powf((x - input_points[i][0]), 2) / sigma);
		}
		output_y.push_back(y);
	}
	
}

void RR_approximation(std::vector<Ubpa::pointf2>& input_points, std::vector<float>& panel_x, std::vector<float>& output_y, float lambda, int m, float b0, float sigma) {
	//Using LSM
	if (input_points.size() < 3) return;
	if (m + 1 > input_points.size()) m = input_points.size() - 1;
	Eigen::MatrixXf A(input_points.size(), m + 1);
	Eigen::VectorXf as(m + 1);
	Eigen::VectorXf y(input_points.size());
	for (int i = 0; i < input_points.size(); ++i) {
		for (int j = 0; j < m + 1; ++j) {
			A(i, j) = std::exp(-0.5 * std::powf((input_points[i][0] - input_points[j][0]), 2) / sigma);
		}
		y(i) = input_points[i][1]-b0;
	}
	as = (A.transpose() * A + lambda * Eigen::MatrixXf::Identity(m + 1, m + 1)).inverse() * A.transpose() * y;

	for (auto x : panel_x) {
		float y = b0;
		for (size_t i = 0; i < m + 1; ++i) {
			y += as(i) * std::exp(-0.5 * std::powf((x - input_points[i][0]), 2) / sigma);
		}
		output_y.push_back(y);
	}
}

void CanvasSystem::OnUpdate(Ubpa::UECS::Schedule& schedule) {
	schedule.RegisterCommand([](Ubpa::UECS::World* w) {
		auto data = w->entityMngr.GetSingleton<CanvasData>();
		if (!data)
			return;

		if (ImGui::Begin("Canvas")) {
			ImGui::Checkbox("Enable grid", &data->opt_enable_grid);
			ImGui::Checkbox("Enable context menu", &data->opt_enable_context_menu);
			ImGui::Text("Mouse Left: drag to add lines,\nMouse Right: drag to scroll, click for context menu.");
			ImGui::Checkbox("Lagrange_interpolation", &data->opt_proc[0]);
			/*		ImGui::SameLine(120);*/
			ImGui::Checkbox("Guass_interpolation", &data->opt_proc[1]);
			ImGui::Checkbox("RR_approximation", &data->opt_proc[2]); //Ridge Regression
			ImGui::Checkbox("LSM_approximation", &data->opt_proc[3]); //Least Squares Method
			ImGui::SliderFloat("Lamda", &data->lambda, -1000.0f, 1000.0f);
			ImGui::SliderFloat("sigma", &data->sigma, 0.0f, 1000.0f);
			ImGui::SliderFloat("b0", &data->b0, 0.0f, 500.0f);
			ImGui::SliderInt("Highest Power", &data->m, 1, 100);
			// Typically you would use a BeginChild()/EndChild() pair to benefit from a clipping
			// region + own scrolling. Here we demonstrate that this can be replaced by simple
			// offsetting + custom drawing + PushClipRect/PopClipRect() calls. To use a child window
			// instead we could use, e.g: ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,
			// 0)); // Disable padding ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(50, 50, 50,
			// 255)); // Set a background color ImGui::BeginChild("canvas", ImVec2(0.0f, 0.0f),
			// true, ImGuiWindowFlags_NoMove); ImGui::PopStyleColor(); ImGui::PopStyleVar(); [...] ImGui::EndChild();

			// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2)
			// allows us to use IsItemHovered()/IsItemActive()
			ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
			ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
			if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
			if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
			ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

			// Draw border and background color
			ImGuiIO& io = ImGui::GetIO();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
			draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

			// This will catch our interactions
			ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			const bool is_hovered = ImGui::IsItemHovered(); // Hovered
			const bool is_active = ImGui::IsItemActive();   // Held
			const ImVec2 origin(canvas_p0.x + data->scrolling[0], canvas_p0.y + data->scrolling[1]); // Lock scrolled origin
			const pointf2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

			//// Add first and second point
			//if (is_hovered && !data->generate_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			//{
			//	data->points.push_back(mouse_pos_in_canvas);
			//	data->points.push_back(mouse_pos_in_canvas);
			//	data->generate_line = true;
			//}
			//if (data->generate_line)
			//{
			//	data->points.back() = mouse_pos_in_canvas;
			//	if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
			//		data->generate_line = false;
			//}


			//Once clicked, generate related line.
			if (is_active && !data->generate_line && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				data->points.push_back(mouse_pos_in_canvas);
				data->generate_line = true;
			}
			if ((data->opt_proc[3] || data->opt_proc[2] )&&data->cur_m != data->m) {
				data->cur_m = data->m;
				data->generate_line = true;
			}
			if ((data->opt_proc[3] || data->opt_proc[2] || data->opt_proc[1]) && data->cur_b0 != data->b0) {
				data->cur_b0 = data->b0;
				data->generate_line = true;
			}
			if ((data->opt_proc[3]) && data->cur_lambda != data->lambda) {
				data->cur_lambda = data->lambda;
				data->generate_line = true;
			}
			if ((data->opt_proc[3] || data->opt_proc[2]|| data->opt_proc[1]) && data->cur_sigma != data->sigma) {
				data->cur_sigma = data->cur_sigma;
				data->generate_line = true;
			}
			// Pan (we use a zero mouse threshold when there's no context menu) You may decide to
			// make that threshold dynamic based on whether the mouse is hovering something etc.

			const float mouse_threshold_for_pan = data->opt_enable_context_menu ? -1.0f : 0.0f;
			if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
			{
				data->scrolling[0] += io.MouseDelta.x;
				data->scrolling[1] += io.MouseDelta.y;
			}

			// Context menu (under default mouse threshold)
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
			if (data->opt_enable_context_menu && ImGui::IsMouseReleased(ImGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
				ImGui::OpenPopupContextItem("context");
			if (ImGui::BeginPopup("context"))
			{
				if (data->generate_line)
					data->points.resize(data->points.size() - 2);
				data->generate_line = false;
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
					draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
				for (float y = fmodf(data->scrolling[1], GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
					draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
			}
			for (size_t i = 0; i < data->points.size(); ++i) {
				draw_list->AddCircleFilled(ImVec2(origin.x + data->points[i][0], origin.y + data->points[i][1]), 5.0f, IM_COL32(255, 0, 0, 255));
			}

			constexpr float step = 1.0f;

			if (data->generate_line) {
				data->panel_x.clear();

				for (float px = canvas_p0.x - origin.x; px < canvas_p1.x - origin.x; px += step) {
					data->panel_x.push_back(px);
				}
				if (data->opt_proc[0]) {
					data->panel_y[0].clear();
					Lagrange_interpolation(data->points, data->panel_x, data->panel_y[0]);

				}
				if (data->opt_proc[1]) {
					data->panel_y[1].clear();
					Guass_interpolation(data->points, data->panel_x, data->panel_y[1], data->sigma, data->b0);
				}
				if (data->opt_proc[2]) {
					data->panel_y[2].clear();
					LSM_approximation(data->points, data->panel_x, data->panel_y[2], data->m,data->b0,data->sigma);
				}
				if (data->opt_proc[3]) {
					data->panel_y[3].clear();
					RR_approximation(data->points, data->panel_x, data->panel_y[3], data->lambda, data->m, data->b0, data->sigma);
				}
				data->generate_line = false;

			}

			if (data->opt_proc[0] && data->points.size() > 1) {
				for (size_t i = 0; i < data->panel_x.size() - 1; ++i) {
					draw_list->AddLine(ImVec2(origin.x + data->panel_x[i], origin.y + data->panel_y[0][i]), ImVec2(origin.x + data->panel_x[i + 1], origin.y + data->panel_y[0][i + 1]), IM_COL32(255, 255, 0, 255));
				}

			}
			if (data->opt_proc[1] && data->points.size() > 1) {
				for (size_t i = 0; i < data->panel_x.size() - 1; ++i) {
					draw_list->AddLine(ImVec2(origin.x + data->panel_x[i], origin.y + data->panel_y[1][i]), ImVec2(origin.x + data->panel_x[i + 1], origin.y + data->panel_y[1][i + 1]), IM_COL32(0, 255, 0, 255));
				}

			}
			if (data->opt_proc[2] && data->points.size() > 2) {
				for (size_t i = 0; i < data->panel_x.size() - 1; ++i) {
					draw_list->AddLine(ImVec2(origin.x + data->panel_x[i], origin.y + data->panel_y[2][i]), ImVec2(origin.x + data->panel_x[i + 1], origin.y + data->panel_y[2][i + 1]), IM_COL32(0, 0, 255, 255));
				}

			}
			if (data->opt_proc[3] && data->points.size() > 2) {
				for (size_t i = 0; i < data->panel_x.size() - 1; ++i) {
					draw_list->AddLine(ImVec2(origin.x + data->panel_x[i], origin.y + data->panel_y[3][i]), ImVec2(origin.x + data->panel_x[i + 1], origin.y + data->panel_y[3][i + 1]), IM_COL32(0, 255, 255, 255));
				}

			}
			/*for (int n = 0; n < data->points.size(); n += 2)
				draw_list->AddLine(ImVec2(origin.x + data->points[n][0], origin.y + data->points[n][1]), ImVec2(origin.x + data->points[n + 1][0], origin.y + data->points[n + 1][1]), IM_COL32(255, 255, 0, 255), 2.0f);*/
			draw_list->PopClipRect();
		}

		ImGui::End();
		});
}
