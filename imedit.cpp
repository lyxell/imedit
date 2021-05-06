#include "imedit.h"
#include "imgui.h"

#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <functional>

namespace ImEdit {

static const auto HIGHLIGHT_COLOR = IM_COL32(255, 243, 201, 255);
static const auto HIGHLIGHT_PADDING = ImVec2(2.0f, 1.0f);
static const auto HIGHLIGHT_ROUNDING = 2.0f;
static std::string current_editor;
static std::unordered_map<std::string, std::vector<std::string>> data;
static std::unordered_map<std::string, std::pair<int,int>> cursors;
static std::unordered_map<std::string, std::unordered_map<int, std::vector<std::tuple<int,int,std::function<void(void)>>>>> highlights;
static std::unordered_map<std::string, std::pair<int,int>> prev_cursors;

void Begin(const char* str) {
    current_editor = str;
}

void Line(const char* str) {
    data[current_editor].emplace_back(str);
}

void Highlight(int row, int x0, int x1, std::function<void(void)> fn) {
    highlights[current_editor][row+1].emplace_back(x0, x1, fn);
}

void Cursor(int x, int y) {
    cursors[current_editor] = {x, y};
}

void End() {
    constexpr int COLUMNS = 2;
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Hideable;
    auto [x, y] = cursors[current_editor];
    auto cursor_did_move = prev_cursors[current_editor] != cursors[current_editor];
    auto* drawList = ImGui::GetWindowDrawList();
    std::unordered_map<int, ImVec2> positions;
    std::vector<std::function<void(void)>> functions;
    if (ImGui::BeginTable((current_editor + "#table").c_str(), COLUMNS, flags)) {
        ImGui::TableSetupColumn((current_editor + "#number_column").c_str(), ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn((current_editor + "#text_column").c_str(), ImGuiTableColumnFlags_WidthStretch);
        int row = 1;
        int buf_pos = 0;
        for (const auto& str : data[current_editor]) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::TextDisabled("%02d", row);
            ImGui::TableNextColumn();
            positions[row] = ImGui::GetCursorScreenPos();
            for (auto [start, end, fn] : highlights[current_editor][row]) {
                auto prefix = str.substr(0, start);
                auto match = str.substr(start, end - start);
                auto suffix = str.substr(end);
                auto rect_upper_left =
                    ImVec2(ImGui::GetCursorScreenPos().x + ImGui::CalcTextSize(prefix.c_str()).x - HIGHLIGHT_PADDING.x,
                           ImGui::GetCursorScreenPos().y - HIGHLIGHT_PADDING.y);
                auto rect_lower_right = ImVec2(rect_upper_left.x + ImGui::CalcTextSize(match.c_str()).x + HIGHLIGHT_PADDING.x,
                                               rect_upper_left.y + ImGui::CalcTextSize(match.c_str()).y + HIGHLIGHT_PADDING.y);

                auto hovered = ImGui::IsMouseHoveringRect(rect_upper_left, rect_lower_right);
                drawList->AddRectFilled(rect_upper_left, rect_lower_right, HIGHLIGHT_COLOR,
                                        HIGHLIGHT_ROUNDING);
                if (hovered && ImGui::IsMouseClicked(0)) {
                    functions.push_back(fn);
                }

            }
            ImGui::Text("%s", str.c_str());
            if (cursors[current_editor].second == row && cursor_did_move && !ImGui::IsItemVisible()) {
                ImGui::SetScrollHereY(0.5f);
            }
            row++;
        }
        ImGui::EndTable();
    }
    // draw cursor
    auto cursor_upper_left = positions[y+1];
    auto cursor_lower_right = ImVec2(cursor_upper_left.x + 1.0f,
                              cursor_upper_left.y + ImGui::GetTextLineHeight());
    drawList->AddRectFilled(cursor_upper_left, cursor_lower_right, IM_COL32(0, 0, 0, 255));
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

}
