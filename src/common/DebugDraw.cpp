#include "DebugDraw.h"

DebugDraw::DebugDraw(vec4_t color, int duration, qboolean depthTest)
{
	InitInterface();
	if (color) {
		memcpy(this->color, color, sizeof(this->color));
	} else {
		color[0] = color[1] = color[2] = color[3] = 1;
	}
	this->duration = duration;
	this->depthTest = depthTest;
}

#ifndef BUILD_GAME
void DebugDraw::InitInterface()
{
	ddi = DebugDrawRenderInterface::NullInterface();
}
#endif
void DebugDraw::Tri(vec3_t verts[3]) {
	Tri(verts[0], verts[1], verts[2]);
}

void DebugDraw::Tri(vec3_t v0, vec3_t v1, vec3_t v2)
{
	ddraw_tri t;
	memcpy(t.verts[0], v0, sizeof(t.verts[0]));
	memcpy(t.verts[1], v1, sizeof(t.verts[0]));
	memcpy(t.verts[2], v2, sizeof(t.verts[0]));
	memcpy(t.color, color, sizeof(t.color));
	t.expiration = duration;
	t.depthTest = depthTest;
	ddi.addTri(t);
}

void DebugDraw::Quad(vec3_t verts[4])
{
	Quad(verts[0], verts[1], verts[2], verts[3]);
}

void DebugDraw::Quad(vec3_t v0, vec3_t v1, vec3_t v2, vec3_t v3)
{
	Tri(v0, v1, v2);
	Tri(v2, v3, v0);
}

void DebugDraw::BoxFill(vec3_t corner, vec3_t sides[3])
{
	vec3_t v[8];
	VectorCopy(corner, v[0]);
	for (int s = 0; s < 3; s++) {
		for (int i = 0; i < (1 << s); i++) {
			VectorAdd(v[i], sides[s], v[i | (1 << s)]);
		}
	}
	for (int ssame = 0; ssame < 3; ssame++) {
		int sdif1 = (ssame + 1) % 3, sdif2 = (ssame + 2) % 3;
		for (int u = 0; u < 2; u++) {
			Quad(v[u << ssame], v[(u << ssame) | (1 << sdif1)], v[(u << ssame) | (1 << sdif1) | (1 << sdif2)], v[(u << ssame) | (1 << sdif2)]);
		}
	}
}

void DebugDraw::BoxOutline(vec3_t corner, vec3_t sides[3])
{
	vec3_t v[8];
	VectorCopy(corner, v[0]);
	for (int s = 0; s < 3; s++) {
		for (int i = 0; i < (1 << s); i++) {
			VectorAdd(v[i], sides[s], v[i | (1 << s)]);
		}
	}
	for (int dif = 0; dif < 3; dif++) {
		int same1 = (dif + 1) % 3, same2 = (dif + 2) % 3;
		for (int t = 0; t < 2; t++) {
			for (int u = 0; u < 2; u++) {
				Line(v[(t << same1) | (u << same2)], v[(t << same1) | (u << same2) | (1 << dif)]);
			}
		}
	}
}

void DebugDraw::BoxOutline(vec3_t corner, vec3_t lengths)
{
	vec3_t sides[3] = {{lengths[0], 0.f, 0.f}, {0.f, lengths[1], 0.f}, {0.f, 0.f, lengths[2]}};
	BoxOutline(corner, sides);
}

void DebugDraw::BoxFill(vec3_t corner, vec3_t lengths)
{
	vec3_t sides[3] = {{lengths[0], 0.f, 0.f}, {0.f, lengths[1], 0.f}, {0.f, 0.f, lengths[2]}};
	BoxFill(corner, sides);
}

void DebugDraw::bucky(vec3_t center, float r)
{
	vec3_t v[60];
	const float phi = 1.61803399;
	const vec_t coord[3][3] = {{0, 1, 3 * phi}, {2, 1 + 2 * phi, phi}, {1, 2 + phi, 2 * phi}};
	int n = 0;
	for (int i = 0; i < 3; i++) {
		for (int perm = 0; perm < 3; perm++) {
			for (int pm = 0; pm < 8; pm += (i?1:2)) {
				for (int d = 0; d < 3; d++) {
					v[n][(d + perm) % 3] = (pm & (1 << d)) ? coord[i][d] : -coord[i][d];
				}
				n++;
			}
		}
	}
	float scale = r / (9 * phi + 10);
	for (int i = 0; i < 60; i++) {
		VectorMA(center, scale, v[i], v[i]);
	}

	float edgelen = 2.01 * scale;
	n = 0;
	for (int i = 0; i < 60; i++) {
		for (int j = i+1; j < 60; j++) {
			if (VectorDistance(v[i], v[j]) < edgelen) {
				Line(v[i], v[j]);
				n++;
			}
		}
	}
	Com_Printf("%d", n);
}

void DebugDraw::Line(vec3_t v0, vec3_t v1)
{
	vec3_t normdif, sides[3], orig;
	VectorSubtract(v1, v0, sides[0]);
	VectorNormalize2(sides[0], normdif);
	PerpendicularVector(sides[1], normdif);
	VectorScale(sides[1], lineWidth, sides[1]);
	CrossProduct(normdif, sides[1], sides[2]);
	VectorMA(v0, -0.5f, sides[1], orig);
	VectorMA(orig, -0.5f, sides[2], orig);
	BoxFill(orig, sides);
}