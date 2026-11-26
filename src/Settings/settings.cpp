#include "settings.h"
#include "../config.h"

namespace SETTINGS
{
    inline BoolSetting combat_only_npc                {eSliderType::BOOL,  "fNPC_combat_only",                EXCO::CONFIG::combat_only_npc,               "combat_only_npc",               "Desc", true, true};
    inline BoolSetting equipped_weight_only_npc       {eSliderType::BOOL,  "fNPC_equipped_weight_only",       EXCO::CONFIG::equipped_weight_only_npc,      "equipped_weight_only_npc",      "Desc", true, false};
    inline BoolSetting exclude_no_stam_npc            {eSliderType::BOOL,  "fNPC_exclude_no_stam",            EXCO::CONFIG::exclude_no_stam_npc,           "exclude_no_stam_npc",           "Desc", true, true};
    inline FloatSetting stamina_cost_attack_npc       {eSliderType::FLOAT, "fNPC_stamina_cost_attack",        EXCO::CONFIG::stamina_cost_attack_npc,       "stamina_cost_attack_npc",       "Desc", 9.0f, 0.0f, 50.0f, 9.0f };
    inline FloatSetting stamina_cost_bow_drain_npc    {eSliderType::FLOAT, "fNPC_stamina_cost_bow_drain",     EXCO::CONFIG::stamina_cost_bow_drain_npc,    "stamina_cost_bow_drain_npc",    "Desc", 5.0f, 0.0f, 50.0f, 5.0f };
    inline FloatSetting stamina_cost_bow_npc          {eSliderType::FLOAT, "fNPC_stamina_cost_bow",           EXCO::CONFIG::stamina_cost_bow_npc,          "stamina_cost_bow_npc",          "Desc", 10.0f, 0.0f, 50.0f, 10.0f };
    inline FloatSetting stamina_cost_casting_npc      {eSliderType::FLOAT, "fNPC_stamina_cost_casting",       EXCO::CONFIG::stamina_cost_casting_npc,      "stamina_cost_casting_npc",      "Desc", 10.0f, 0.0f, 50.0f, 10.0f };
    inline FloatSetting stamina_cost_jump_npc         {eSliderType::FLOAT, "fNPC_stamina_cost_jump",          EXCO::CONFIG::stamina_cost_jump_npc,         "stamina_cost_jump_npc",         "Desc", 10.0f, 0.0f, 50.0f, 10.0f };
    inline FloatSetting stamina_cost_running_npc      {eSliderType::FLOAT, "fNPC_stamina_cost_running",       EXCO::CONFIG::stamina_cost_running_npc,      "stamina_cost_running_npc",      "Desc", 1.0f, 0.0f, 50.0f, 1.0f };
    inline FloatSetting stamina_cost_sneak_npc        {eSliderType::FLOAT, "fNPC_stamina_cost_sneak",         EXCO::CONFIG::stamina_cost_sneak_npc,        "stamina_cost_sneak_npc",        "Desc", 2.0f, 0.0f, 50.0f, 2.0f };
    inline FloatSetting stamina_cost_sprint_horse_npc {eSliderType::FLOAT, "fNPC_stamina_cost_sprint_horse",  EXCO::CONFIG::stamina_cost_sprint_horse_npc, "stamina_cost_sprint_horse_npc", "Desc", 7.0f, 0.0f, 50.0f, 7.0f };
    inline FloatSetting stamina_cost_sprint_npc       {eSliderType::FLOAT, "fNPC_stamina_cost_sprint",        EXCO::CONFIG::stamina_cost_sprint_npc,       "stamina_cost_sprint_npc",       "Desc", 7.0f, 0.0f, 50.0f, 7.0f };
    inline FloatSetting stamina_cost_swim_npc         {eSliderType::FLOAT, "fNPC_stamina_cost_swim",          EXCO::CONFIG::stamina_cost_swim_npc,         "stamina_cost_swim_npc",         "Desc", 5.0f, 0.0f, 50.0f, 5.0f };
    inline FloatSetting stamina_mult_attack_npc       {eSliderType::FLOAT, "fNPC_stamina_mult_attack",        EXCO::CONFIG::stamina_mult_attack_npc,       "stamina_mult_attack_npc",       "Desc", 1.414f, 0.0f, 50.0f, 1.414f };
    inline FloatSetting stamina_mult_pattack_npc      {eSliderType::FLOAT, "fNPC_stamina_mult_pattack",       EXCO::CONFIG::stamina_mult_pattack_npc,      "stamina_mult_pattack_npc",      "Desc", 2.0f, 0.0f, 50.0f, 2.0f };

    inline BoolSetting combat_only_player                {eSliderType::BOOL,  "fPlayer_combat_only",               EXCO::CONFIG::combat_only_player,               "combat_only_player",               "Desc", true, true };
    inline BoolSetting equipped_weight_only_player       {eSliderType::BOOL,  "fPlayer_equipped_weight_only",      EXCO::CONFIG::equipped_weight_only_player,      "equipped_weight_only_player",      "Desc", true, true };
    inline FloatSetting stamina_cost_attack_player       {eSliderType::FLOAT, "fPlayer_stamina_cost_attack",       EXCO::CONFIG::stamina_cost_attack_player,       "stamina_cost_attack_player",       "Desc", 9.0f, 0.0f, 50.0f, 9.0f };
    inline FloatSetting stamina_cost_bow_drain_player    {eSliderType::FLOAT, "fPlayer_stamina_cost_bow_drain",    EXCO::CONFIG::stamina_cost_bow_drain_player,    "stamina_cost_bow_drain_player",    "Desc", 5.0f, 0.0f, 50.0f, 5.0f };
    inline FloatSetting stamina_cost_bow_player          {eSliderType::FLOAT, "fPlayer_stamina_cost_bow",          EXCO::CONFIG::stamina_cost_bow_player,          "stamina_cost_bow_player",          "Desc", 10.0f, 0.0f, 50.0f, 10.0f };
    inline FloatSetting stamina_cost_casting_player      {eSliderType::FLOAT, "fPlayer_stamina_cost_casting",      EXCO::CONFIG::stamina_cost_casting_player,      "stamina_cost_casting_player",      "Desc", 10.0f, 0.0f, 50.0f, 10.0f };
    inline FloatSetting stamina_cost_jump_player         {eSliderType::FLOAT, "fPlayer_stamina_cost_jump",         EXCO::CONFIG::stamina_cost_jump_player,         "stamina_cost_jump_player",         "Desc", 10.0f, 0.0f, 50.0f, 10.0f };
    inline FloatSetting stamina_cost_running_player      {eSliderType::FLOAT, "fPlayer_stamina_cost_running",      EXCO::CONFIG::stamina_cost_running_player,      "stamina_cost_running_player",      "Desc", 1.0f, 0.0f, 50.0f, 1.0f };
    inline FloatSetting stamina_cost_sneak_player        {eSliderType::FLOAT, "fPlayer_stamina_cost_sneak",        EXCO::CONFIG::stamina_cost_sneak_player,        "stamina_cost_sneak_player",        "Desc", 2.0f, 0.0f, 50.0f, 2.0f };
    inline FloatSetting stamina_cost_sprint_horse_player {eSliderType::FLOAT, "fPlayer_stamina_cost_sprint_horse", EXCO::CONFIG::stamina_cost_sprint_horse_player, "stamina_cost_sprint_horse_player", "Desc", 7.0f, 0.0f, 50.0f, 7.0f };
    inline FloatSetting stamina_cost_sprint_player       {eSliderType::FLOAT, "fPlayer_stamina_cost_sprint",       EXCO::CONFIG::stamina_cost_sprint_player,       "stamina_cost_sprint_player",       "Desc", 7.0f, 0.0f, 50.0f, 7.0f };
    inline FloatSetting stamina_cost_swim_player         {eSliderType::FLOAT, "fPlayer_stamina_cost_swim",         EXCO::CONFIG::stamina_cost_swim_player,         "stamina_cost_swim_player",         "Desc", 5.0f, 0.0f, 50.0f, 5.0f };
    inline FloatSetting stamina_mult_attack_player       {eSliderType::FLOAT, "fPlayer_stamina_mult_attack",       EXCO::CONFIG::stamina_mult_attack_player,       "stamina_mult_attack_player",       "Desc", 1.414f, 0.0f, 50.0f, 1.414f };
    inline FloatSetting stamina_mult_pattack_player      {eSliderType::FLOAT, "fPlayer_stamina_mult_pattack",      EXCO::CONFIG::stamina_mult_pattack_player,      "stamina_mult_pattack_player",      "Desc", 2.0f, 0.0f, 50.0f, 2.0f };

    inline FloatSetting stamina_av_mult_onehanded        {eSliderType::FLOAT, "fAVMult_onehanded",       EXCO::CONFIG::stamina_av_mult_onehanded,       "stamina_av_mult_onehanded",       "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_twohanded        {eSliderType::FLOAT, "fAVMult_twohanded",       EXCO::CONFIG::stamina_av_mult_twohanded,       "stamina_av_mult_twohanded",       "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_archery          {eSliderType::FLOAT, "fAVMult_archery",         EXCO::CONFIG::stamina_av_mult_archery,         "stamina_av_mult_archery",         "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_block            {eSliderType::FLOAT, "fAVMult_block",           EXCO::CONFIG::stamina_av_mult_block,           "stamina_av_mult_block",           "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_heavyarmor       {eSliderType::FLOAT, "fAVMult_heavyarmor",      EXCO::CONFIG::stamina_av_mult_heavyarmor,      "stamina_av_mult_heavyarmor",      "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_lightarmor       {eSliderType::FLOAT, "fAVMult_lightarmor",      EXCO::CONFIG::stamina_av_mult_lightarmor,      "stamina_av_mult_lightarmor",      "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_sneak            {eSliderType::FLOAT, "fAVMult_sneak",           EXCO::CONFIG::stamina_av_mult_sneak,           "stamina_av_mult_sneak",           "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_alteration       {eSliderType::FLOAT, "fAVMult_alteration",      EXCO::CONFIG::stamina_av_mult_alteration,      "stamina_av_mult_alteration",      "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_illusion         {eSliderType::FLOAT, "fAVMult_illusion",        EXCO::CONFIG::stamina_av_mult_illusion,        "stamina_av_mult_illusion",        "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_destruction      {eSliderType::FLOAT, "fAVMult_destruction",     EXCO::CONFIG::stamina_av_mult_destruction,     "stamina_av_mult_destruction",     "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_conjuration      {eSliderType::FLOAT, "fAVMult_conjuration",     EXCO::CONFIG::stamina_av_mult_conjuration,     "stamina_av_mult_conjuration",     "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_restoration      {eSliderType::FLOAT, "fAVMult_restoration",     EXCO::CONFIG::stamina_av_mult_restoration,     "stamina_av_mult_restoration",     "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_speedmult        {eSliderType::FLOAT, "fAVMult_speedmult",       EXCO::CONFIG::stamina_av_mult_speedmult,       "stamina_av_mult_speedmult",       "Desc", 0.0f, 0.0f, 1.0f, 0.0f };
    inline FloatSetting stamina_av_mult_inventoryweight  {eSliderType::FLOAT, "fAVMult_inventoryweight", EXCO::CONFIG::stamina_av_mult_inventoryweight, "stamina_av_mult_inventoryweight", "Desc", 0.0f, 0.0f, 1.0f, 0.0f };

}
