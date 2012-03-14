/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2000-2009 Darklegion Development

This file is part of Daemon.

Daemon is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Daemon is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Daemon; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "g_local.h"

/*
================
G_Bounce

================
*/
static void G_Bounce( gentity_t *ent, trace_t *trace )
{
	vec3_t   velocity;
	float    dot;
	int      hitTime;
	float    minNormal;
	qboolean invert = qfalse;

	// reflect the velocity on the trace plane
	hitTime = level.previousTime + ( level.time - level.previousTime ) * trace->fraction;
	BG_EvaluateTrajectoryDelta( &ent->s.pos, hitTime, velocity );
	dot = DotProduct( velocity, trace->plane.normal );
	VectorMA( velocity, -2 * dot, trace->plane.normal, ent->s.pos.trDelta );

	if ( ent->s.eType == ET_BUILDABLE )
	{
		minNormal = BG_Buildable( ent->s.modelindex )->minNormal;
		invert = BG_Buildable( ent->s.modelindex )->invertNormal;
	}
	else
	{
		minNormal = 0.707f;
	}

	// cut the velocity to keep from bouncing forever
	if ( ( trace->plane.normal[ 2 ] >= minNormal ||
	       ( invert && trace->plane.normal[ 2 ] <= -minNormal ) ) &&
	     trace->entityNum == ENTITYNUM_WORLD )
	{
		VectorScale( ent->s.pos.trDelta, ent->physicsBounce, ent->s.pos.trDelta );
	}
	else
	{
		VectorScale( ent->s.pos.trDelta, 0.3f, ent->s.pos.trDelta );
	}

	if ( VectorLength( ent->s.pos.trDelta ) < 10 )
	{
		VectorMA( trace->endpos, 0.5f, trace->plane.normal, trace->endpos );  // make sure it is off ground
		G_SetOrigin( ent, trace->endpos );
		ent->s.groundEntityNum = trace->entityNum;
		VectorCopy( trace->plane.normal, ent->s.origin2 );
		VectorSet( ent->s.pos.trDelta, 0.0f, 0.0f, 0.0f );
		return;
	}

	VectorCopy( ent->r.currentOrigin, ent->s.pos.trBase );
	VectorAdd( ent->r.currentOrigin, trace->plane.normal, ent->r.currentOrigin );
	ent->s.pos.trTime = level.time;
}

#define PHYSICS_TIME 200

/*
================
G_Physics

================
*/
void G_Physics( gentity_t *ent, int msec )
{
	vec3_t  origin;
	trace_t tr;
	int     contents;
	int     mask;

	// if groundentity has been set to -1, it may have been pushed off an edge
	if ( ent->s.groundEntityNum == -1 )
	{
		if ( ent->s.eType == ET_BUILDABLE )
		{
			if ( ent->s.pos.trType != BG_Buildable( ent->s.modelindex )->traj )
			{
				ent->s.pos.trType = BG_Buildable( ent->s.modelindex )->traj;
				ent->s.pos.trTime = level.time;
			}
		}
		else if ( ent->s.pos.trType != TR_GRAVITY )
		{
			ent->s.pos.trType = TR_GRAVITY;
			ent->s.pos.trTime = level.time;
		}
	}

	// trace a line from the previous position to the current position
	if ( ent->clipmask )
	{
		mask = ent->clipmask;
	}
	else
	{
		mask = MASK_DEADSOLID;
	}

	if ( ent->s.pos.trType == TR_STATIONARY )
	{
		// check think function
		G_RunThink( ent );

		//check floor infrequently
		if ( ent->nextPhysicsTime < level.time )
		{
			VectorCopy( ent->r.currentOrigin, origin );

			VectorMA( origin, -2.0f, ent->s.origin2, origin );

			trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, ent->s.number, mask );

			if ( tr.fraction == 1.0f )
			{
				ent->s.groundEntityNum = -1;
			}

			ent->nextPhysicsTime = level.time + PHYSICS_TIME;
		}

		return;
	}

	// get current position
	BG_EvaluateTrajectory( &ent->s.pos, level.time, origin );

	trap_Trace( &tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, ent->s.number, mask );

	VectorCopy( tr.endpos, ent->r.currentOrigin );

	if ( tr.startsolid )
	{
		tr.fraction = 0;
	}

	trap_LinkEntity( ent );  // FIXME: avoid this for stationary?

	// check think function
	G_RunThink( ent );

	if ( tr.fraction == 1.0f )
	{
		return;
	}

	// if it is in a nodrop volume, remove it
	contents = trap_PointContents( ent->r.currentOrigin, -1 );

	if ( contents & CONTENTS_NODROP )
	{
		if ( ent->s.eType == ET_BUILDABLE )
		{
			G_RemoveRangeMarkerFrom( ent );
		}

		G_FreeEntity( ent );
		return;
	}

	G_Bounce( ent, &tr );
}
