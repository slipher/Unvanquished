set(DETOURLIST
    ${LIB_DIR}/recastnavigation/DebugUtils/Source/DebugDraw.cpp
    ${LIB_DIR}/recastnavigation/Detour/Source/DetourAlloc.cpp
    ${LIB_DIR}/recastnavigation/Detour/Include/DetourAssert.h
    ${LIB_DIR}/recastnavigation/Detour/Source/DetourAssert.cpp
    ${LIB_DIR}/recastnavigation/Detour/Source/DetourCommon.cpp
    ${LIB_DIR}/recastnavigation/DebugUtils/Source/DetourDebugDraw.cpp
    ${LIB_DIR}/recastnavigation/Detour/Source/DetourNavMesh.cpp
    ${LIB_DIR}/recastnavigation/Detour/Source/DetourNavMeshBuilder.cpp
    ${LIB_DIR}/recastnavigation/Detour/Source/DetourNavMeshQuery.cpp
    ${LIB_DIR}/recastnavigation/Detour/Include/DetourMath.h
    ${LIB_DIR}/recastnavigation/Detour/Source/DetourNode.cpp
    ${LIB_DIR}/recastnavigation/DetourCrowd/Source/DetourPathCorridor.cpp
    ${LIB_DIR}/recastnavigation/Detour/Include/DetourStatus.h
    ${LIB_DIR}/recastnavigation/DetourTileCache/Source/DetourTileCache.cpp
    ${LIB_DIR}/recastnavigation/DetourTileCache/Source/DetourTileCacheBuilder.cpp
)

# Note that this includes some "demo" code not part of the library proper
set(RECASTLIST
    ${LIB_DIR}/recastnavigation/Recast/Source/RecastAlloc.cpp
    ${LIB_DIR}/recastnavigation/Recast/Include/RecastAlloc.h
    ${LIB_DIR}/recastnavigation/Recast/Source/RecastArea.cpp
    ${LIB_DIR}/recastnavigation/Recast/Source/RecastAssert.cpp
    ${LIB_DIR}/recastnavigation/Recast/Include/RecastAssert.h
    ${LIB_DIR}/recastnavigation/Recast/Source/RecastContour.cpp
    ${LIB_DIR}/recastnavigation/Recast/Source/Recast.cpp
    ${LIB_DIR}/recastnavigation/Recast/Include/Recast.h
    ${LIB_DIR}/recastnavigation/Recast/Source/RecastFilter.cpp
    ${LIB_DIR}/recastnavigation/Recast/Source/RecastLayers.cpp
    ${LIB_DIR}/recastnavigation/Recast/Source/RecastMesh.cpp
    ${LIB_DIR}/recastnavigation/Recast/Source/RecastMeshDetail.cpp
    ${LIB_DIR}/recastnavigation/Recast/Source/RecastRasterization.cpp
    ${LIB_DIR}/recastnavigation/Recast/Source/RecastRegion.cpp

    ${LIB_DIR}/recastnavigation/RecastDemo/Source/ChunkyTriMesh.cpp
    ${LIB_DIR}/recastnavigation/RecastDemo/Include/ChunkyTriMesh.h
)

set(FASTLZLIST
    ${LIB_DIR}/fastlz/fastlz.c
)

set(TINYGETTEXTLIST
    ${LIB_DIR}/tinygettext/include/tinygettext/dictionary.hpp
    ${LIB_DIR}/tinygettext/include/tinygettext/dictionary_manager.hpp
    ${LIB_DIR}/tinygettext/include/tinygettext/file_system.hpp
    ${LIB_DIR}/tinygettext/include/tinygettext/iconv.hpp
    ${LIB_DIR}/tinygettext/include/tinygettext/language.hpp
    ${LIB_DIR}/tinygettext/include/tinygettext/log.hpp
    ${LIB_DIR}/tinygettext/include/tinygettext/log_stream.hpp
    ${LIB_DIR}/tinygettext/include/tinygettext/plural_forms.hpp
    ${LIB_DIR}/tinygettext/include/tinygettext/po_parser.hpp
    ${LIB_DIR}/tinygettext/include/tinygettext/tinygettext.hpp
    ${LIB_DIR}/tinygettext/include/tinygettext/unix_file_system.hpp
    ${LIB_DIR}/tinygettext/src/dictionary.cpp
    ${LIB_DIR}/tinygettext/src/dictionary_manager.cpp
    ${LIB_DIR}/tinygettext/src/iconv.cpp
    ${LIB_DIR}/tinygettext/src/language.cpp
    ${LIB_DIR}/tinygettext/src/log.cpp
    ${LIB_DIR}/tinygettext/src/plural_forms.cpp
    ${LIB_DIR}/tinygettext/src/po_parser.cpp
)

set(GAMESHAREDLIST
    ${GAMELOGIC_DIR}/shared/bg_alloc.cpp
    ${GAMELOGIC_DIR}/shared/bg_gameplay.h
    ${GAMELOGIC_DIR}/shared/bg_gameplay.cpp
    ${GAMELOGIC_DIR}/shared/bg_local.h
    ${GAMELOGIC_DIR}/shared/bg_misc.cpp
    ${GAMELOGIC_DIR}/shared/bg_parse.cpp
    ${GAMELOGIC_DIR}/shared/bg_pmove.cpp
    ${GAMELOGIC_DIR}/shared/bg_public.h
    ${GAMELOGIC_DIR}/shared/bg_teamprogress.cpp
    ${GAMELOGIC_DIR}/shared/bg_utilities.cpp
    ${GAMELOGIC_DIR}/shared/bg_voice.cpp
    ${GAMELOGIC_DIR}/shared/bot_nav_shared.cpp
    ${GAMELOGIC_DIR}/shared/bot_nav_shared.h
    ${GAMELOGIC_DIR}/shared/parse.cpp
    ${GAMELOGIC_DIR}/shared/parse.h
    ${GAMELOGIC_DIR}/shared/Clustering.h

    ${GAMELOGIC_DIR}/shared/navgen/brush.cpp
    ${GAMELOGIC_DIR}/shared/navgen/nav.cpp
    ${GAMELOGIC_DIR}/shared/navgen/navgen.h
)

set(CGAMELIST
    ${GAMELOGIC_DIR}/cgame/cg_animation.cpp
    ${GAMELOGIC_DIR}/cgame/cg_animdelta.cpp
    ${GAMELOGIC_DIR}/cgame/cg_animdelta.h
    ${GAMELOGIC_DIR}/cgame/cg_animmapobj.cpp
    ${GAMELOGIC_DIR}/cgame/cg_api.cpp
    ${GAMELOGIC_DIR}/cgame/cg_attachment.cpp
    ${GAMELOGIC_DIR}/cgame/cg_beacon.cpp
    ${GAMELOGIC_DIR}/cgame/cg_buildable.cpp
    ${GAMELOGIC_DIR}/cgame/cg_consolecmds.cpp
    ${GAMELOGIC_DIR}/cgame/cg_draw.cpp
    ${GAMELOGIC_DIR}/cgame/cg_drawtools.cpp
    ${GAMELOGIC_DIR}/cgame/cg_ents.cpp
    ${GAMELOGIC_DIR}/cgame/cg_event.cpp
    ${GAMELOGIC_DIR}/cgame/cg_gameinfo.cpp
    ${GAMELOGIC_DIR}/cgame/cg_key_name.cpp
    ${GAMELOGIC_DIR}/cgame/cg_key_name.h
    ${GAMELOGIC_DIR}/cgame/cg_local.h
    ${GAMELOGIC_DIR}/cgame/cg_main.cpp
    ${GAMELOGIC_DIR}/cgame/cg_marks.cpp
    ${GAMELOGIC_DIR}/cgame/cg_minimap.cpp
    ${GAMELOGIC_DIR}/cgame/cg_parseutils.cpp
    ${GAMELOGIC_DIR}/cgame/cg_particles.cpp
    ${GAMELOGIC_DIR}/cgame/cg_players.cpp
    ${GAMELOGIC_DIR}/cgame/cg_playerstate.cpp
    ${GAMELOGIC_DIR}/cgame/cg_predict.cpp
    ${GAMELOGIC_DIR}/cgame/cg_rocket.cpp
    ${GAMELOGIC_DIR}/cgame/cg_rocket_dataformatter.cpp
    ${GAMELOGIC_DIR}/cgame/cg_rocket_datasource.cpp
    ${GAMELOGIC_DIR}/cgame/cg_rocket_draw.cpp
    ${GAMELOGIC_DIR}/cgame/cg_rocket_events.cpp
    ${GAMELOGIC_DIR}/cgame/cg_rocket_progressbar.cpp
    ${GAMELOGIC_DIR}/cgame/cg_segmented_skeleton.cpp
    ${GAMELOGIC_DIR}/cgame/cg_segmented_skeleton.h
    ${GAMELOGIC_DIR}/cgame/cg_servercmds.cpp
    ${GAMELOGIC_DIR}/cgame/cg_skeleton_modifier.h
    ${GAMELOGIC_DIR}/cgame/cg_snapshot.cpp
    ${GAMELOGIC_DIR}/cgame/cg_trails.cpp
    ${GAMELOGIC_DIR}/cgame/cg_tutorial.cpp
    ${GAMELOGIC_DIR}/cgame/cg_ui.h
    ${GAMELOGIC_DIR}/cgame/cg_utils.cpp
    ${GAMELOGIC_DIR}/cgame/cg_view.cpp
    ${GAMELOGIC_DIR}/cgame/cg_weapons.cpp
    ${GAMELOGIC_DIR}/cgame/translation.cpp
    ${GAMELOGIC_DIR}/cgame/Filter.h
    ${GAMELOGIC_DIR}/cgame/CombatFeedback.cpp

    ${GAMELOGIC_DIR}/cgame/rocket/rocket.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/rocket.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketChatField.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketColorInput.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketConditionalElement.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketConsoleTextElement.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketCvarInlineElement.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketDataFormatter.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketDataGrid.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketDataSelect.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketDataSource.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketDataSourceSingle.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketElement.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketElementDocument.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketEventInstancer.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketFocusManager.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketFormControlInput.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketFormControlSelect.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketIncludeElement.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketKeyBinder.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketProgressBar.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocketSelectableDataGrid.h
    ${GAMELOGIC_DIR}/cgame/rocket/rocket_dataformatter.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/rocket_datagrid.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/rocket_documents.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/rocket_element.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/rocket_events.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/rocket_hud.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/rocket_keys.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/lua/CDataSource.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/lua/CDataSource.h
    ${GAMELOGIC_DIR}/cgame/rocket/lua/Cmd.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/lua/Cmd.h
    ${GAMELOGIC_DIR}/cgame/rocket/lua/Cvar.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/lua/Cvar.h
    ${GAMELOGIC_DIR}/cgame/rocket/lua/Events.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/lua/Events.h
    ${GAMELOGIC_DIR}/cgame/rocket/lua/Timer.cpp
    ${GAMELOGIC_DIR}/cgame/rocket/lua/Timer.h

    ${ENGINE_DIR}/client/cg_api.h
    ${ENGINE_DIR}/client/cg_msgdef.h

    ${GAMESHAREDLIST}
)

set(SGAMELIST
    ${GAMELOGIC_DIR}/sgame/Beacon.cpp
    ${GAMELOGIC_DIR}/sgame/BaseClustering.cpp
    ${GAMELOGIC_DIR}/sgame/Entities.cpp
    ${GAMELOGIC_DIR}/sgame/Entities.h
    ${GAMELOGIC_DIR}/sgame/sg_active.cpp
    ${GAMELOGIC_DIR}/sgame/sg_admin.cpp
    ${GAMELOGIC_DIR}/sgame/sg_admin.h
    ${GAMELOGIC_DIR}/sgame/sg_api.cpp
    ${GAMELOGIC_DIR}/sgame/sg_bot_ai.cpp
    ${GAMELOGIC_DIR}/sgame/sg_bot_ai.h
    ${GAMELOGIC_DIR}/sgame/sg_bot.cpp
    ${GAMELOGIC_DIR}/sgame/sg_bot_local.h
    ${GAMELOGIC_DIR}/sgame/sg_bot_nav.cpp
    ${GAMELOGIC_DIR}/sgame/sg_bot_parse.cpp
    ${GAMELOGIC_DIR}/sgame/sg_bot_parse.h
    ${GAMELOGIC_DIR}/sgame/sg_bot_public.h
    ${GAMELOGIC_DIR}/sgame/sg_bot_skilltree.cpp
    ${GAMELOGIC_DIR}/sgame/sg_bot_util.cpp
    ${GAMELOGIC_DIR}/sgame/sg_bot_util.h
    ${GAMELOGIC_DIR}/sgame/sg_buildable.cpp
    ${GAMELOGIC_DIR}/sgame/sg_buildpoints.cpp
    ${GAMELOGIC_DIR}/sgame/sg_client.cpp
    ${GAMELOGIC_DIR}/sgame/sg_cmds.cpp
    ${GAMELOGIC_DIR}/sgame/sg_cm_world.cpp
    ${GAMELOGIC_DIR}/sgame/sg_cm_world.h
    ${GAMELOGIC_DIR}/sgame/sg_combat.cpp
    ${GAMELOGIC_DIR}/sgame/sg_definitions.h
    ${GAMELOGIC_DIR}/sgame/sg_entities.cpp
    ${GAMELOGIC_DIR}/sgame/sg_entities.h
    ${GAMELOGIC_DIR}/sgame/sg_extern.h
    ${GAMELOGIC_DIR}/sgame/sg_local.h
    ${GAMELOGIC_DIR}/sgame/sg_main.cpp
    ${GAMELOGIC_DIR}/sgame/sg_maprotation.cpp
    ${GAMELOGIC_DIR}/sgame/sg_missile.cpp
    ${GAMELOGIC_DIR}/sgame/sg_momentum.cpp
    ${GAMELOGIC_DIR}/sgame/sg_namelog.cpp
    ${GAMELOGIC_DIR}/sgame/sg_physics.cpp
    ${GAMELOGIC_DIR}/sgame/sg_public.h
    ${GAMELOGIC_DIR}/sgame/sg_session.cpp
    ${GAMELOGIC_DIR}/sgame/sg_spawn.cpp
    ${GAMELOGIC_DIR}/sgame/sg_spawn.h
    ${GAMELOGIC_DIR}/sgame/sg_spawn_afx.cpp
    ${GAMELOGIC_DIR}/sgame/sg_spawn_ctrl.cpp
    ${GAMELOGIC_DIR}/sgame/sg_spawn_fx.cpp
    ${GAMELOGIC_DIR}/sgame/sg_spawn_game.cpp
    ${GAMELOGIC_DIR}/sgame/sg_spawn_generic.cpp
    ${GAMELOGIC_DIR}/sgame/sg_spawn_gfx.cpp
    ${GAMELOGIC_DIR}/sgame/sg_spawn_mover.cpp
    ${GAMELOGIC_DIR}/sgame/sg_spawn_position.cpp
    ${GAMELOGIC_DIR}/sgame/sg_spawn_sensor.cpp
    ${GAMELOGIC_DIR}/sgame/sg_spawn_shared.cpp
    ${GAMELOGIC_DIR}/sgame/sg_struct.h
    ${GAMELOGIC_DIR}/sgame/sg_svcmds.cpp
    ${GAMELOGIC_DIR}/sgame/sg_team.cpp
    ${GAMELOGIC_DIR}/sgame/sg_trapcalls.h
    ${GAMELOGIC_DIR}/sgame/sg_utils.cpp
    ${GAMELOGIC_DIR}/sgame/sg_votes.h
    ${GAMELOGIC_DIR}/sgame/sg_votes.cpp
    ${GAMELOGIC_DIR}/sgame/sg_weapon.cpp
    ${GAMELOGIC_DIR}/sgame/CBSE.h
    ${GAMELOGIC_DIR}/sgame/CombatFeedback.cpp
    ${GAMELOGIC_DIR}/sgame/CustomSurfaceFlags.h

    ${GAMELOGIC_DIR}/sgame/botlib/bot_api.h
    ${GAMELOGIC_DIR}/sgame/botlib/bot_debug.cpp
    ${GAMELOGIC_DIR}/sgame/botlib/bot_convert.cpp
    ${GAMELOGIC_DIR}/sgame/botlib/bot_convert.h
    ${GAMELOGIC_DIR}/sgame/botlib/bot_load.cpp
    ${GAMELOGIC_DIR}/sgame/botlib/bot_local.cpp
    ${GAMELOGIC_DIR}/sgame/botlib/bot_local.h
    ${GAMELOGIC_DIR}/sgame/botlib/bot_nav.cpp
    ${GAMELOGIC_DIR}/sgame/botlib/bot_nav_edit.cpp
    ${GAMELOGIC_DIR}/sgame/botlib/bot_navdraw.h
    ${GAMELOGIC_DIR}/sgame/botlib/bot_types.h

    ${GAMELOGIC_DIR}/sgame/components/AcidTubeComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/AcidTubeComponent.h
    ${GAMELOGIC_DIR}/sgame/components/AlienBuildableComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/AlienBuildableComponent.h
    ${GAMELOGIC_DIR}/sgame/components/AlienClassComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/AlienClassComponent.h
    ${GAMELOGIC_DIR}/sgame/components/ArmorComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/ArmorComponent.h
    ${GAMELOGIC_DIR}/sgame/components/ArmouryComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/ArmouryComponent.h
    ${GAMELOGIC_DIR}/sgame/components/BarricadeComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/BarricadeComponent.h
    ${GAMELOGIC_DIR}/sgame/components/BoosterComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/BoosterComponent.h
    ${GAMELOGIC_DIR}/sgame/components/BuildableComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/BuildableComponent.h
    ${GAMELOGIC_DIR}/sgame/components/ClientComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/ClientComponent.h
    ${GAMELOGIC_DIR}/sgame/components/DeferredFreeingComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/DeferredFreeingComponent.h
    ${GAMELOGIC_DIR}/sgame/components/DrillComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/DrillComponent.h
    ${GAMELOGIC_DIR}/sgame/components/EggComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/EggComponent.h
    ${GAMELOGIC_DIR}/sgame/components/HealthComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/HealthComponent.h
    ${GAMELOGIC_DIR}/sgame/components/HiveComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/HiveComponent.h
    ${GAMELOGIC_DIR}/sgame/components/HumanBuildableComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/HumanBuildableComponent.h
    ${GAMELOGIC_DIR}/sgame/components/HumanClassComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/HumanClassComponent.h
    ${GAMELOGIC_DIR}/sgame/components/IgnitableComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/IgnitableComponent.h
    ${GAMELOGIC_DIR}/sgame/components/KnockbackComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/KnockbackComponent.h
    ${GAMELOGIC_DIR}/sgame/components/LeechComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/LeechComponent.h
    ${GAMELOGIC_DIR}/sgame/components/MainBuildableComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/MainBuildableComponent.h
    ${GAMELOGIC_DIR}/sgame/components/MedipadComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/MedipadComponent.h
    ${GAMELOGIC_DIR}/sgame/components/MGTurretComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/MGTurretComponent.h
    ${GAMELOGIC_DIR}/sgame/components/MiningComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/MiningComponent.h
    ${GAMELOGIC_DIR}/sgame/components/MissileComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/MissileComponent.h
    ${GAMELOGIC_DIR}/sgame/components/OvermindComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/OvermindComponent.h
    ${GAMELOGIC_DIR}/sgame/components/ReactorComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/ReactorComponent.h
    ${GAMELOGIC_DIR}/sgame/components/RocketpodComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/RocketpodComponent.h
    ${GAMELOGIC_DIR}/sgame/components/SpawnerComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/SpawnerComponent.h
    ${GAMELOGIC_DIR}/sgame/components/SpectatorComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/SpectatorComponent.h
    ${GAMELOGIC_DIR}/sgame/components/SpikerComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/SpikerComponent.h
    ${GAMELOGIC_DIR}/sgame/components/TeamComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/TeamComponent.h
    ${GAMELOGIC_DIR}/sgame/components/TelenodeComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/TelenodeComponent.h
    ${GAMELOGIC_DIR}/sgame/components/ThinkingComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/ThinkingComponent.h
    ${GAMELOGIC_DIR}/sgame/components/TrapperComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/TrapperComponent.h
    ${GAMELOGIC_DIR}/sgame/components/TurretComponent.cpp
    ${GAMELOGIC_DIR}/sgame/components/TurretComponent.h

    ${ENGINE_DIR}/server/sg_api.h
    ${ENGINE_DIR}/server/sg_msgdef.h

    ${GAMESHAREDLIST}
)
