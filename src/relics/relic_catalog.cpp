#include "relics/relic_catalog.hpp"

namespace ern {

const RelicInfo *RelicCatalog::find(std::uint32_t game_item_id) const {
    // CSGaitem +0x0C contains the category-tagged EquipParamAntique row ID.
    if ((game_item_id & 0xF0000000) != 0x80000000)
        return nullptr;
    const auto found = entries_.find(game_item_id & 0x0FFFFFFF);
    return found == entries_.end() ? nullptr : &found->second;
}

const char *relic_lock_reason(const RelicInfo *info) {
    if (!info)
        return "无法识别该遗物，无法编辑。";
    switch (info->kind) {
    case RelicKind::Rolled:
        return nullptr;
    case RelicKind::Unique:
        return "独特遗物的词条固定，无法修改。";
    case RelicKind::Fixed:
        return "该遗物的词条固定，无法修改。";
    }
    return "无法识别该遗物，无法编辑。";
}

const char *relic_kind_name(const RelicInfo *info) {
    if (!info)
        return "未识别的遗物";
    switch (info->kind) {
    case RelicKind::Rolled:
        return info->family == RelicFamily::Deep ? "深层遗物" : "普通遗物";
    case RelicKind::Unique:
        return info->family == RelicFamily::Deep ? "独特深层遗物" : "独特遗物";
    case RelicKind::Fixed:
        return info->family == RelicFamily::Deep ? "固定深层遗物" : "固定遗物";
    }
    return "未识别的遗物";
}
} // namespace ern
