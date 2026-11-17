#include "ui/diagnostics.hpp"
#include "app/editor_view.hpp"
#include <algorithm>
#include <cmath>

namespace ern::ui {
void draw_diagnostics(const EditorView &rt, std::optional<EditorAction> &action,
                      const Appearance &appearance, const EquippedRelics *equipped,
                      std::size_t selected, UiRect editor) {
    const auto position = ImGui::GetCursorScreenPos();
    if (button("诊断", {appearance.space(24), 0}))
        ImGui::OpenPopup("Diagnostics");
    const auto bounds = effect_popup_bounds(
        editor,
        {position.x, position.y - appearance.space(2),
         std::min(appearance.space(160), editor.width - appearance.space(9)), appearance.space(8)},
        appearance.space(160), appearance.space(3));
    ImGui::SetNextWindowPos({bounds.x, bounds.y + bounds.height}, ImGuiCond_Always, {0, 1});
    ImGui::SetNextWindowSizeConstraints({bounds.width, 0}, {bounds.width, bounds.height});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
                        {std::round(appearance.space(4)), std::round(appearance.space(4))});
    if (ImGui::BeginPopup("Diagnostics", ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                             ImGuiWindowFlags_AlwaysAutoResize)) {
        heading("诊断", appearance.name_font, ImGui::GetFrameHeight());
        const float action_width = appearance.space(18), gap = appearance.space(2);
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - action_width * 2 - gap);
        const bool refresh = button("刷新", {action_width, 0});
        ImGui::SameLine(0, gap);
        if (button("关闭", {action_width, 0}))
            ImGui::CloseCurrentPopup();
        vertical_gap(appearance.space(3));
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, {appearance.space(1), appearance.space(1)});
        ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextBorderSize, 1.f);
        const auto row = [](const char *label, const std::string &value, ImVec4 color = ivory) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(muted, "%s", label);
            ImGui::TableSetColumnIndex(1);
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextWrapped("%s", value.c_str());
            ImGui::PopStyleColor();
        };
        ImGui::SeparatorText("会话");
        if (ImGui::BeginTable("session-values", 2, ImGuiTableFlags_SizingFixedFit)) {
            ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed,
                                    appearance.space(24));
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            row("游戏", rt.version);
            row("位置", rt.location == EditLocation::RoundtableHold      ? "圆桌厅堂"
                        : rt.location == EditLocation::OutsideRoundtable ? "圆桌厅堂外"
                                                                         : "未加载或不可用");
            row("词条目录", std::to_string(rt.catalog().size()) + " 个词条，" +
                                std::to_string(rt.items().size()) + " 个遗物");
            if (const auto *dlc = std::get_if<DlcState>(&rt.context))
                row("DLC", *dlc == DlcState::Present  ? "已安装"
                           : *dlc == DlcState::Absent ? "未安装"
                                                      : "未知；两套规则必须一致");
            if (const auto *reason = rt.editing_lock_reason())
                row("编辑", reason, amber);
            if (equipped && equipped->slots[selected]) {
                const auto &relic = *equipped->slots[selected];
                row("物品 ID", std::to_string(relic.item_id & 0x0FFFFFFF));
                row("背包序号", std::to_string(relic.inventory_index()));
            }
            row("编辑日志", rt.journal_path.filename().string());
            ImGui::EndTable();
        }
        ImGui::Spacing();
        ImGui::SeparatorText("规则文件");
        const auto &diagnostic = rt.rule_diagnostics;
        const auto path_text = [](const std::filesystem::path &path) {
            const auto utf8 = path.u8string();
            return std::string(utf8.begin(), utf8.end());
        };
        if (ImGui::BeginTable("regulation-values", 2, ImGuiTableFlags_SizingFixedFit)) {
            ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed,
                                    appearance.space(24));
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            if (diagnostic.requested_path.empty())
                row("来源", rt.source_label);
            else {
                const auto requested = path_text(diagnostic.requested_path),
                           resolved = path_text(diagnostic.resolved_path);
                row("请求的文件", requested.empty() ? "不可用" : requested);
                row("打开的文件", resolved.empty() ? "不可用" : resolved);
                row("检测到的哈希",
                    diagnostic.detected_hash.empty() ? "不可用" : diagnostic.detected_hash);
                row("校验哈希", rt.source_hash.empty() ? "不可用" : rt.source_hash);
                row("规则",
                    !diagnostic.error.empty()                    ? diagnostic.error
                    : diagnostic.detected_hash == rt.source_hash ? "与当前 regulation.bin 匹配"
                                                                 : "未检查",
                    !diagnostic.error.empty() ? amber : ivory);
                row("本地资源", std::to_string(rt.assets().item_names.size()) + " 个物品名称，" +
                                    std::to_string(rt.assets().effect_names.size()) +
                                    " 个词条名称，" + std::to_string(rt.assets().cells.size()) +
                                    " 个图标");
                if (!rt.assets().warnings.empty())
                    row("资源回退", rt.assets().warnings.front(), amber);
            }
            ImGui::EndTable();
        }
        ImGui::PopStyleVar(2);
        // The application worker publishes the refreshed state independently of drawing.
        if (refresh)
            action = RefreshInspection{};
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();
}
} // namespace ern::ui
