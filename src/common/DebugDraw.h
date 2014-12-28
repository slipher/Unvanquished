#ifndef COMMON_DEBUGDRAW_H
#define COMMON_DEBUGDRAW_H

#include "DebugDrawRenderInterface.h"

struct DebugDraw {
	vec4_t color;
	int duration;
	qboolean depthTest;
	float lineWidth;

	DebugDraw(vec4_t color = NULL, int duration = 0, qboolean depthTest = true);

	void Tri(vec3_t verts[3]);
	void Tri(vec3_t v0, vec3_t v1, vec3_t v2);
	void Quad(vec3_t verts[4]);
	void Quad(vec3_t v0, vec3_t v1, vec3_t v2, vec3_t v3);
	void Line(vec3_t v0, vec3_t v1);
	void Line(vec3_t verts[2]);
	void BoxFill(vec3_t corner, vec3_t sides[3]);
	void BoxFill(vec3_t corner, vec3_t lengths);
	void BoxOutline(vec3_t corner, vec3_t sides[3]);
	void BoxOutline(vec3_t corner, vec3_t lengths);   //Axis-aligned box
	

	void bucky(vec3_t center, float r);

private:
	DebugDrawRenderInterface ddi;
	void InitInterface();
};

#endif
