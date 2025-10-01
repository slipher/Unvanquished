#include "InertCrateComponent.h"

InertCrateComponent::InertCrateComponent(Entity& entity, HealthComponent& r_HealthComponent, ThinkingComponent& r_ThinkingComponent)
	: InertCrateComponentBase(entity, r_HealthComponent, r_ThinkingComponent)
{}

void InertCrateComponent::HandleDie(gentity_t* killer, meansOfDeath_t meansOfDeath)
{
	entity.Get<MissileComponent>()->Explode();
}
