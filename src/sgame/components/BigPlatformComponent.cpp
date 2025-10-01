#include "BigPlatformComponent.h"
#include "sgame/sg_local.h"
#include "sgame/CBSE.h"
#include "sgame/Entities.h"

#define MAX_PLAYERS 4

static Cvar::Cvar<int> crateInterval("bash_crateInterval", "interval in ms for adding crates", Cvar::NONE, 1000);
static Cvar::Cvar<int> healthItemInterval(
	"bash_healthItemInterval", "interval in ms for adding health items", Cvar::NONE, 2777);

static void UseCrate(gentity_t* crate, gentity_t* player, gentity_t*)
{
	if (player->client->ps.weapon != WP_HANDS)
		return;
	player->client->ps.stats[ STAT_WEAPON ] = WP_CRATE;
	G_ForceWeaponChange( player, WP_CRATE );
	G_FreeEntity(crate);
}

static void UseInertCrate(gentity_t* crate, gentity_t* player, gentity_t*)
{
	if (player->client->ps.weapon != WP_HANDS)
		return;
	player->client->ps.stats[ STAT_WEAPON ] = WP_INERT_CRATE;
	G_ForceWeaponChange( player, WP_INERT_CRATE );
	G_FreeEntity(crate);
}

static void GreenCrate_touch(gentity_t* self, gentity_t* other)
{
	if (!other->client) return;

	self->entity->Get<MissileComponent>()->Explode();
}

BigPlatformComponent::BigPlatformComponent(Entity& entity, HumanBuildableComponent& r_HumanBuildableComponent)
	: BigPlatformComponentBase(entity, r_HumanBuildableComponent)
{
	REGISTER_THINKER(AddCrates, ThinkingComponent::SCHEDULER_AVERAGE, crateInterval.Get());
	REGISTER_THINKER(AddHealthItems, ThinkingComponent::SCHEDULER_AVERAGE, healthItemInterval.Get());
	entity.oldEnt->flags |= FL_GODMODE;
}

static gentity_t *SpawnDumbMissileRestingCrate( const glm::vec3 &start, const glm::vec3 &dir )
{
	gentity_t *m = G_NewEntity( HAS_CBSE );
	RestingCrateEntity::Params params;
	params.oldEnt = m;
	params.Health_maxHealth = 5;
	params.Missile_attributes = BG_Missile( MIS_CRATE );
	m->entity = new RestingCrateEntity{ params };
	G_SetUpMissile( m, &g_entities[ENTITYNUM_NONE], GLM4READ( start ), GLM4READ( dir ) );
	return m;
}

static gentity_t *SpawnDumbMissileGreenCrate( const glm::vec3 &start, const glm::vec3 &dir )
{
	gentity_t *m = G_NewEntity( HAS_CBSE );
	GreenCrateEntity::Params params;
	params.oldEnt = m;
	params.Health_maxHealth = 5;
	params.Missile_attributes = BG_Missile( MIS_GREEN_CRATE );
	m->entity = new GreenCrateEntity{ params };
	G_SetUpMissile( m, &g_entities[ENTITYNUM_NONE], GLM4READ( start ), GLM4READ( dir ) );
	return m;
}

static gentity_t *SpawnDumbMissileInertCrate( const glm::vec3 &start, const glm::vec3 &dir )
{
	gentity_t *m = G_NewEntity( HAS_CBSE );
	InertCrateEntity::Params params;
	params.oldEnt = m;
	params.Health_maxHealth = 15;
	params.Missile_attributes = BG_Missile( MIS_INERT_CRATE );
	m->entity = new InertCrateEntity{ params };
	G_SetUpMissile( m, &g_entities[ENTITYNUM_NONE], GLM4READ( start ), GLM4READ( dir ) );
	return m;
}

void BigPlatformComponent::Bounds(vec3_t mins, vec3_t maxs)
{
	BG_BuildableBoundingBox(BA_H_BIGPLATFORM, mins, maxs);
	VectorAdd(mins, entity.oldEnt->s.origin, mins);
	VectorAdd(maxs, entity.oldEnt->s.origin, maxs);
}

static int seed;

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

	gentity_t* crate;
	glm::vec3 velocity{0, 0, -0.01};
	float crateTypeChoice = Q_random(&seed);
	if (crateTypeChoice < 0.2) {
		crate = SpawnDumbMissileGreenCrate(location, velocity);
		crate->touch = GreenCrate_touch;
	} else if (crateTypeChoice < 0.4) {
		crate = SpawnDumbMissileRestingCrate(location, velocity);
		crate->use = UseCrate;
	} else {
		crate = SpawnDumbMissileInertCrate(location, velocity);
		crate->use = UseInertCrate;
	}
	// EF_NO_BOUNCE_SOUND to distinguish crate sitting on ground from a thrown one
	crate->s.eFlags |= EF_NO_BOUNCE_SOUND | EF_BOUNCE_HALF;
}

static void TryAddHealthItem(const vec3_t location)
{
	trace_t trace;
	vec3_t mins{-5, -5, -5};
	vec3_t maxs{5, 5, 5};
	trap_Trace(&trace, location, mins, maxs, location, ENTITYNUM_NONE, MASK_ALL, 0);
	if (trace.fraction != 1.0f)
		return;

	gentity_t* item = G_NewEntity(HAS_CBSE);
	item->classname = BG_strdup("healthItem");
	item->s.eType = entityType_t::ET_FIRE;

	HealthItemEntity::Params params;
	params.oldEnt = item;
	item->entity = new HealthItemEntity(params);

	VectorCopy(mins, item->r.mins);
	VectorCopy(maxs, item->r.maxs);
	G_SetOrigin(item, VEC2GLM(location));
	trap_LinkEntity(item);
}

void BigPlatformComponent::AddCrates(int timeDelta)
{
	vec3_t unused, platformMins, platformMaxs, crateMins;
	Bounds(platformMins, platformMaxs);
	float xlerp = 0.1 + random() * 0.8;
	float ylerp = 0.1 + random() * 0.8;
	BG_MissileBounds(BG_Missile(MIS_CRATE), crateMins, unused);
	glm::vec3 location{ platformMins[0] + xlerp * (platformMaxs[0] - platformMins[0]),
	                 platformMins[1] + ylerp * (platformMaxs[1] - platformMins[1]),
	                 platformMaxs[2] - crateMins[2] + 3 };
	TryAddCrate(location);
}

void BigPlatformComponent::AddHealthItems(int)
{
	vec3_t platformMins, platformMaxs;
	Bounds(platformMins, platformMaxs);
	float xlerp = 0.1 + random() * 0.8;
	float ylerp = 0.1 + random() * 0.8;
	float itemZmin = -5;
	vec3_t location{ platformMins[0] + xlerp * (platformMaxs[0] - platformMins[0]),
					 platformMins[1] + ylerp * (platformMaxs[1] - platformMins[1]),
					 platformMaxs[2] - itemZmin + 4.0f };
	TryAddHealthItem(location);
}

void BigPlatformComponent::Place(gentity_t* player, int position)
{
	vec3_t hMins;
	BG_ClassBoundingBox(PCL_HUMAN_NAKED, hMins, nullptr, nullptr, nullptr, nullptr);
	vec3_t platformMins, platformMaxs;
	Bounds(platformMins, platformMaxs);
	float xlerp = position & 1 ? 0.95 : 0.05;
	float ylerp = position & 2 ? 0.95 : 0.05;
	glm::vec3 location{ platformMins[0] + xlerp * (platformMaxs[0] - platformMins[0]),
	                 platformMins[1] + ylerp * (platformMaxs[1] - platformMins[1]),
	                 platformMaxs[2] - hMins[2] + 3 };
	glm::vec3 angles{}; //TODO
	G_TeleportPlayer(player, location, angles, 0.0f);
}

void BigPlatformComponent::ImmobilizePlayers(int)
{
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (players_[i])
			Place(players_[i].entity, i);
	}
	if (countdown_ < 0) {
		GetThinkingComponent().UnregisterActiveThinker();
		REGISTER_THINKER(CheckWinner, ThinkingComponent::SCHEDULER_BEFORE, 1);
	}
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

void BigPlatformComponent::CheckWinner(int)
{
	gentity_t* winner = nullptr;
	for (GentityRef player : players_) {
		if (player) {
			if (winner)
				return;
			else
				winner = player.get();
		}
	}

	playing_ = false;
	std::string message = winner ? Str::Format("%s ^*wins", winner->client->pers.netname) : "Tie";
	trap_SendServerCommand(-1, va("cp %s 33", Quote(message.c_str())));
	GetThinkingComponent().UnregisterActiveThinker();
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

void Cmd_Devteam_f(gentity_t* ent);
void BigPlatformComponent::StartRound(const std::vector<gentity_t*>& players)
{
	if (playing_) {
		Log::Warn("already playing");
		return;
	}
	playing_ = true;
	CleanUp();
	ASSERT_LE(int(players.size()), MAX_PLAYERS);
	for (size_t i = 0; i < players.size(); i++) {
		Place(players[i], i);
		Cmd::PushArgs("devteam h");
		Cmd_Devteam_f(players[i]);
		Cmd::PopArgs();
		players[i]->flags |= FL_GODMODE;
		players_[i] = players[i];
	}
	REGISTER_THINKER(ImmobilizePlayers, ThinkingComponent::SCHEDULER_BEFORE, 1);
	countdown_ = 3;
	Countdown(0);
	REGISTER_THINKER(Countdown, ThinkingComponent::SCHEDULER_AVERAGE, 1000);
}

// using spawn queue
void BigPlatformComponent::StartRound()
{
	if (playing_) {
		Log::Warn("already playing");
		return;
	}

	std::vector<gentity_t*> players;
	while (players.size() < MAX_PLAYERS) {
		int next = G_PopSpawnQueue( &level.team[ TEAM_HUMANS ].spawnQueue );
		if (next < 0) break;
		players.push_back(g_entities + next);
	}
	StartRound(players);
}
