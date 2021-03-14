#pragma once
#include "LiteMath.h"
#include "RayTracer.h"

class Lighting {
public:
	explicit Lighting(const float3 position, const float3 color) :
	position(position), color(color) {};
   virtual ~Lighting() = default;

	float3 position;
	float3 color;
};