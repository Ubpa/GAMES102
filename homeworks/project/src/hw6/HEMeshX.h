#pragma once

#include <UHEMesh/HEMesh.h>

#include <UGM/UGM.h>

struct Vertex;
struct Edge;
struct Triangle;
struct HalfEdge;

using HEMeshXTraits = Ubpa::HEMeshTraits<Vertex, Edge, Triangle, HalfEdge>;

struct Vertex : Ubpa::TVertex<HEMeshXTraits> {
	Ubpa::pointf3 position{ 0.f };
};

struct Edge : Ubpa::TEdge<HEMeshXTraits> {

};

struct Triangle : Ubpa::TPolygon<HEMeshXTraits> {

};

struct HalfEdge : Ubpa::THalfEdge<HEMeshXTraits> {

};

using HEMeshX = Ubpa::HEMesh<HEMeshXTraits>;
