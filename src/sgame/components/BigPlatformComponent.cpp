#include "BigPlatformComponent.h"
#include "sgame/sg_local.h"
#include "sgame/CBSE.h"
#include "sgame/Entities.h"

#define MAX_PLAYERS 4

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

void BigPlatformComponent::Bounds(vec3_t mins, vec3_t maxs)
{
	BG_BuildableBoundingBox(BA_H_BIGPLATFORM, mins, maxs);
	VectorAdd(mins, entity.oldEnt->s.origin, mins);
	VectorAdd(maxs, entity.oldEnt->s.origin, maxs);
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
	RestingCrateEntity::Params p;
	p.oldEnt = crate;
	p.Health_maxHealth = 5;
	ASSERT_EQ(crate->entity, level.emptyEntity);
	crate->entity = new RestingCrateEntity(p);
}

void BigPlatformComponent::AddCrates(int timeDelta)
{
	vec3_t unused, platformMins, platformMaxs, crateMins;
	Bounds(platformMins, platformMaxs);
	float xlerp = 0.1 + random() * 0.8;
	float ylerp = 0.1 + random() * 0.8;
	BG_MissileBounds(BG_Missile(MIS_CRATE), crateMins, unused);
	vec3_t location{ platformMins[0] + xlerp * (platformMaxs[0] - platformMins[0]),
	                 platformMins[1] + ylerp * (platformMaxs[1] - platformMins[1]),
	                 platformMaxs[2] - crateMins[2] + 3 };
	TryAddCrate(location);
}

void BigPlatformComponent::Place(gentity_t* player, int position)
{
	vec3_t platformMins, platformMaxs;
	Bounds(platformMins, platformMaxs);
	float xlerp = position & 1 ? 0.95 : 0.05;
	float ylerp = position & 2 ? 0.95 : 0.05;
	vec3_t location{ platformMins[0] + xlerp * (platformMaxs[0] - platformMins[0]),
	                 platformMins[1] + ylerp * (platformMaxs[1] - platformMins[1]),
	                 platformMaxs[2] - player->r.mins[2] + 3 };
	//VectorCopy(place, player->s.origin);
	vec3_t angles{}; //TODO
	G_TeleportPlayer(player, location, angles, 0.0f);
}

void BigPlatformComponent::ImmobilizePlayers(int)
{
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (players_[i])
			Place(players_[i].entity, i);
	}
	if (countdown_ < 0)
		GetThinkingComponent().UnregisterActiveThinker();
}

void BigPlatformComponent::Countdown(int)
{
	if (countdown_ > 0) {
		trap_SendServerCommand(-1, Str::Format("cp %d 60", countdown_).c_str());
	} else {
		trap_SendServerCommand(-1, "cp Go! 8");
		for (auto player : players_) {
			if (player) player->flags &= ~FL_GODMODE;
		}
		GetThinkingComponent().UnregisterActiveThinker();
	}
	--countdown_;
}

static void KillBox(gentity_t* ent, vec3_t mins, vec3_t maxs)
{
	int       i, num;
	int       touch[MAX_GENTITIES];
	gentity_t* hit;

	num = trap_EntitiesInBox(mins, maxs, touch, MAX_GENTITIES);

	for (i = 0; i < num; i++)
	{
		hit = &g_entities[touch[i]];

		// impossible to telefrag self
		if (ent == hit)
		{
			continue;
		}

		Entities::Kill(hit, ent, MOD_TELEFRAG);
	}
}

void BigPlatformComponent::CleanUp()
{
	players_ = {};

	vec3_t platformMins, platformMaxs;
	BG_BuildableBoundingBox(BA_H_BIGPLATFORM, platformMins, platformMaxs);
	for (vec3_t* v : { &platformMins, &platformMaxs }) {
		(*v)[0] *= 1.5;
		(*v)[1] *= 1.5;
		(*v)[2] *= 10;
	}
	VectorAdd(platformMins, entity.oldEnt->s.origin, platformMins);
	VectorAdd(platformMaxs, entity.oldEnt->s.origin, platformMaxs);
	KillBox(entity.oldEnt, platformMins, platformMaxs);
}

void BigPlatformComponent::StartRound(const std::vector<gentity_t*>& players)
{
	CleanUp();
	ASSERT_LT(players.size(), MAX_PLAYERS);
	for (size_t i = 0; i < players.size(); i++) {
		players[i]->flags |= FL_GODMODE;
		players_[i] = players[i];
	}
	REGISTER_THINKER(ImmobilizePlayers, ThinkingComponent::SCHEDULER_BEFORE, 1);
	countdown_ = 3;
	Countdown(0);
	REGISTER_THINKER(Countdown, ThinkingComponent::SCHEDULER_AVERAGE, 1000);
}
