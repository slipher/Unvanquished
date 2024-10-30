/*
===========================================================================

Unvanquished GPL Source Code
Copyright (C) 2012 Unvanquished Developers

This file is part of the Unvanquished GPL Source Code (Unvanquished Source Code).

Unvanquished Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Unvanquished Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Unvanquished Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Unvanquished Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following the
terms and conditions of the GNU General Public License which accompanied the Unvanquished
Source Code.  If not, please request a copy in writing from id Software at the address
below.

If you have questions concerning this license or the applicable additional terms, you
may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville,
Maryland 20850 USA.

===========================================================================
*/

// this file holds commands that can be executed by the server console, but not remote clients
// For commands that can be performed by any remote client, see sv_cmds.cpp.
// For administrator commands (performed by console *or* remote clients), see sg_admin.cpp.
// TODO: Convert all these commands to Cmd::StaticCmd (which works for these, but not networked commands)

#include "common/Common.h"
#include "common/cm/cm_public.h"
#include "sg_local.h"
#include "botlib/bot_api.h"

#define IS_NON_NULL_VEC3(vec3tor) (vec3tor[0] || vec3tor[1] || vec3tor[2])

class TraceCmd : public Cmd::StaticCmd
{
public:
	TraceCmd() : StaticCmd( "trace", 0, "perform collision-testing trace (for debugging)" ) {}
	void Run( const Cmd::Args& args ) const override
	{
		glm::vec3 start, end;
		glm::vec3 mins{}, maxs{};
		int contents = MASK_ALL;
		int skipmask = 0;
		int passEntityNum = ENTITYNUM_NONE;

		switch ( args.Argc() )
		{
		case 13:
			if ( !Str::ToFloat( args.Argv( 7 ), mins.x ) || !Str::ToFloat( args.Argv( 8 ), mins.y ) || !Str::ToFloat( args.Argv( 9 ), mins.z ) ||
					!Str::ToFloat( args.Argv( 10 ), maxs.x ) || !Str::ToFloat( args.Argv( 11 ), maxs.y ) || !Str::ToFloat( args.Argv( 12 ), maxs.z ) )
				break;
			DAEMON_FALLTHROUGH;
		case 7:
			if ( !Str::ToFloat( args.Argv( 1 ), start.x ) || !Str::ToFloat( args.Argv( 2 ), start.y ) || !Str::ToFloat( args.Argv( 3 ), start.z ) ||
					!Str::ToFloat( args.Argv( 4 ), end.x ) || !Str::ToFloat( args.Argv( 5 ), end.y ) || !Str::ToFloat( args.Argv( 6 ), end.z ) )
				break;
			trace_t trace;
			trap_Trace( &trace, start, mins, maxs, end, passEntityNum, contents, skipmask );
			Print( "startsolid=%s allsolid=%s fraction=%f\nendpos=(%.1f %.1f %.1f)\nplane=(%f %f %f) %.1f\ncontents=0x%08x entityNum=%d",
				trace.startsolid, trace.allsolid, trace.fraction,
				trace.endpos[ 0 ], trace.endpos[ 1 ], trace.endpos[ 2 ],
				trace.plane.normal[ 0 ], trace.plane.normal[ 1 ], trace.plane.normal[ 2 ], trace.plane.dist,
				trace.contents, trace.entityNum );
			return;
		}

		Print( "Usage: trace <x0> <y0> <z0> <x1> <y1> <z1> (<mins.x> <mins.y> <mins.z> <maxs.x> <maxs.y> <maxs.z>)" );
	}
};
static TraceCmd traceRegistration;

class DeltaAnglesCmd : public Cmd::StaticCmd
{
public:
	DeltaAnglesCmd() : StaticCmd( "addDeltaAngles", 0, "perturb a client's view angles (for debugging)" ) {}
	void Run( const Cmd::Args& args ) const override
	{
		vec3_t deltas{};

		switch ( args.Argc() )
		{
		case 5:
			if ( !Cvar::ParseCvarValue( args.Argv( 4 ), deltas[ ROLL ] ) ) break;
			DAEMON_FALLTHROUGH;
		case 4:
			if ( !Cvar::ParseCvarValue( args.Argv( 3 ), deltas[ YAW ] ) ) break;
			DAEMON_FALLTHROUGH;
		case 3:
			if ( !Cvar::ParseCvarValue( args.Argv( 2 ), deltas[ PITCH ] ) ) break;

			int client = G_ClientNumberFromString( args.Argv( 1 ).c_str(), nullptr, 0 );
			if ( client < 0 ) break;

			for ( int i = 0; i < 3; i++ )
			{
				g_clients[ client ].ps.delta_angles[ i ] += ANGLE2SHORT( deltas[ i ] );
			}
			return;
		}

		PrintUsage( args, "<client> <pitch> [yaw [roll]]" );
	}
};
static DeltaAnglesCmd deltaAnglesRegistration;

class ShowBehaviorCmd : public Cmd::StaticCmd
{
public:
	ShowBehaviorCmd() : StaticCmd( "showBehavior", 0, "print the textual representation of a bot behavior tree" ) {}
	void Run( const Cmd::Args& args ) const override
	{
		if ( args.Argc() != 2 )
		{
			PrintUsage( args, "<behavior>" );
			return;
		}

		std::string str = G_BotBehaviorToString( args.Argv( 1 ) );
		if ( str.empty() )
		{
			Print( "behavior `%s` does not exist", args.Argv( 1 ) );
		}
		else
		{
			Print( str );
		}
	}
};
static ShowBehaviorCmd showBehaviorRegistration;

static void Svcmd_EntityFire_f()
{
	char argument[ MAX_STRING_CHARS ];
	int  entityNum;
	gentity_t *selection;
	gentityCall_t call;
	gentityCallDefinition_t callDefinition = { nullptr, ON_DEFAULT, nullptr, nullptr, ECA_DEFAULT };

	if ( trap_Argc() < 2 || trap_Argc() > 3 )
	{
		Log::Notice( "usage: entityFire <entityNum> [<action>]" );
		return;
	}

	trap_Argv( 1, argument, sizeof( argument ) );
	entityNum = atoi( argument );

	if ( entityNum >= level.num_entities || entityNum < MAX_CLIENTS )
	{
		Log::Notice( "invalid entityNum %d", entityNum );
		return;
	}

	selection = &g_entities[entityNum];

	if (!selection->inuse)
	{
		Log::Notice("entity slot %d is not in use", entityNum);
		return;
	}

	if( trap_Argc() >= 3 )
	{
		trap_Argv( 2, argument, sizeof( argument ) );
		callDefinition.action = argument;
		callDefinition.actionType = G_GetCallActionTypeFor( callDefinition.action );
	}

	Log::Notice( "firing %s:%s", etos( selection ), callDefinition.action ? callDefinition.action : "default" );

	if(selection->mapEntity.names[0])
		callDefinition.name = selection->mapEntity.names[0];

	call.definition = &callDefinition;
	call.caller = &g_entities[ ENTITYNUM_NONE ];
	call.activator = &g_entities[ ENTITYNUM_NONE ] ;

	G_CallEntity(selection, &call);
}

class FloodFillCmd : public Cmd::StaticCmd
{
	using octpt = std::array<int, 3>;
	struct oct {
		bool allAccessible; // set on creation
		int dist;
		std::array<std::unique_ptr<oct>, 8> children;
	};

	glm::vec3 gridOrigin_;
	float gridDist_;
	vec3_t trMins_, trMaxs_;
	int dim_;
	int d_;

	void ToWorld(octpt p, vec3_t &out)
	{
		for (int i = 0; i < 3; i++)
			out[i] = gridOrigin_[i] + p[i] * gridDist_;
	}

	bool InBounds(octpt p, int max)
	{
		for (int i = 0; i < 3; i++)
			if (p[i] < 0 || p[i] >= max)
				return false;
		return true;
	}

	bool TraceBlocked(octpt a, octpt b)
	{
		vec3_t a2, b2;
		ToWorld(a, a2);
		ToWorld(b, b2);
		trace_t tr;
		CM_BoxTrace(&tr, a2, b2, trMins_, trMaxs_, 0, CONTENTS_SOLID, 0, traceType_t::TT_AABB);
		return tr.fraction < 1.0f;
	}

	bool CubeBlocked(octpt min, int d)
	{
		vec3_t center;
		ToWorld(min, center);
		float edgeCenterDist = 0.5f * (d - 1) * gridDist_;
		for (float& c : center)
			c += edgeCenterDist;
		vec3_t mins, maxs;
		maxs[0] = maxs[1] = maxs[2] = trMaxs_[0] + edgeCenterDist;
		mins[0] = mins[1] = mins[2] = -maxs[0];
		trace_t tr;
		CM_BoxTrace(&tr, center, center, mins, maxs, 0, CONTENTS_SOLID, 0, traceType_t::TT_AABB);
		return tr.fraction < 1.0f;
	}

	struct LookupResult {
		int d;
		oct* t;
	};
	LookupResult LookUpPoint(oct* t, octpt p)
	{
		ASSERT(InBounds(p, d_));
		octpt tOrg{};
		int d = d_;
		for (;;) {
			int j = 0;
			for (int i = 0; i < 3; i++) {
				if (p[i] & d/2) {
					p[i] ^= d/2;
					tOrg[i] |= d/2;
					j |= 1 << i;
				}
			}
			if (!t->children[j]) {
				return {d, t};
			}
			t = t->children[j].get();
			d /= 2;
		}
	}

	void Return(oct* root, int dist, octpt tOrg)
	{
		int d = 1;
		while (dist > 0) {
			int nextDist = dist;
			int nextD;
			octpt nextPoint;
			for (int i = 0; i < d; i++) {
				for (int j = 0; j < d; j++) {
					octpt edge[3] = {
						{ tOrg[0], tOrg[1] + i, tOrg[2] + j },
						{ tOrg[0] + i, tOrg[1], tOrg[2] + j },
						{ tOrg[0] + i, tOrg[1] + j, tOrg[2] },
					};
					for (int k = 0; k < 3; k++) {
						octpt out = edge[k];
						out[k] -= 1;
						if (InBounds(out, d_)) {
							auto r = LookUpPoint(root, out);
							if (r.t->allAccessible && r.t->dist < nextDist && !TraceBlocked(edge[k], out)) {
								nextDist = r.t->dist;
								nextD = r.d;
								nextPoint = out;
							}
						}
						edge[k][k] += d - 1;
						out[k] += d + 1;
						if (InBounds(out, d_)) {
							auto r = LookUpPoint(root, out);
							if (r.t->allAccessible && r.t->dist < nextDist && !TraceBlocked(edge[k], out)) {
								nextDist = r.t->dist;
								nextD = r.d;
								nextPoint = out;
							}
						}
					}
				}
			}
			if (nextDist >= dist) {
				Print("lost breadcrumbs");
				return;
			}
			dist = nextDist;
			d = nextD;
			for (int i = 0; i < 3; i++)
				tOrg[i] = nextPoint[i] & -d;
			vec3_t worldp;
			ToWorld(tOrg, worldp);
			float edgeCenterDist = 0.5f * (d - 1) * gridDist_;
			for (float& c : worldp)
				c += edgeCenterDist;
			Print("%s (+/- %.0f)", vtos(worldp), edgeCenterDist);
		}
	}

	void Go(octpt origin)
	{
		std::unique_ptr<oct> tree;
		struct qentry {
			octpt p;
			oct* pred;
		};
		std::vector<qentry> q{ {origin, nullptr} };
		while(!q.empty()) {
			qentry qe = q.back();
			q.pop_back();
			if (!InBounds(qe.p, dim_)) {
				vec3_t p2;
				ToWorld(qe.p, p2);
				Print("escaped to %s", vtos(p2));
				Return(tree.get(), qe.pred->dist + 1, qe.p);
				return;
			}

			octpt tOrg{};
			auto* t = &tree;
			int d = d_;
			for(;;) {
				if (!*t) {
					(*t).reset(new oct);
					(*t)->allAccessible = d == 1 || !CubeBlocked(tOrg, d);
					if ((*t)->allAccessible) {
						if (!qe.pred)
							(*t)->dist = 0;
						else
							(*t)->dist = qe.pred->dist + 1;

						for (int i = 0; i < d; i++) {
							for (int j = 0; j < d; j++) {
								octpt edge[3] = {
									{ tOrg[0], tOrg[1] + i, tOrg[2] + j },
									{ tOrg[0] + i, tOrg[1], tOrg[2] + j },
									{ tOrg[0] + i, tOrg[1] + j, tOrg[2] },
								};
								for (int k = 0; k < 3; k++) {
									octpt out = edge[k];
									out[k] -= 1;
									if (!TraceBlocked(edge[k], out))
										q.push_back({ out, (*t).get() });
									edge[k][k] += d - 1;
									out[k] += d + 1;
									if (!TraceBlocked(edge[k], out))
										q.push_back({ out, (*t).get() });
								}
							}
						}

						break;
					}
				} else if ((*t)->allAccessible) {
					(*t)->dist = std::min((*t)->dist, qe.pred->dist + 1);
					break; //already visited
				}

				int j = 0;
				d >>= 1;
				for (int i = 0; i < 3; i++) {
					if (qe.p[i] & d) {
						qe.p[i] ^= d;
						tOrg[i] |= d;
						j |= 1 << i;
					}
				}
				t = &(*t)->children[j];
			}
		}
		Log::Notice("no escape found");
	}

	void FloodFill(glm::vec3 origin, float traceHalfEdge, float gridDist, float worldHalfEdge)
	{
		trMins_[0] = trMins_[1] = trMins_[2] = -traceHalfEdge;
		trMaxs_[0] = trMaxs_[1] = trMaxs_[2] = traceHalfEdge;
		gridDist_ = gridDist;
		int stepsBeforeEdge = static_cast<int>(worldHalfEdge / gridDist);
		dim_ = 2 * stepsBeforeEdge + 1; // dim^3 included grid points
		gridOrigin_ = origin - gridDist * stepsBeforeEdge;
		d_ = 2;
		while(d_ < dim_) d_ <<= 1;

		octpt originp = {stepsBeforeEdge, stepsBeforeEdge, stepsBeforeEdge};
		if (CubeBlocked(originp, 1)) {
			Print("invalid - origin point occupied");
			return;
		}

		Go(originp);
	}

public:
	FloodFillCmd() : StaticCmd("floodfill", "test map for holes") {}

	void Run(const Cmd::Args& args) const override
	{
		float traceHalfEdge, gridDist, worldHalfEdge;
		glm::vec3 origin;
		if (args.Argc() == 7 && Str::ToFloat(args.Argv(1), origin.x)
		                     && Str::ToFloat(args.Argv(2), origin.y)
		                     && Str::ToFloat(args.Argv(3), origin.z)
		                     && Str::ToFloat(args.Argv(4), traceHalfEdge)
		                     && Str::ToFloat(args.Argv(5), gridDist)
		                     && Str::ToFloat(args.Argv(6), worldHalfEdge)) {
			const_cast<FloodFillCmd *>(this)->FloodFill(origin, traceHalfEdge, gridDist, worldHalfEdge);
		} else {
			PrintUsage(args, "x y z t g w",
				"Flood fills the world bounded by (x-w y-w z-w) and (x+w y+w z+w), starting from (x y z),\n"
				"using a trace size mins (-t -t -t) maxs (t t t) and spacing between grid points g");
		}
	}
};
static FloodFillCmd floodFillRegistration;

static inline void PrintEntityOverviewLine( gentity_t *entity )
{
	Log::Notice( "%3i: %15s/^5%-24s^*%s%s",
			entity->num(), Com_EntityTypeName( static_cast<entityType_t>( entity->s.eType ) ), entity->classname,
			entity->mapEntity.names[0] ? entity->mapEntity.names[0] : "", entity->mapEntity.names[1] ? " …" : "");
}

/*
===================
Svcmd_EntityShow_f
===================
*/
static void Svcmd_EntityShow_f()
{
	int       entityNum;
	int       lastTargetIndex, targetIndex;
	gentity_t *selection;
	gentity_t *possibleTarget = nullptr;
	char argument[ 128 ];


	if (trap_Argc() != 2)
	{
		Log::Notice("usage: entityShow <entityNum or entityID>");
		return;
	}

	trap_Argv( 1, argument, sizeof( argument ) );
	if ( !Str::ParseInt( entityNum, argument ) )
	{
		entityNum = G_IdToEntityNum( argument );
		if ( entityNum < 0 )
		{
			Log::Notice( "entity ID `%s` does not exist", argument );
			return;
		}
	}

	if (entityNum >= level.num_entities || entityNum < MAX_CLIENTS)
	{
		Log::Notice("entityNum %d is out of range", entityNum);
		return;
	}

	selection = &g_entities[entityNum];

	if (!selection->inuse)
	{
		Log::Notice("entity slot %d is unused/free", entityNum);
		return;
	}

	Log::Notice( "⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼" );
	Log::Notice( "^5#%3i^*: %16s", entityNum, Com_EntityTypeName( static_cast<entityType_t>( selection->s.eType ) ) );
	if (IS_NON_NULL_VEC3(selection->s.origin))
	{
		Log::Notice("%26s", vtos( selection->s.origin ) );
	}
	Log::Notice( "⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼⎼" );
	Log::Notice( "Classname: ^5%s^*", selection->classname );
	if ( selection->id != nullptr )
	{
		Log::Notice( "ID: ^5%s^*", selection->id );
	}
	Log::Notice( "Capabilities:%s%s%s%s%s%s%s",
			selection->act ? " acts" : "",
			selection->think ? " thinks" : "",
			selection->pain ? " pains" : "",
			selection->die ? " dies" : "",
			selection->reset ? " resets" : "",
			selection->touch ? " touchable" : "",
			selection->use ? " usable" : "");
	if (selection->mapEntity.names[0])
	{
		Log::Notice( "Names: ");
		G_PrintEntityNameList( selection );
	}

	Log::Notice("State: %s", selection->enabled ? "enabled" : "disabled");

	if (selection->mapEntity.groupName)
	{
		Log::Notice("Member of Group: %s%s", selection->mapEntity.groupName, !selection->mapEntity.groupMaster ? " [master]" : "");
	}

	if(selection->mapEntity.targetCount)
	{
		Log::Notice( "Aims at");

		while ((possibleTarget = G_IterateTargets(possibleTarget, &targetIndex, selection)) != nullptr )
		{
			Log::Notice(" • %s %s", etos( possibleTarget ), vtos( possibleTarget->s.origin));
		}
	}

	if(selection->mapEntity.callTargetCount)
	{
		lastTargetIndex = -1;
		while ((possibleTarget = G_IterateCallEndpoints(possibleTarget, &targetIndex, selection)) != nullptr )
		{

			if(lastTargetIndex != targetIndex)
			{
				Log::Notice("Calls %s \"%s:%s\"",
						selection->mapEntity.calltargets[ targetIndex ].event ? selection->mapEntity.calltargets[ targetIndex ].event : "onUnknown",
						selection->mapEntity.calltargets[ targetIndex ].name,
						selection->mapEntity.calltargets[ targetIndex ].action ? selection->mapEntity.calltargets[ targetIndex ].action : "default");
				lastTargetIndex = targetIndex;
			}

			Log::Notice(" • %s", etos(possibleTarget));
			if(possibleTarget->mapEntity.names[1])
			{
				Log::Notice(" using \"%s\" ∈ ", selection->mapEntity.calltargets[ targetIndex ].name);
				G_PrintEntityNameList( possibleTarget );
			}
		}
	}
}

/*
===================
Svcmd_EntityList_f
===================
*/

static void  Svcmd_EntityList_f()
{
	int       entityNum;
	int i;
	int currentEntityCount;
	gentity_t *displayedEntity;
	char* filter;

	displayedEntity = g_entities;

	if(trap_Argc() > 1)
	{
		filter = ConcatArgs( 1 );
	}
	else
	{
		filter = nullptr;
	}

	for ( entityNum = 0, currentEntityCount = 0; entityNum < level.num_entities; entityNum++, displayedEntity++ )
	{
		if ( !displayedEntity->inuse )
		{
			continue;
		}
		currentEntityCount++;

		if(filter && !Com_Filter(filter, displayedEntity->classname, false) )
		{
			for (i = 0; i < MAX_ENTITY_ALIASES && displayedEntity->mapEntity.names[i]; ++i)
			{
				if( Com_Filter(filter, displayedEntity->mapEntity.names[i], false) )
				{
					PrintEntityOverviewLine( displayedEntity );
					break;
				}
			}
			continue;
		}
		PrintEntityOverviewLine( displayedEntity );
	}

	Log::Notice( "A total of %i entities are currently in use.", currentEntityCount);
}

static gclient_t *ClientForString( char *s )
{
	int  idnum;
	char err[ MAX_STRING_CHARS ];

	idnum = G_ClientNumberFromString( s, err, sizeof( err ) );

	if ( idnum == -1 )
	{
		Log::Notice( err );
		return nullptr;
	}

	return &level.clients[ idnum ];
}

/*
===================
Svcmd_EntityLock_f
===================
*/

static void Svcmd_EntityLock_f()
{
	char      argument[ 16 ];
	int       e;
	gentity_t *door;

	if ( trap_Argc() != 2 )
	{
		Log::Notice( "usage: entitylock <entityNum> (tip: use "
		             "'cg_drawEntityInfo on' on your client to find this)" );
		return;
	}

	trap_Argv( 1, argument, sizeof( argument ) );

	if ( !Str::ParseInt( e, argument ) )
	{
		Log::Warn( "entitylock: this is not numeric: %s", argument );
		return;
	}

	if ( e >= level.num_entities || e < MAX_CLIENTS )
	{
		Log::Warn( "entitylock: not a valid entity" );
		return;
	}

	door = &g_entities[ e ];

	if ( door->s.eType != entityType_t::ET_MOVER )
	{
		Log::Notice( "entitylock: must be a ^5MOVER ^*entity "
		             "such as a door, button or platform");
		return;
	}

	door->mapEntity.locked = !door->mapEntity.locked;

	if ( door->mapEntity.locked )
	{
		glm::vec3 mins = VEC2GLM( door->mapEntity.restingPosition ) + VEC2GLM( door->r.mins );
		glm::vec3 maxs = VEC2GLM( door->mapEntity.restingPosition ) + VEC2GLM( door->r.maxs );
		G_BotAddObstacle( mins, maxs, door->num() );
	}
	else
	{
		G_BotRemoveObstacle( door->num() );
	}

	Log::Notice( "entitylock: entity ^5%s^7#^5%d^* %s",
	             door->classname, e, door->mapEntity.locked ? "locked" : "unlocked" );
}

/*
===================
Svcmd_ForceTeam_f

forceteam <player> <team>
===================
*/
static void Svcmd_ForceTeam_f()
{
	gclient_t *cl;
	char      str[ MAX_TOKEN_CHARS ];
	team_t    team;

	if ( trap_Argc() != 3 )
	{
		Log::Notice( "usage: forceteam <player> <team>" );
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );
	cl = ClientForString( str );

	if ( !cl )
	{
		return;
	}

	trap_Argv( 2, str, sizeof( str ) );
	team = G_TeamFromString( str );

	if ( team == NUM_TEAMS )
	{
		Log::Notice( "forceteam: invalid team \"%s\"", str );
		return;
	}

	G_ChangeTeam( cl->ent(), team );
}

/*
===================
Svcmd_LayoutSave_f

layoutsave <name>
===================
*/
static void Svcmd_LayoutSave_f()
{
	char str[ MAX_QPATH ];
	char str2[ MAX_QPATH - 4 ];
	char *s;
	int  i = 0;

	if ( trap_Argc() != 2 )
	{
		Log::Notice( "usage: layoutsave <name>" );
		return;
	}

	trap_Argv( 1, str, sizeof( str ) );

	// sanitize name
	s = &str[ 0 ];
	str2[ 0 ] = 0;

	while ( *s && i < (int) sizeof( str2 ) - 1 )
	{
		if ( Str::cisalnum( *s ) || *s == '-' || *s == '_' )
		{
			str2[ i++ ] = *s;
			str2[ i ] = '\0';
		}

		s++;
	}

	if ( !str2[ 0 ] )
	{
		Log::Notice( "layoutsave: invalid name \"%s\"", str );
		return;
	}

	G_LayoutSave( str2 );
}

char *ConcatArgs( int start );

/*
===================
Svcmd_LayoutLoad_f

layoutload [<name> [<name2> [<name3 [...]]]]

This is just a silly alias for doing:
 set g_layouts "name name2 name3"
 map_restart
===================
*/
static void Svcmd_LayoutLoad_f()
{
	char layouts[ MAX_CVAR_VALUE_STRING ];
	char *s;

	if ( trap_Argc() < 2 )
	{
		Log::Notice( "usage: layoutload <name> …" );
		return;
	}

	s = ConcatArgs( 1 );
	Q_strncpyz( layouts, s, sizeof( layouts ) );
	g_layouts.Set(layouts);
	trap_SendConsoleCommand( "map_restart\n" );
	level.restarted = true;
}

static void Svcmd_AdmitDefeat_f()
{
	int  team;
	char teamNum[ 2 ];

	if ( trap_Argc() != 2 )
	{
		Log::Notice( "admitdefeat: must provide a team" );
		return;
	}

	trap_Argv( 1, teamNum, sizeof( teamNum ) );
	team = G_TeamFromString( teamNum );

	if ( team == TEAM_ALIENS )
	{
		G_TeamCommand( TEAM_ALIENS, "cp \"Hivemind Link Broken\" 1" );
		trap_SendServerCommand( -1, "print_tr " QQ( N_("Alien team has admitted defeat") ) );
	}
	else if ( team == TEAM_HUMANS )
	{
		G_TeamCommand( TEAM_HUMANS, "cp \"Life Support Terminated\" 1" );
		trap_SendServerCommand( -1, "print_tr " QQ( N_("Human team has admitted defeat") ) );
	}
	else
	{
		Log::Notice( "admitdefeat: invalid team" );
		return;
	}

	level.surrenderTeam = (team_t) team;
	G_BaseSelfDestruct( (team_t) team );
}

static void Svcmd_TeamWin_f()
{
	// this is largely made redundant by admitdefeat <team>
	char cmd[ 6 ];
	team_t team;
	trap_Argv( 0, cmd, sizeof( cmd ) );

	team = G_TeamFromString( cmd );

	if ( TEAM_ALIENS == team )
	{
		G_BaseSelfDestruct( TEAM_HUMANS );
	}
	if ( TEAM_HUMANS == team )
	{
		G_BaseSelfDestruct( TEAM_ALIENS );
	}
}

static void Svcmd_Evacuation_f()
{
	trap_SendServerCommand( -1, "print_tr " QQ( N_("Evacuation ordered") ) );
	level.lastWin = TEAM_NONE;
	trap_SetConfigstring( CS_WINNER, "Evacuation" );
	G_notify_sensor_end( TEAM_NONE );
	LogExit( "Evacuation." );
	G_MapLog_Result( 'd' );
}

static void Svcmd_MapRotation_f()
{
	char rotationName[ MAX_QPATH ];

	if ( trap_Argc() != 2 )
	{
		Log::Notice( "usage: maprotation <name>" );
		return;
	}

	G_ClearRotationStack();

	trap_Argv( 1, rotationName, sizeof( rotationName ) );

	if ( !G_StartMapRotation( rotationName, false, true, false, 0 ) )
	{
		Log::Notice( "maprotation: invalid map rotation \"%s\"", rotationName );
	}
}

static void Svcmd_TeamMessage_f()
{
	char   teamNum[ 2 ];
	team_t team;
	char   *arg;

	if ( trap_Argc() < 3 )
	{
		Log::Notice( "usage: say_team <team> <message>" );
		return;
	}

	trap_Argv( 1, teamNum, sizeof( teamNum ) );
	team = G_TeamFromString( teamNum );

	if ( team == NUM_TEAMS )
	{
		Log::Notice( "say_team: invalid team \"%s\"", teamNum );
		return;
	}

	arg = ConcatArgs( 2 );
	G_TeamCommand( team, va( "chat -1 %d %s", SAY_TEAM, Quote( arg ) ) );
	G_LogPrintf( "SayTeam: -1 \"console\": %s", arg );
}

static void Svcmd_CenterPrint_f()
{
	if ( trap_Argc() < 2 )
	{
		Log::Notice( "usage: cp <message>" );
		return;
	}

	trap_SendServerCommand( -1, va( "cp %s", Quote( ConcatArgs( 1 ) ) ) );
}

static void Svcmd_EjectClient_f()
{
	char *reason, name[ MAX_STRING_CHARS ];

	if ( trap_Argc() < 2 )
	{
		Log::Notice( "usage: eject <player|-1> <reason>" );
		return;
	}

	trap_Argv( 1, name, sizeof( name ) );
	reason = ConcatArgs( 2 );

	if ( atoi( name ) == -1 )
	{
		int i;

		for ( i = 0; i < level.maxclients; i++ )
		{
			if ( level.clients[ i ].pers.connected == CON_DISCONNECTED )
			{
				continue;
			}

			if ( level.clients[ i ].pers.localClient )
			{
				continue;
			}

			trap_DropClient( i, reason );
		}
	}
	else
	{
		gclient_t *cl = ClientForString( name );

		if ( !cl )
		{
			return;
		}

		if ( cl->pers.localClient )
		{
			Log::Notice( "eject: cannot eject local clients" );
			return;
		}

		trap_DropClient( cl->num(), reason );
	}
}

static void Svcmd_DumpUser_f()
{
	char       name[ MAX_STRING_CHARS ], userinfo[ MAX_INFO_STRING ];
	char       key[ BIG_INFO_KEY ], value[ BIG_INFO_VALUE ];
	const char *info;
	gclient_t  *cl;

	if ( trap_Argc() != 2 )
	{
		Log::Notice( "usage: dumpuser <player>" );
		return;
	}

	trap_Argv( 1, name, sizeof( name ) );
	cl = ClientForString( name );

	if ( !cl )
	{
		return;
	}

	trap_GetUserinfo( cl->num(), userinfo, sizeof( userinfo ) );
	info = &userinfo[ 0 ];
	Log::Notice( "userinfo--------" );

	//Info_Print( userinfo );
	while ( 1 )
	{
		Info_NextPair( &info, key, value );

		if ( !*info )
		{
			return;
		}

		Log::Notice( "%-20s%s", key, value );
	}
}

static void Svcmd_Pr_f()
{
	char targ[ 4 ];
	int  cl;

	if ( trap_Argc() < 3 )
	{
		Log::Notice( "usage: <clientnum|-1> <message>" );
		return;
	}

	trap_Argv( 1, targ, sizeof( targ ) );
	cl = atoi( targ );

	if ( cl >= MAX_CLIENTS || cl < -1 )
	{
		Log::Notice( "invalid clientnum %d", cl );
		return;
	}

	trap_SendServerCommand( cl, va( "print %s", Quote( ConcatArgs( 2 ) ) ) );
}

static void Svcmd_PrintQueue_f()
{
	team_t team;
	char teamName[ MAX_STRING_CHARS ];

	if ( trap_Argc() != 2 )
	{
		Log::Notice( "usage: printqueue <team>" );
		return;
	}

	trap_Argv( 1, teamName, sizeof( teamName ) );

	team = G_TeamFromString(teamName);
	if ( G_IsPlayableTeam( team ) )
	{
		G_PrintSpawnQueue( &level.team[ team ].spawnQueue );
	}
	else
	{
		Log::Notice( "unknown team" );
	}
}

// dumb wrapper for "a", "m", "chat", and "say"
static void Svcmd_MessageWrapper()
{
	char cmd[ 5 ];
	trap_Argv( 0, cmd, sizeof( cmd ) );

	if ( !Q_stricmp( cmd, "a" ) )
	{
		Cmd_AdminMessage_f( nullptr );
	}
	else if ( !Q_stricmp( cmd, "asay" ) )
	{
		G_Say( nullptr, SAY_ALL_ADMIN, ConcatArgs( 1 ) );
	}
	else if ( !Q_stricmp( cmd, "m" ) )
	{
		Cmd_PrivateMessage_f( nullptr );
	}
	else if ( !Q_stricmp( cmd, "say" ) )
	{
		G_Say( nullptr, SAY_ALL, ConcatArgs( 1 ) );
	}
	else if ( !Q_stricmp( cmd, "chat" ) )
	{
		G_Say( nullptr, SAY_RAW, ConcatArgs( 1 ) );
	}
}

static void Svcmd_MapLogWrapper()
{
	Cmd_MapLog_f( nullptr );
}

static void Svcmd_G_AdvanceMapRotation_f()
{
	G_AdvanceMapRotation( 0 );
}

static const struct svcmd
{
	const char *cmd;
	bool conflicts; //With a command registered by cgame
	void ( *function )();
} svcmds[] =
{
	{ "a",                  true,  Svcmd_MessageWrapper         },
	{ "admitDefeat",        false, Svcmd_AdmitDefeat_f          },
	{ "advanceMapRotation", false, Svcmd_G_AdvanceMapRotation_f },
	{ "alienWin",           false, Svcmd_TeamWin_f              },
	{ "asay",               true,  Svcmd_MessageWrapper         },
	{ "chat",               true,  Svcmd_MessageWrapper         },
	{ "cp",                 false, Svcmd_CenterPrint_f          },
	{ "dumpuser",           false, Svcmd_DumpUser_f             },
	{ "eject",              false, Svcmd_EjectClient_f          },
	{ "entityFire",         false, Svcmd_EntityFire_f           },
	{ "entityList",         false, Svcmd_EntityList_f           },
	{ "entityLock",         false, Svcmd_EntityLock_f           },
	{ "entityShow",         false, Svcmd_EntityShow_f           },
	{ "evacuation",         false, Svcmd_Evacuation_f           },
	{ "forceTeam",          false, Svcmd_ForceTeam_f            },
	{ "humanWin",           false, Svcmd_TeamWin_f              },
	{ "layoutLoad",         false, Svcmd_LayoutLoad_f           },
	{ "layoutSave",         false, Svcmd_LayoutSave_f           },
	{ "m",                  true,  Svcmd_MessageWrapper         },
	{ "maplog",             true,  Svcmd_MapLogWrapper          },
	{ "mapRotation",        false, Svcmd_MapRotation_f          },
	{ "pr",                 false, Svcmd_Pr_f                   },
	{ "printqueue",         false, Svcmd_PrintQueue_f           },
	{ "say",                true,  Svcmd_MessageWrapper         },
	{ "say_team",           true,  Svcmd_TeamMessage_f          },
	{ "stopMapRotation",    false, G_StopMapRotation            },
};

/*
=================
ConsoleCommand

=================
*/
bool  ConsoleCommand()
{
	char         cmd[ MAX_TOKEN_CHARS ];
	struct svcmd *command;

	trap_Argv( 0, cmd, sizeof( cmd ) );

	command = (struct svcmd*) bsearch( cmd, svcmds, ARRAY_LEN( svcmds ),
	                   sizeof( struct svcmd ), cmdcmp );

	if ( !command )
	{
		// see if this is an admin command
		if ( G_admin_cmd_check( nullptr ) )
		{
			return true;
		}

		if ( level.inClient )
		{
			Log::Warn( "unknown command server console command: %s", cmd );
		}

		return false;
	}

	if ( command->conflicts && level.inClient )
	{
		return false;
	}

	command->function();
	return true;
}

void CompleteCommand(int)
{
}

void G_RegisterCommands()
{
	for ( unsigned i = 0; i < ARRAY_LEN( svcmds ); i++ )
	{
		if ( svcmds[ i ].conflicts && level.inClient )
		{
			continue;
		}

		trap_AddCommand( svcmds[ i ].cmd );
	}

	G_admin_register_cmds();
	BotRegisterNavEdit();
}

void G_UnregisterCommands()
{
	for ( unsigned i = 0; i < ARRAY_LEN( svcmds ); i++ )
	{
		if ( svcmds[ i ].conflicts && level.inClient )
		{
			continue;
		}

		trap_RemoveCommand( svcmds[ i ].cmd );
	}

	G_admin_unregister_cmds();
}
