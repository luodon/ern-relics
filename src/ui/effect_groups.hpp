#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <string>
#include <string_view>

namespace ern {
// Presentation categories only. Roll-table compatibility still belongs to the validator.
enum class EffectGroup {
    Attributes,
    Attack,
    Recovery,
    Defense,
    Discovery,
    Skills,
    Character,
    Other
};
inline constexpr std::array effect_groups{
    EffectGroup::Attributes, EffectGroup::Attack, EffectGroup::Recovery,  EffectGroup::Defense,
    EffectGroup::Discovery,  EffectGroup::Skills, EffectGroup::Character, EffectGroup::Other};

inline const char *effect_group_name(EffectGroup group) {
    switch (group) {
    case EffectGroup::Attributes:
        return "属性";
    case EffectGroup::Attack:
        return "攻击";
    case EffectGroup::Recovery:
        return "恢复";
    case EffectGroup::Defense:
        return "防御";
    case EffectGroup::Discovery:
        return "探索";
    case EffectGroup::Skills:
        return "战技与法术";
    case EffectGroup::Character:
        return "角色";
    case EffectGroup::Other:
        return "其他";
    }
    return "其他";
}

inline EffectGroup effect_group(std::string_view description) {
    if (description.starts_with('[') || description.starts_with("【"))
        return EffectGroup::Character;
    std::string text(description);
    std::transform(text.begin(), text.end(), text.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    const auto has = [&text](std::string_view word) {
        return text.find(word) != std::string::npos;
    };
    for (const auto stat :
         {"生命力+", "集中力+", "耐力+", "力气+", "灵巧+", "智力+", "信仰+", "感应+", "强韧度+"})
        if (text.starts_with(stat))
            return EffectGroup::Attributes;
    if (has("潜在能力") || has("观察力") || has("宝藏") || has("获得的卢恩"))
        return EffectGroup::Discovery;
    if (has("技艺") || has("绝招"))
        return EffectGroup::Character;
    if (has("魔法") || has("祷告") || has("战技") || has("法术"))
        return EffectGroup::Skills;
    if (has("恢复") || has("回复") || has("圣杯瓶"))
        return EffectGroup::Recovery;
    if (has("血量上限") || has("专注值上限") || has("精力上限") || has("减伤率") || has("抵抗力") ||
        has("防御") || has("强韧度"))
        return EffectGroup::Defense;
    if (has("攻击") || has("伤害") || has("致命一击") || has("异常状态") || has("出血") ||
        has("冻伤") || has("中毒") || has("量表") || has("损伤"))
        return EffectGroup::Attack;
    return EffectGroup::Other;
}
} // namespace ern
