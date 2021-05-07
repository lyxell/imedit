#pragma once

#include <functional>

namespace ImEdit {

void Begin(const char* name);
void Line(const char* content);
void Cursor(int x);
void Highlight(int x0, int x1);
void Underline(int x0, int x1);
void End();

} // namespace ImEdit
