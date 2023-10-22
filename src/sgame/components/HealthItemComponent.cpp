#include "HealthItemComponent.h"

HealthItemComponent::HealthItemComponent(Entity& entity, ThinkingComponent& r_ThinkingComponent)
	: HealthItemComponentBase(entity, r_ThinkingComponent)
{
	REGISTER_THINKER(CheckPickup, ThinkingComponent::SCHEDULER_BEFORE, 0);
}

void HealthItemComponent::CheckPickup(int)
{
	bool picked = false;
	ForEntities<HumanClassComponent>([&](Entity& player, HumanClassComponent&) {
		if (picked) return;
		if (G_DistanceToBBox(VEC2GLM(entity.oldEnt->s.origin), player.oldEnt) > 5) return;

		picked = true;
		// dead players intentionally included
		player.Heal(7, entity.oldEnt);
		entity.FreeAt(DeferredFreeingComponent::FREE_AFTER_THINKING);
	});
}
