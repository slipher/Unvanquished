#include "BigPlatformComponent.h"
#include "sgame/sg_local.h"

BigPlatformComponent::BigPlatformComponent(Entity& entity, HumanBuildableComponent& r_HumanBuildableComponent)
	: BigPlatformComponentBase(entity, r_HumanBuildableComponent)
{
	REGISTER_THINKER(AddCrates, ThinkingComponent::SCHEDULER_AVERAGE, 1000);
}

void BigPlatformComponent::AddCrates(int timeDelta)
{
	static bool done;
	if (done)return;
	done = true;
	vec3_t unused, platformMaxs, crateMins;
	BG_BuildableBoundingBox( BA_H_BIGPLATFORM, unused, platformMaxs);
	BG_MissileBounds(BG_Missile(MIS_CRATE), crateMins, unused);
	vec3_t location{ entity.oldEnt->s.origin[0] + 100, entity.oldEnt->s.origin[1] + 100,
		entity.oldEnt->s.origin[2] + platformMaxs[2] - crateMins[2] + 3 };
	vec3_t velocity{0, 0, -0.01};
	// EF_NO_BOUNCE_SOUND to distinguish crate sitting on ground from a throw one
	G_SpawnMissile( MIS_CRATE, &g_entities[ENTITYNUM_NONE], location, velocity, nullptr, G_ExplodeMissile, INT_MAX )
		->s.eFlags |= EF_NO_BOUNCE_SOUND | EF_BOUNCE_HALF;
}
