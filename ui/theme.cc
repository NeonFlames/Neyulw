#include "theme.hh"

#define NEYULW_STYLE_FG ImVec4(0.78, 1.0, 0.78, 1.0)
#define NEYULW_STYLE_BORDER ImVec4(0.78, 1.0, 0.78, 0.7)
#define NEYULW_STYLE_TEXT_HIGHLIGHT ImVec4(0.0, 1.0, 0.0, 0.4)
#define NEYULW_STYLE_TEXT_DISABLED ImVec4(1.0, 1.0, 1.0, 0.7)
#define NEYULW_STYLE_BG ImVec4(0.0, 0.06, 0.0, 1.0)
#define NEYULW_STYLE_ALT_BG ImVec4(0.0, 0.1, 0.05, 1.0)
#define NEYULW_STYLE_ACTIVE_BG ImVec4(0.0, 0.60, 0.0, 1.0)
#define NEYULW_STYLE_HOVER_BG ImVec4(0.0, 0.45, 0.0, 1.0)
#define NEYULW_STYLE_INACTIVE_BG ImVec4(0.0, 0.35, 0.0, 1.0)
#define NEYULW_STYLE_TABLE_HEADER ImVec4(0.0, 0.2, 0.0, 1.0)

void NeyulwStyle(ImGuiStyle &style) {
    style.AntiAliasedLines = true;
    style.Alpha = 1.0;
    style.ChildRounding = 0.0;
    style.WindowRounding = 0.0;
    style.FrameRounding = 0.0;
    style.WindowBorderSize = 0.0;
    style.PopupBorderSize *= 1.5;
    style.CellPadding = ImVec2(4.0, 0.0);

    style.Colors[ImGuiCol_Text] = NEYULW_STYLE_FG;
    style.Colors[ImGuiCol_ScrollbarGrab] = NEYULW_STYLE_FG;
    style.Colors[ImGuiCol_CheckMark] = NEYULW_STYLE_FG;
    style.Colors[ImGuiCol_Border] = NEYULW_STYLE_BORDER;
    style.Colors[ImGuiCol_TextSelectedBg] = NEYULW_STYLE_TEXT_HIGHLIGHT;
    style.Colors[ImGuiCol_TextDisabled] = NEYULW_STYLE_TEXT_DISABLED;
    style.Colors[ImGuiCol_WindowBg] = NEYULW_STYLE_BG;
    style.Colors[ImGuiCol_TableRowBg] = NEYULW_STYLE_BG;
    style.Colors[ImGuiCol_Tab] = NEYULW_STYLE_BG;
    style.Colors[ImGuiCol_TabDimmed] = NEYULW_STYLE_BG;
    style.Colors[ImGuiCol_PopupBg] = NEYULW_STYLE_BG;
    style.Colors[ImGuiCol_TableRowBgAlt] = NEYULW_STYLE_ALT_BG;
    style.Colors[ImGuiCol_NavCursor] = NEYULW_STYLE_HOVER_BG;
    style.Colors[ImGuiCol_TabSelected] = NEYULW_STYLE_ACTIVE_BG;
    style.Colors[ImGuiCol_TabDimmedSelected] = NEYULW_STYLE_ACTIVE_BG;
    style.Colors[ImGuiCol_ButtonActive] = NEYULW_STYLE_ACTIVE_BG;
    style.Colors[ImGuiCol_FrameBgActive] = NEYULW_STYLE_ACTIVE_BG;
    style.Colors[ImGuiCol_HeaderActive] = NEYULW_STYLE_ACTIVE_BG;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = NEYULW_STYLE_ACTIVE_BG;
    style.Colors[ImGuiCol_ButtonHovered] = NEYULW_STYLE_HOVER_BG;
    style.Colors[ImGuiCol_HeaderHovered] = NEYULW_STYLE_HOVER_BG;
    style.Colors[ImGuiCol_FrameBgHovered] = NEYULW_STYLE_HOVER_BG;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = NEYULW_STYLE_HOVER_BG;
    style.Colors[ImGuiCol_TabHovered] = NEYULW_STYLE_HOVER_BG;
    style.Colors[ImGuiCol_Button] = NEYULW_STYLE_INACTIVE_BG;
    style.Colors[ImGuiCol_FrameBg] = NEYULW_STYLE_INACTIVE_BG;
    style.Colors[ImGuiCol_Header] = NEYULW_STYLE_INACTIVE_BG;
    style.Colors[ImGuiCol_TableHeaderBg] = NEYULW_STYLE_TABLE_HEADER;
}
