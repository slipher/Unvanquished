#include "BigPlatformComponent.h"
#include "sgame/sg_local.h"

static void UseCrate(gentity_t* crate, gentity_t* player, gentity_t*)
{
	if (player->client->ps.weapon == WP_CRATE)
		return;
	G_ForceWeaponChange( player, WP_CRATE );
	G_FreeEntity(crate);
}

BigPlatformComponent::BigPlatformComponent(Entity& entity, HumanBuildableComponent& r_HumanBuildableComponent)
	: BigPlatformComponentBase(entity, r_HumanBuildableComponent)
{
	REGISTER_THINKER(AddCrates, ThinkingComponent::SCHEDULER_AVERAGE, 200);
}


static void SetBuildableLinkState()
{
	int       i;
	gentity_t *ent;

	for ( i = MAX_CLIENTS, ent = g_entities + i; i < level.num_entities; i++, ent++ )
	{
		if ( ent->s.eType != entityType_t::ET_MISSILE )
		{
			continue;
		}

		gentity_t* a = g_entities + 800;
		memcpy(a, g_entities, sizeof(*a));

		// important // memset(&a->r, 0, sizeof(a->r));
		memset(&a->s, 0, sizeof(a->s));

		sizeof(a->r);
		memset((char*)(&a->r) + 0, 0, 56);
		constexpr int joeue = offsetof(entityShared_t, contents);


		memset((char*)(&a->r) + 60, 0, 140);
		//important // memset((char*)(&a->r) + 48, 0, 52);

		

		//VectorCopy(ent->r.mins, a->r.mins);
		//VectorCopy(ent->r.maxs, a->r.maxs);

		//VectorCopy(ent->r.absmin, a->r.absmin); // not important
		//VectorCopy(ent->r.absmax, a->r.absmax);

		a->s.number = 800;
		VectorCopy(ent->s.origin, a->s.origin);
		a->s.origin[0] += 50;
		a->client = nullptr;
		VectorCopy(ent->r.currentOrigin, a->r.currentOrigin);
		a->r.currentOrigin[0] += 50;
		a->r.ownerNum = ENTITYNUM_NONE;
		//a->r.contents = g_entities[0].r.contents;
		//ent->r.ownerNum = ENTITYNUM_NONE;
		a->r.contents = CONTENTS_BODY;

		//ent->r.contents = CONTENTS_BODY;
		//ent->clipmask   = MASK_PLAYERSOLID;

		{
			trap_LinkEntity( a );
		}
	}
}

static void TryAddCrate(const vec3_t location)
{
	trace_t trace;
	vec3_t mins, maxs;
	vec3_t end{ location[0], location[1], location[2] - 1 };
	const missileAttributes_t* ma = BG_Missile(MIS_CRATE);
	BG_MissileBounds(ma, mins, maxs);
	trap_Trace(&trace, location, mins, maxs, location, ENTITYNUM_NONE, MASK_ALL, 0);
	if (trace.fraction < 1)
		return;

	vec3_t velocity{0, 0, -0.01};
	// EF_NO_BOUNCE_SOUND to distinguish crate sitting on ground from a throw one
	gentity_t* crate = G_SpawnMissile(MIS_CRATE, &g_entities[ENTITYNUM_NONE], location, velocity, nullptr, G_ExplodeMissile, INT_MAX);
	crate->s.eFlags |= EF_NO_BOUNCE_SOUND | EF_BOUNCE_HALF;
	crate->use = UseCrate;
}

void BigPlatformComponent::AddCrates(int timeDelta)
{
	vec3_t unused, platformMins, platformMaxs, crateMins;
	BG_BuildableBoundingBox( BA_H_BIGPLATFORM, platformMins, platformMaxs);
	VectorAdd(platformMins, entity.oldEnt->s.origin, platformMins);
	VectorAdd(platformMaxs, entity.oldEnt->s.origin, platformMaxs);
	float xlerp = 0.1 + random() * 0.8;
	float ylerp = 0.1 + random() * 0.8;
	BG_MissileBounds(BG_Missile(MIS_CRATE), crateMins, unused);
	vec3_t location{ platformMins[0] + xlerp * (platformMaxs[0] - platformMins[0]),
	                 platformMins[1] + ylerp * (platformMaxs[1] - platformMins[1]),
	                 platformMaxs[2] - crateMins[2] + 3 };
	TryAddCrate(location);
}
