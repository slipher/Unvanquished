/*
===========================================================================

Unvanquished BSD Source Code
Copyright (c) 2013-2016, Unvanquished Developers
All rights reserved.

This file is part of the Unvanquished BSD Source Code (Unvanquished Source Code).

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Unvanquished developers nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL DAEMON DEVELOPERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

===========================================================================
*/

#include "sg_local.h"
#include "sg_cm_world.h"
#include "botlib/bot_api.h"
#include "engine/server/sg_msgdef.h"
#include "shared/VMMain.h"
#include "shared/CommonProxies.h"
#include "shared/server/sg_api.h"

#include <glm/vec3.hpp>

// Symbols required by the shared VMMain code

int VM::VM_API_VERSION = GAME_API_VERSION;

void VM::VMInit() {
	// Allocate entities and clients shared memory region
	shmRegion = IPC::SharedMemory::Create(sizeof(gentity_t) * MAX_GENTITIES + sizeof(gclient_t) * MAX_CLIENTS);
	char* shmBase = reinterpret_cast<char*>(shmRegion.GetBase());
	g_entities = reinterpret_cast<gentity_t*>(shmBase);
	g_clients = reinterpret_cast<gclient_t*>(shmBase + sizeof(gentity_t) * MAX_GENTITIES);

	// Load the map collision data
	std::string mapName = Cvar::GetValue("mapname");
	CM_LoadMap(mapName);
	G_CM_ClearWorld();
}

void VM::VMHandleSyscall(uint32_t id, Util::Reader reader) {

	int major = id >> 16;
	int minor = id & 0xffff;
	if (major == VM::QVM) {
		switch (minor) {
		case GAME_STATIC_INIT:
			IPC::HandleMsg<GameStaticInitMsg>(VM::rootChannel, std::move(reader), [] (int milliseconds) {
				VM::InitializeProxies(milliseconds);
				FS::Initialize();
				VM::VMInit();
			});
			break;

		case GAME_INIT:
			IPC::HandleMsg<GameInitMsg>(VM::rootChannel, std::move(reader), [](int levelTime, int randomSeed, bool cheats, bool inClient) {
				g_cheats = cheats;
				G_InitGame(levelTime, randomSeed, inClient);
			});
			break;

		case GAME_SHUTDOWN:
			IPC::HandleMsg<GameShutdownMsg>(VM::rootChannel, std::move(reader), [](bool restart) {
				G_ShutdownGame(restart);
			});
			break;

		case GAME_CLIENT_CONNECT:
			IPC::HandleMsg<GameClientConnectMsg>(VM::rootChannel, std::move(reader), [](int clientNum, bool firstTime, int isBot, bool& denied, std::string& reason) {
				const char* deniedStr = isBot ? ClientBotConnect(clientNum, firstTime, TEAM_NONE) : ClientConnect(clientNum, firstTime);
				denied = deniedStr != nullptr;
				if (denied)
					reason = deniedStr;
			});
			break;

		case GAME_CLIENT_THINK:
			IPC::HandleMsg<GameClientThinkMsg>(VM::rootChannel, std::move(reader), [](int clientNum) {
				ClientThink(clientNum);
			});
			break;

		case GAME_CLIENT_USERINFO_CHANGED:
			IPC::HandleMsg<GameClientUserinfoChangedMsg>(VM::rootChannel, std::move(reader), [](int clientNum) {
				ClientUserinfoChanged(clientNum, false);
			});
			break;

		case GAME_CLIENT_DISCONNECT:
			IPC::HandleMsg<GameClientDisconnectMsg>(VM::rootChannel, std::move(reader), [](int clientNum) {
				ClientDisconnect(clientNum);
			});
			break;

		case GAME_CLIENT_BEGIN:
			IPC::HandleMsg<GameClientBeginMsg>(VM::rootChannel, std::move(reader), [](int clientNum) {
				ClientBegin(clientNum);
			});
			break;

		case GAME_CLIENT_COMMAND:
			IPC::HandleMsg<GameClientCommandMsg>(VM::rootChannel, std::move(reader), [](int clientNum, std::string command) {
				Cmd::PushArgs(command);
				ClientCommand(clientNum);
				Cmd::PopArgs();
			});
			break;

		case GAME_RUN_FRAME:
			IPC::HandleMsg<GameRunFrameMsg>(VM::rootChannel, std::move(reader), [](int levelTime) {
				G_RunFrame(levelTime);
			});
			break;

		case GAME_SNAPSHOT_CALLBACK:
			Sys::Drop("GAME_SNAPSHOT_CALLBACK not implemented");
			break;

		case BOTAI_START_FRAME:
			Sys::Drop("BOTAI_START_FRAME not implemented");
			break;

		default:
			Sys::Drop("VMMain(): unknown game command %i", minor);
		}
	} else if (major < VM::LAST_COMMON_SYSCALL) {
		VM::HandleCommonSyscall(major, minor, std::move(reader), VM::rootChannel);
	} else {
		Sys::Drop("unhandled VM major syscall number %i", major);
	}
}

void trap_LinkEntity(gentity_t *ent)
{
	G_CM_LinkEntity(ent);
}

void trap_UnlinkEntity(gentity_t *ent)
{
	G_CM_UnlinkEntity(ent);
}

int trap_EntitiesInBox(const vec3_t mins, const vec3_t maxs, int *list, int maxcount)
{
	return G_CM_AreaEntities(mins, maxs, list, maxcount);
}

bool trap_EntityContact(const vec3_t mins, const vec3_t maxs, const gentity_t *ent)
{
	return G_CM_EntityContact( mins, maxs, ent, traceType_t::TT_AABB );
}

// Traces against the world and all entities. See the comments on struct trace_t (q_shared.h) for
// a general explanation of trace semantics. The difference with CM_* trace functions is that those
// only trace against the world or a single BSP model, whereas trap_Trace checks everything in one
// call.
// Parameters:
//     start / end: beginning and end points of the trace
//     mins / maxs: bounding box extents (may be nullptr for a point trace)
//     contentmask: collide with brushes/entities having any of these content flags
//     skipmask: ignore brushes/entities having any of these content flags
//     passEntityNum: if not ENTITYNUM_NONE, ignore the entity of this number and any entities owned by it
//
// `results` is populated according to the trace_t documentation, except there is one additional
// field, entityNum. entityNum can be either one of two things:
//   * an entity hit by the trace
//   * an entity other than ENTITYNUM_WORLD that caused the startsolid flag to be set
// If a trace both starts overlapping a non-world entity, and hits a different entity,
// then entityNum may correspond to either one at random (higher entity numbers would overwrite
// lower ones). TODO: fix wonky entityNum behavior by making it always correspond to the hit entity
// (thus ignoring collisions that set startsolid but not allsolid). Some callers must be fixed that
// rely on this behavior; create a new API if necessary.
void trap_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs,
                 const vec3_t end, int passEntityNum, int contentmask, int skipmask )
{
	G_CM_Trace(results, start, mins, maxs, end, passEntityNum, contentmask, skipmask, traceType_t::TT_AABB);
	
	int passOwner = g_entities[passEntityNum].r.ownerNum;
	if (passOwner != 0 && passOwner != ENTITYNUM_NONE)
		Log::Warn("passentity %s has has an owner %s", etos(g_entities + passEntityNum), etos(g_entities + passOwner));

	bool shouldHaveFreeSpaceAtEndpoint = results->fraction == 1.0f || ( !results->startsolid && results->fraction != 0.0f );
	if ( shouldHaveFreeSpaceAtEndpoint && mins && maxs && mins[0] < -2 && mins[1] < -2 && mins[2] < -2 &&
			maxs[0] > 2 && maxs[1] > 2 && maxs[2] > 2)
	{
		vec3_t mins1{ mins[0] + 1, mins[1] + 1, mins[2] + 1 };
		vec3_t maxs1{ maxs[0] - 1, maxs[1] - 1, maxs[2] - 1 };
		trace_t wat; // test if a free point was reached at the end of the trace
		G_CM_Trace(&wat, results->endpos, mins1, maxs1, results->endpos, passEntityNum, contentmask, skipmask, traceType_t::TT_AABB);
		if ( wat.fraction != 1.0f )
		{
			Log::Warn("endpoint is not clear of collisions as expected. start=%s end=%s mins=%s maxs=%s plane-normal=(%f %f %f)",
				vtos(start), vtos(end), vtos(mins), vtos(maxs), results->plane.normal[0], results->plane.normal[1], results->plane.normal[2]);
		}
	}

	// for detecting likely misuses of the trace API where the behavior of ignoring beginning overlaps is not expected)
	if (results->startsolid && !VectorCompare(start, end) && !(contentmask & 0x800 && contentmask != MASK_ALL))
	{
		if ( mins && maxs )
		{
			vec3_t mins2{ mins[0] + .2, mins[1] + .2, mins[2] + .2 };
			vec3_t maxs2{ maxs[0] - .2, maxs[1] - .2, maxs[2] - .2 };
			trace_t redo;
			G_CM_Trace(&redo, start, mins2, maxs2, end, passEntityNum, contentmask, skipmask, traceType_t::TT_AABB);
			if (!redo.startsolid) goto invariants;
		}

		int ents[MAX_GENTITIES];
		vec3_t m, M;
		vec3_t z{};
		VectorAdd(start, mins?mins:z, m);
		VectorAdd(start, maxs?maxs:z, M);
		
		std::string stuff;
		for (int i = trap_EntitiesInBox(m, M, ents, MAX_GENTITIES); i--; )
		{
			if (!g_entities[ents[i]].r.contents) continue;
			if (passEntityNum != ENTITYNUM_NONE)
			{
				if (ents[i] == passEntityNum) continue;
				if (g_entities[ents[i]].r.ownerNum == passEntityNum) continue;
				if (g_entities[passEntityNum].r.ownerNum == ents[i]) continue;
				if (g_entities[ents[i]].r.ownerNum != ENTITYNUM_NONE && g_entities[ents[i]].r.ownerNum == g_entities[passEntityNum].r.ownerNum) continue;
			}
			stuff += " ";
			stuff += etos(g_entities + ents[i]);
		}
		Log::Warn("startsolid overlapping:%s\nstart=%s end=%s mins=%s maxs=%s", stuff,
			vtos(start), vtos(end), vtos(mins), vtos(maxs));
	}

	// test invariants
	invariants:
	ASSERT_GE(results->fraction, 0.0f);
	ASSERT_LE(results->fraction, 1.0f);
	if (results->fraction == 1.0f)
	{
		ASSERT_EQ(results->contents, 0);
		ASSERT_EQ(results->entityNum, ENTITYNUM_NONE);

		// these are also not valid with allsolid but could have been overwritten
		ASSERT_EQ(results->plane.dist, 0);
		ASSERT_EQ(results->surfaceFlags, 0);
	}
	else
	{
		ASSERT(results->contents & contentmask);
		ASSERT_NQ(results->entityNum, ENTITYNUM_NONE);
		ASSERT_NQ(results->entityNum, passEntityNum);
	}
	ASSERT_EQ(0, results->contents & skipmask);
	if (results->allsolid)
	{
		ASSERT(results->startsolid);
		// if allsolid is true, the thing the trace box started out overlapping is
		// *not* ignored, unlike when only allsolid is true.
		ASSERT_EQ(results->fraction, 0.0f);
	}
	if (!results->allsolid && results->fraction < 1.0f)
	{
		ASSERT_EQ(roundf(1000 * VectorLength(results->plane.normal)), 1000);
	}

	ASSERT_LT(Distance(results->endpos, &((1.0f - results->fraction) * VEC2GLM(start) + results->fraction * VEC2GLM(end) )[0]), 0.01f);

	ASSERT_EQ(CM_CheckTraceConsistency(start, end, contentmask, skipmask, *results), "");

	//check plane. but ignore for non-world entities where it is not corrected for origin
	if (!results->allsolid && results->fraction != 1.0f && results->entityNum >= ENTITYNUM_WORLD)
	{
		float maxCornerDist = mins ? glm::length(glm::max(-VEC2GLM(mins), VEC2GLM(maxs))) : 0;
		float endposPlaneDist = glm::abs(DotProduct(results->endpos, results->plane.normal) - results->plane.dist);
		if (endposPlaneDist > maxCornerDist + 0.2f)
			Log::Warn("plane seems wrong. start=%s end=%s mins=%s maxs=%s",
				vtos(start), vtos(end), vtos(mins), vtos(maxs));
	}

	// check startsolid. interesting if original trace is nonzero length to compare vs. the zero length implementation
	if (mins && maxs)
	{
		if (results->startsolid)
		{
			// slightly bigger box at start shoudl still be solid
			auto minsInflated = VEC2GLM(mins) - 0.1f;
			auto maxsInflated = VEC2GLM(maxs) + 0.1f;
			trace_t startcheck;
			G_CM_Trace(&startcheck, start, &minsInflated[0], &maxsInflated[0], start, passEntityNum, contentmask, skipmask, traceType_t::TT_AABB);
			if (!startcheck.startsolid)
				Log::Warn("a startsolid went away with zero-length trace:start=%s end=%s mins=%s maxs=%s",
					vtos(start), vtos(end), vtos(mins), vtos(maxs));
		}
		else
		{
			// slightly bigger trace shoudl still be solid
			auto minsDeflated = VEC2GLM(mins) + 0.1f;
			auto maxsDeflated = VEC2GLM(maxs) - 0.1f;
			trace_t startcheck;
			G_CM_Trace(&startcheck, start, &minsDeflated[0], &maxsDeflated[0], start, passEntityNum, contentmask, skipmask, traceType_t::TT_AABB);
			if (startcheck.startsolid)
				Log::Warn("a startsolid appeared with zero-length trace:start=%s end=%s mins=%s maxs=%s",
					vtos(start), vtos(end), vtos(mins), vtos(maxs));
		}
	}
}

void trap_Trace( trace_t *results, const glm::vec3& start, const glm::vec3& mins, const glm::vec3& maxs,
                 const glm::vec3& end, int passEntityNum, int contentmask , int skipmask)
{
	trap_Trace( results, &start[0], &mins[0], &maxs[0], &end[0], passEntityNum, contentmask, skipmask );
}

int trap_PointContents(const vec3_t point, int passEntityNum)
{
	return G_CM_PointContents( point, passEntityNum );
}

void trap_SetBrushModel(gentity_t *ent, const char *name)
{
	G_CM_SetBrushModel( ent, name );
}

bool trap_InPVS(const vec3_t p1, const vec3_t p2)
{
	return G_CM_inPVS( p1, p2 );
}

bool trap_InPVSIgnorePortals(const vec3_t p1, const vec3_t p2)
{
	return G_CM_inPVSIgnorePortals( p1, p2 );
}

void trap_AdjustAreaPortalState(gentity_t *ent, bool open)
{
	VM::SendMsg<AdjustAreaPortalStateMsg>(ent->num(), open);
	G_CM_AdjustAreaPortalState( ent, open );
}
