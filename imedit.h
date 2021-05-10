#pragma once

namespace ImEdit {

void Begin(const char* name);
void Line(const char* content);
void Cursor(int x);
bool Highlight(int x0, int x1);
bool Underline(int x0, int x1);
void End();

} // namespace ImEdit
