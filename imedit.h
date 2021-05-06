#pragma once

#include <functional>

namespace ImEdit {

void Begin(const char* name);
void Line(const char* content);
void Cursor(int x, int y);
void Highlight(int y, int x0, int y0, std::function<void(void)> fn);
void End();

} // namespace ImEdit
