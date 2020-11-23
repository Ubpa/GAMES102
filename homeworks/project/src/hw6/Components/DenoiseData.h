#pragma once

#include <Utopia/Render/Mesh.h>
#include <Utopia/App/Editor/InspectorRegistry.h>
#include "../HEMeshX.h"

struct DenoiseData {
	// [[...]] is attribute list
	// user-defined attribute is useless
	// but Utopia use this information for Inspector

	// [[UInspector::range(std::pair{0.f, 10.f})]]
	[[UInspector::min_value(0.f)]]
	[[UInspector::tooltip("random scale")]]
	float randomScale = 1.f;

	std::shared_ptr<Ubpa::Utopia::Mesh> mesh;

	[[UInspector::hide]]
	std::shared_ptr<HEMeshX> heMesh{ std::make_shared<HEMeshX>() };

	[[UInspector::hide]]
	Ubpa::Utopia::Mesh copy;
};

#include "details/DenoiseData_AutoRefl.inl"
