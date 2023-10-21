#include "GreenCrateComponent.h"

GreenCrateComponent::GreenCrateComponent(Entity& entity, HealthComponent& r_HealthComponent, ThinkingComponent& r_ThinkingComponent)
	: GreenCrateComponentBase(entity, r_HealthComponent, r_ThinkingComponent)
{
	glm::vec3 mins = VEC2GLM(entity.oldEnt->r.currentOrigin) - 25.f;
	glm::vec3 maxs = VEC2GLM(entity.oldEnt->r.currentOrigin) + 25.f;
	mins.z -= 1000;
	maxs.z += 1000;
	G_BotAddObstacle(mins, maxs, entity.oldEnt->num());
}

void GreenCrateComponent::HandleDie(gentity_t* killer, meansOfDeath_t meansOfDeath) {
	entity.Get<MissileComponent>()->Explode();
	G_BotRemoveObstacle(entity.oldEnt->num());
}
