#include "GreenCrateComponent.h"

GreenCrateComponent::GreenCrateComponent(Entity& entity, HealthComponent& r_HealthComponent, ThinkingComponent& r_ThinkingComponent)
	: GreenCrateComponentBase(entity, r_HealthComponent, r_ThinkingComponent)
{}

void GreenCrateComponent::HandleDie(gentity_t* killer, meansOfDeath_t meansOfDeath) {
	entity.Get<MissileComponent>()->Explode();
}
