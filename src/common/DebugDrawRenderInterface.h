#ifndef COMMON_DEBUGDRAWRENDERINTERFACE_H
#define COMMON_DEBUGDRAWRENDERINTERFACE_H

#include "../engine/qcommon/q_shared.h"

struct ddraw_tri {
	vec3_t verts[3];
	vec4_t color;
	qboolean depthTest;
	int expiration;
};

typedef enum ddraw_action {
	DDA_CLEAR_ALL
};

#define DDRAW_TRI_BATCHSIZE (1 << 10)

struct DebugDrawRenderInterface {
	void (*addTri)(ddraw_tri tri);
	void (*miscAction)(ddraw_action act);

	static DebugDrawRenderInterface NullInterface()
	{
		return {[](ddraw_tri){}, [](ddraw_action){}};
	}
};


/*#ifdef ANY_BUILD_CLIENT
extern DebugDrawRenderInterface ddri;
#elif defined(BUILD_SERVER)
DebugDrawRenderInterface ddri = DebugDrawRenderInterface::NullInterface();
#endif*/

/*template <size_t n> {
	typedef ptl<n> std::array<vec3_t, n>;
};*/


#endif