#include <filesystem>
#include "configure.hh"
#include "winit.hh"
#include "imgui_stdlib.h"
#include "tinyfiledialogs.h"

void _WInitUpdateVars(NeyulwState *state) {
    struct WInitData *data = (struct WInitData*)state->ui_data;
    data->vars.clear();
    for (auto& var : state->configs[data->config]->vars) {
        data->vars[var.first] = std::string();
    }
}

void WInit(NeyulwState *state) {
    bool configure;
    struct WInitData *data = (struct WInitData*)state->ui_data;
    Config *cfg = state->configs.at(data->config);
    ImGui::Begin("##WInit", 0, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize);
    ImGui::SetWindowPos(ImVec2 {0.0f, 0.0f});
    ImGui::SetWindowSize(ImVec2 {(float)state->width, (float)state->height});
    configure = ImGui::Button("Configure") && data->ready(state);
    ImGui::SameLine();
    ImGui::PushItemWidth(-FLT_MIN);
    if (ImGui::BeginCombo("##config", cfg->name.c_str(), ImGuiConfigFlags_NavEnableKeyboard)) {
        {
            for (size_t i = 0; i < state->configs.size(); i++) {
                if (ImGui::Selectable(state->configs[i]->name.c_str(), i == data->config)) {
                    data->config = i;
                }
            }
        }
        ImGui::EndCombo();
    }
    ImGui::PopItemWidth();
    ImGui::BeginTable("##vars", 3, ImGuiTableFlags_BordersInnerH|ImGuiTableFlags_SizingFixedFit);
    ImGui::TableSetupColumn("Variable", ImGuiTableColumnFlags_NoResize|ImGuiTableColumnFlags_NoSort);
    ImGui::TableSetupColumn("##value", ImGuiTableColumnFlags_NoResize|ImGuiTableColumnFlags_NoSort|ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_NoResize|ImGuiTableColumnFlags_NoSort);
    ImGui::TableHeadersRow();
    for (auto var : cfg->vars) {
        ImGui::PushID(var.first.c_str());
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::TextUnformatted(var.second.name.c_str());
        ImGui::TableNextColumn();
        ImGui::PushItemWidth(-FLT_MIN);
        ImGui::InputTextWithHint("##input", var.second.hint.c_str(), &data->vars[var.first]);
        ImGui::PopItemWidth();
        ImGui::TableNextColumn();
        if (var.second.type == "file") {
            if (ImGui::Button("File")) {
                const char *path = tinyfd_openFileDialog(var.second.name.c_str(), data->vars[var.first].data(), 0, 0, 0, 0);
                if (path != NULL) data->vars[var.first] = std::string(path);
            }
        } else {
            if (ImGui::Button("Folder")) {
                const char *path = tinyfd_selectFolderDialog(var.second.name.c_str(), data->vars[var.first].c_str());
                if (path != NULL) data->vars[var.first] = std::string(path);
            }
        }
        ImGui::PopID();
    }
    ImGui::EndTable();
    ImGui::End();
    if (configure) {
        ConfigureSetup(state);
    }
}

void WInitSetup(NeyulwState *state) {
    struct WInitData *data = new struct WInitData;
    data->config = 0;
    data->ready = [](NeyulwState *state){
        struct WInitData *data = (struct WInitData*)state->ui_data;
        for (auto &var : data->vars) {
            if (var.second.empty()) return false;
            if (state->configs[data->config]->vars[var.first].type == "file" && !std::filesystem::is_regular_file(var.second)) return false;
            else if (!std::filesystem::is_directory(var.second)) return false;
        }
        return true;
    };
    state->ui_data = data;
    state->ui = WInit;
}
