#include "imedit.h"
#include "imgui.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ImEdit {

static const auto HIGHLIGHT_COLOR = IM_COL32(255, 243, 201, 255);
static const auto HIGHLIGHT_PADDING = ImVec2(2.0f, 1.0f);
static const auto HIGHLIGHT_ROUNDING = 2.0f;
static std::string current_editor;
static std::unordered_map<std::string, std::vector<std::string>> data;
static std::unordered_map<std::string, std::pair<int, int>> cursors;
static std::unordered_map<
    std::string,
    std::unordered_map<
        int, std::vector<std::tuple<int, int, std::function<void(void)>>>>>
    highlights;
static std::unordered_map<std::string, std::pair<int, int>> prev_cursors;

static std::pair<ImVec2, std::vector<std::function<void(void)>>>
render_row(const std::string& str, int row) {
    auto* drawList = ImGui::GetWindowDrawList();
    const auto cursor_did_move =
        prev_cursors[current_editor] != cursors[current_editor];
    ImVec2 position;
    std::vector<std::function<void(void)>> functions;
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TextDisabled("%02d", row);
    ImGui::TableNextColumn();
    position = ImGui::GetCursorScreenPos();
    for (auto [start, end, fn] : highlights[current_editor][row]) {
        auto prefix = str.substr(0, start);
        auto match = str.substr(start, end - start);
        auto upper_left = ImVec2();
        upper_left.x = ImGui::GetCursorScreenPos().x +
                       ImGui::CalcTextSize(prefix.c_str()).x -
                       HIGHLIGHT_PADDING.x;
        upper_left.y = ImGui::GetCursorScreenPos().y - HIGHLIGHT_PADDING.y;
        auto lower_right = ImVec2();
        lower_right.x = upper_left.x + ImGui::CalcTextSize(match.c_str()).x +
                        HIGHLIGHT_PADDING.x;
        lower_right.y = upper_left.y + ImGui::CalcTextSize(match.c_str()).y +
                        HIGHLIGHT_PADDING.y;
        auto hovered = ImGui::IsMouseHoveringRect(upper_left, lower_right);
        drawList->AddRectFilled(upper_left, lower_right, HIGHLIGHT_COLOR,
                                HIGHLIGHT_ROUNDING);
        if (hovered && ImGui::IsMouseClicked(0)) {
            functions.push_back(fn);
        }
    }
    ImGui::Text("%s", str.c_str());
    if (cursors[current_editor].second == row && cursor_did_move &&
        !ImGui::IsItemVisible()) {
        ImGui::SetScrollHereY(0.5f);
    }
    return {position, functions};
}

void Begin(const char* str) { current_editor = str; }

void Line(const char* str) { data[current_editor].emplace_back(str); }

void Highlight(int row, int x0, int x1, std::function<void(void)> fn) {
    highlights[current_editor][row + 1].emplace_back(x0, x1, fn);
}

void Cursor(int x, int y) { cursors[current_editor] = {x, y}; }

void End() {
    constexpr int COLUMNS = 2;
    static ImGuiTableFlags flags =
        ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Hideable;
    auto [x, y] = cursors[current_editor];
    auto* drawList = ImGui::GetWindowDrawList();
    std::unordered_map<int, ImVec2> positions;
    std::vector<std::function<void(void)>> functions;
    if (ImGui::BeginTable((current_editor + "#table").c_str(), COLUMNS,
                          flags)) {
        ImGui::TableSetupColumn((current_editor + "#number_column").c_str(),
                                ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn((current_editor + "#text_column").c_str(),
                                ImGuiTableColumnFlags_WidthStretch);
        int row = 1;
        int buf_pos = 0;
        for (const auto& str : data[current_editor]) {
            auto [pos, fns] = render_row(str, row);
            positions[row] = pos;
            for (auto fn : fns) {
                functions.push_back(fn);
            }
            row++;
        }
        ImGui::EndTable();
    }
    // draw cursor
    auto cursor_upper_left = positions[y + 1];
    auto cursor_lower_right =
        ImVec2(cursor_upper_left.x + 1.0f,
               cursor_upper_left.y + ImGui::GetTextLineHeight());
    drawList->AddRectFilled(cursor_upper_left, cursor_lower_right,
                            IM_COL32(0, 0, 0, 255));
    data[current_editor] = {};
    prev_cursors[current_editor] = cursors[current_editor];
    highlights[current_editor] = {};
    cursors[current_editor] = {};
    current_editor = {};
    // call functions
    for (auto fn : functions) {
        fn();
    }
}

} // namespace ImEdit
