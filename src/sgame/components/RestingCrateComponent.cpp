#include "RestingCrateComponent.h"

RestingCrateComponent::RestingCrateComponent(Entity& entity, HealthComponent& r_HealthComponent, ThinkingComponent& r_ThinkingComponent)
	: RestingCrateComponentBase(entity, r_HealthComponent, r_ThinkingComponent)
{}

void RestingCrateComponent::HandleDie(gentity_t*, meansOfDeath_t)
{
	this->GetThinkingComponent().RegisterThinker([this](int) { G_ExplodeMissile(entity.oldEnt); },
	                                             ThinkingComponent::SCHEDULER_BEFORE, 500);
}
