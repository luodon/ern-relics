#pragma once

#include "data/preset_types.hpp"
#include "relics/edit_result.hpp"

namespace ern::ui {
inline std::string rejection_message(const EditRejected &rejected) {
    if (!rejected.detail.empty())
        return rejected.detail;
    switch (rejected.reason) {
    case EditRejectReason::NotAtRoundtable:
        return "仅在圆桌厅堂内可编辑遗物。请返回圆桌厅堂并等待加载完成。";
    case EditRejectReason::Stale:
        return "遗物已在游戏内更改。应用更改前请重新读取该遗物。";
    case EditRejectReason::Locked:
        return "该遗物为只读，无法修改其词条。";
    case EditRejectReason::Invalid:
        return "遗物最终校验失败，未更改任何词条。";
    case EditRejectReason::Unreadable:
        return "无法解析已装备的遗物。请等待加载完成后刷新。";
    case EditRejectReason::RulesUnavailable:
        return "遗物抽取数据不可用。";
    case EditRejectReason::JournalOpen:
        return "无法打开备份日志，未更改任何词条。";
    case EditRejectReason::JournalWrite:
        return "无法写入备份日志，未更改任何词条。";
    case EditRejectReason::Unchanged:
        return "没有词条发生更改。";
    case EditRejectReason::EquipUnavailable:
        return "游戏尚无法卸下该遗物，未更改任何词条。请在加载完成后重试。";
    }
    return "未知的编辑拒绝原因。";
}

inline std::string failure_message(const EditFailure &failure) {
    if (const auto *rejected = std::get_if<EditRejected>(&failure))
        return rejection_message(*rejected);
    const auto failed = std::get<WriteFailure>(failure);
    const char *reason = failed.stage == WriteStage::Write       ? "内存写入失败。"
                         : failed.stage == WriteStage::Equipment ? "遗物装备切换失败。"
                                                                 : "无法验证更改结果。";
    return std::string(reason) + (failed.restoration == Restoration::Verified
                                      ? "原有词条已恢复。"
                                      : "无法验证恢复结果。请先检查该遗物再继续。");
}

inline std::string slot_failure_message(const SlotFailure &failure) {
    return "槽位 " + std::to_string(failure.slot + 1) + "：" + failure_message(failure.failure);
}

inline std::string preset_store_message(PresetStoreError error) {
    switch (error) {
    case PresetStoreError::InvalidName:
        return "请使用 1-32 个字母、数字、空格、下划线或连字符。首尾不能为空格，并避免使用 CON、NUL、COM1、LPT1 等 Windows 设备名。";
    case PresetStoreError::AlreadyExists:
        return "同名预设已存在。请选择“替换预设”覆盖它，或改用其他名称。";
    case PresetStoreError::Read:
        return "无法读取预设文件夹。";
    case PresetStoreError::CreateFolder:
        return "无法创建预设文件夹。";
    case PresetStoreError::Write:
        return "无法保存预设。";
    case PresetStoreError::Delete:
        return "无法删除预设。";
    }
    return "未知的预设错误。";
}
} // namespace ern::ui
