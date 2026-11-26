#include "stamina/mounted-combat.h"

#include <atomic>
#include <string_view>

#include "logging.h"
#include "stamina/costs.h"

namespace ExhaustingCombat::Stamina::MountedCombat
{
namespace
{
constexpr std::string_view kRightSwingEvent = "weaponSwing";
constexpr std::string_view kLeftSwingEvent  = "weaponLeftSwing";

class SwingEventSink final : public RE::BSTEventSink<RE::BSAnimationGraphEvent>
{
public:
    RE::BSEventNotifyControl ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>*) override
    {
        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!a_event || !player || a_event->holder != player || a_event->tag.empty()) {
            return RE::BSEventNotifyControl::kContinue;
        }
        const std::string_view tag(a_event->tag.c_str(), a_event->tag.length());
        if (tag == kRightSwingEvent || tag == kLeftSwingEvent) {
            ChargeMountedSwing(*player, tag);
        }
        return RE::BSEventNotifyControl::kContinue;
    }
};

SwingEventSink g_swingSink;
bool           g_listening = false;  // only touched from the player update (main thread)
}  // namespace

void UpdatePlayer(RE::PlayerCharacter& a_player)
{
    const bool mounted = a_player.IsOnMount();
    if (mounted && !g_listening) {
        a_player.RemoveAnimationGraphEventSink(&g_swingSink);  // never register twice
        g_listening = a_player.AddAnimationGraphEventSink(&g_swingSink);
        logs::debug("[attack-mounted] {} mounted: {}", Describe(&a_player),
                    g_listening ? "listening for swing events" : "animation graph not ready, retrying next frame");
    } else if (!mounted && g_listening) {
        a_player.RemoveAnimationGraphEventSink(&g_swingSink);
        g_listening = false;
        logs::debug("[attack-mounted] {} dismounted: stopped listening for swing events", Describe(&a_player));
    }
}
}  // namespace ExhaustingCombat::Stamina::MountedCombat
