#pragma once

#include <optional>

namespace ImEdit {

void Begin(const char* name);
std::optional<std::pair<int,int>> Line(const char* content);
void Cursor(int x);
bool Highlight(int x0, int x1, int color);
bool Underline(int x0, int x1);
void End();

} // namespace ImEdit
