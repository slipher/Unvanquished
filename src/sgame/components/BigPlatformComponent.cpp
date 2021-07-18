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
	REGISTER_THINKER(AddCrates, ThinkingComponent::SCHEDULER_AVERAGE, 3000);
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
	vec3_t unused, platformMaxs, crateMins;
	BG_BuildableBoundingBox( BA_H_BIGPLATFORM, unused, platformMaxs);
	BG_MissileBounds(BG_Missile(MIS_CRATE), crateMins, unused);
	vec3_t location{ entity.oldEnt->s.origin[0] + 100, entity.oldEnt->s.origin[1] + 100,
		entity.oldEnt->s.origin[2] + platformMaxs[2] - crateMins[2] + 3 };
	TryAddCrate(location);
}
