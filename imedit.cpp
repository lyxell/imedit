#include "imedit.h"
#include "imgui.h"

#include <string>

namespace ImEdit {

static const float number_column_width = 20.0f;
static std::string current_editor;
static int current_row;

ImVec2 get_upper_left(int x0, int x1) {
    ImGuiStyle& style = ImGui::GetStyle();
    return ImVec2(ImGui::GetCursorScreenPos().x +
                      ImGui::CalcTextSize("x").x * x0 + number_column_width +
                      style.CellPadding.x * 2.0f,
                  ImGui::GetCursorScreenPos().y + style.CellPadding.y);
}
ImVec2 get_lower_right(int x0, int x1) {
    auto upper_left = get_upper_left(x0, x1);
    return ImVec2(upper_left.x + ImGui::CalcTextSize("x").x * (x1 - x0),
                  upper_left.y + ImGui::GetTextLineHeight());
}
ImVec2 get_lower_left(int x0, int x1) {
    ImGuiStyle& style = ImGui::GetStyle();
    return ImVec2(ImGui::GetCursorScreenPos().x +
                      ImGui::CalcTextSize("x").x * x0 + number_column_width +
                      style.CellPadding.x * 2.0f,
                  ImGui::GetCursorScreenPos().y + style.CellPadding.y +
                      ImGui::GetTextLineHeight());
}

void Begin(const char* str) { current_editor = str; }

void Line(const char* str) {
    constexpr int COLUMNS = 2;
    current_row++;
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit;
    if (ImGui::BeginTable((current_editor + "#Line").c_str(), COLUMNS, flags)) {
        ImGui::TableSetupColumn((current_editor + "#number_column").c_str(), 0,
                                number_column_width);
        ImGui::TableSetupColumn((current_editor + "#text_column").c_str(),
                                ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextColumn();
        ImGui::TextDisabled("%d", current_row);
        ImGui::TableNextColumn();
        ImGui::Text("%s", str);
        ImGui::EndTable();
    }
}

bool Highlight(int x0, int x1) {
    auto* draw_list = ImGui::GetWindowDrawList();
    auto upper_left = get_upper_left(x0, x1);
    auto lower_right = get_lower_right(x0, x1);
    draw_list->AddRectFilled(upper_left, lower_right,
                             IM_COL32(150, 200, 255, 255));
    return ImGui::IsMouseHoveringRect(upper_left, lower_right) &&
           ImGui::IsMouseClicked(0);
}

bool Underline(int x0, int x1) {
    ImGuiStyle& style = ImGui::GetStyle();
    auto* draw_list = ImGui::GetWindowDrawList();
    auto curr = get_lower_left(x0, x1);
    auto end_x = curr.x + ImGui::CalcTextSize("x").x * (x1 - x0);
    auto incr_x = 2.0f;
    bool bump = 0;
    while (curr.x < end_x) {
        if (bump) {
            draw_list->AddLine(ImVec2(curr.x, curr.y),
                               ImVec2(curr.x + incr_x, curr.y + 1.0f),
                               IM_COL32(255, 0, 0, 255), 1.0f);
        } else {
            draw_list->AddLine(ImVec2(curr.x, curr.y + 1.0f),
                               ImVec2(curr.x + incr_x, curr.y),
                               IM_COL32(255, 0, 0, 255), 1.0f);
        }
        curr.x += incr_x;
        bump = !bump;
    }
    return ImGui::IsMouseHoveringRect(get_upper_left(x0, x1),
                                      get_lower_right(x0, x1)) &&
           ImGui::IsMouseClicked(0);
}

void Cursor(int x) {
    ImGuiStyle& style = ImGui::GetStyle();
    auto* draw_list = ImGui::GetWindowDrawList();
    auto upper_left =
        ImVec2(ImGui::GetCursorScreenPos().x + ImGui::CalcTextSize("x").x * x +
                   number_column_width + style.CellPadding.x * 2.0f,
               ImGui::GetCursorScreenPos().y + style.CellPadding.y);
    auto lower_right =
        ImVec2(upper_left.x + 1.0f, upper_left.y + ImGui::GetTextLineHeight());
    draw_list->AddRectFilled(upper_left, lower_right, IM_COL32(0, 0, 0, 255));
}

void End() { current_row = 0; }

} // namespace ImEdit
