#pragma once

#include <Utopia/Render/Mesh.h>
#include <Utopia/App/Editor/InspectorRegistry.h>
#include "../HEMeshX.h"

struct DenoiseData {
	[[UInspector::min_value(0.f)]]
	[[UInspector::tooltip("random scale")]]
	float randomScale = 1.f;

	std::shared_ptr<Ubpa::Utopia::Mesh> mesh;

	[[UInspector::hide]]
	std::shared_ptr<HEMeshX> heMesh{ Ubpa::USTL::make_shared_object<HEMeshX>() };

	[[UInspector::hide]]
	Ubpa::Utopia::Mesh copy;
};

#include "details/DenoiseData_AutoRefl.inl"
