#pragma once

#include <imgui-SFML.h>
#include <imgui.h>

namespace path_finding_visualizer {
namespace gui {

class LoggerPanel {
 public:
  LoggerPanel() {
    AutoScroll = true;
    clear();
  }

  void clear() {
    Buf.clear();
    LineOffsets.clear();
    LineOffsets.push_back(0);
  }

  void render(const char* title) {
    if (!ImGui::Begin(title)) {
      ImGui::End();
      return;
    }

    ImGui::BeginChild("scrolling", ImVec2(0, 0), false,
                      ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    const char* buf = Buf.begin();
    const char* buf_end = Buf.end();
    {
      ImGuiListClipper clipper;
      clipper.Begin(LineOffsets.Size);
      while (clipper.Step()) {
        for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd;
             line_no++) {
          const char* line_start = buf + LineOffsets[line_no];
          const char* line_end = (line_no + 1 < LineOffsets.Size)
                                     ? (buf + LineOffsets[line_no + 1] - 1)
                                     : buf_end;
          ImGui::TextUnformatted(line_start, line_end);
        }
      }
      clipper.End();
    }
    ImGui::PopStyleVar();

    if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
      ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
    ImGui::End();
  }

  void info(const std::string& msg) { AddLog("[INFO] %s\n", msg.c_str()); }

 private:
  ImGuiTextBuffer Buf;
  ImVector<int> LineOffsets;  // Index to lines offset. We maintain this with
                              // AddLog() calls.
  bool AutoScroll;            // Keep scrolling if already at the bottom.

  void AddLog(const char* fmt, ...) IM_FMTARGS(2) {
    int old_size = Buf.size();
    va_list args;
    va_start(args, fmt);
    Buf.appendfv(fmt, args);
    va_end(args);
    for (int new_size = Buf.size(); old_size < new_size; old_size++)
      if (Buf[old_size] == '\n') LineOffsets.push_back(old_size + 1);
  }
};

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a
// merged icon fonts (see docs/FONTS.md)
inline void HelpMarker(const char* desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

inline bool inputInt(const std::string& label, int* val, const int& min_val,
                     const int& max_val, const int& step = 1,
                     const int& step_fast = 100,
                     const std::string& help_marker = "",
                     ImGuiInputTextFlags flags = 0) {
  flags |= ImGuiInputTextFlags_EnterReturnsTrue;
  bool is_active = ImGui::InputInt(label.c_str(), val, step, step_fast, flags);
  if (!help_marker.empty()) {
    ImGui::SameLine();
    HelpMarker(help_marker.c_str());
  }
  if (is_active) {
    if (*val < min_val)
      *val = min_val;
    else if (*val > max_val)
      *val = max_val;
  }
  return is_active;
}

inline bool inputDouble(const std::string& label, double* val,
                        const double& min_val, const double& max_val,
                        const double& step = 0.0, const double& step_fast = 0.0,
                        const std::string& help_marker = "",
                        const char* format = "%.6f",
                        ImGuiInputTextFlags flags = 0) {
  flags |= ImGuiInputTextFlags_EnterReturnsTrue;
  bool is_active =
      ImGui::InputDouble(label.c_str(), val, step, step_fast, format, flags);
  if (!help_marker.empty()) {
    ImGui::SameLine();
    HelpMarker(help_marker.c_str());
  }
  if (is_active) {
    if (*val < min_val)
      *val = min_val;
    else if (*val > max_val)
      *val = max_val;
  }
  return is_active;
}

}  // namespace gui
}  // namespace path_finding_visualizer