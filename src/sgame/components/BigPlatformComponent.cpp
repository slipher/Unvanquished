#include "BigPlatformComponent.h"
#include "sgame/sg_local.h"
#include "sgame/CBSE.h"

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
	REGISTER_THINKER(AddCrates, ThinkingComponent::SCHEDULER_AVERAGE, 1000);
}

static gentity_t *SpawnDumbMissileRestingCrate( missile_t missile, gentity_t *parent, const glm::vec3 &start, const glm::vec3 &dir )
{
	gentity_t *m = G_NewEntity( HAS_CBSE );
	RestingCrateEntity::Params params;
	params.oldEnt = m;
	params.Health_maxHealth = 5;
	params.Missile_attributes = BG_Missile( missile );
	m->entity = new RestingCrateEntity{ params };
	G_SetUpMissile( m, parent, GLM4READ( start ), GLM4READ( dir ) );
	return m;
}

static void TryAddCrate(const glm::vec3& location)
{
	trace_t trace;
	vec3_t mins, maxs;
	glm::vec3 end{ location[0], location[1], location[2] - 1 };
	const missileAttributes_t* ma = BG_Missile(MIS_CRATE);
	BG_MissileBounds(ma, mins, maxs);
	trap_Trace(&trace, location, VEC2GLM(mins), VEC2GLM(maxs), location, ENTITYNUM_NONE, MASK_ALL, 0);
	if (trace.fraction < 1)
		return;

	glm::vec3 velocity{0, 0, -0.01};
	// EF_NO_BOUNCE_SOUND to distinguish crate sitting on ground from a throw one
	gentity_t* crate = SpawnDumbMissileRestingCrate(MIS_CRATE, &g_entities[ENTITYNUM_NONE], location, velocity);
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
	glm::vec3 location{ platformMins[0] + xlerp * (platformMaxs[0] - platformMins[0]),
	                 platformMins[1] + ylerp * (platformMaxs[1] - platformMins[1]),
	                 platformMaxs[2] - crateMins[2] + 3 };
	TryAddCrate(location);
}
