#pragma once

#include <UECS/World.h>

#include <_deps/imgui/imgui.h>

struct CanvasSystem {
	static void OnUpdate(Ubpa::UECS::Schedule& schedule);
	static void draw_func_power(ImDrawList* draw_list, std::vector<Ubpa::pointf2> v, ImVec2 origin);
};