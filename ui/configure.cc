#include <algorithm>
#include <regex>
#include "configure.hh"
#include "io.hh"
#include "imgui_stdlib.h"

void Configure(NeyulwState *state) {
    struct ConfigureData *data = (struct ConfigureData*)state->ui_data;
    ImGui::Begin("##Configure", 0, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize);
    ImGui::SetWindowPos(ImVec2 {0.0f, 0.0f});
    ImGui::SetWindowSize(ImVec2 {(float)state->width, (float)state->height});
    if (data->presets_) {
        if (ImGui::BeginTable("##c", 2)) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::Button("Apply", ImVec2(-FLT_MIN, 0))) {
                data->apply_presets(data);
                data->presets_ = false;
            }
            ImGui::TableNextColumn();
            if (ImGui::Button("Cancel", ImVec2(-FLT_MIN, 0))) data->presets_ = false;
            ImGui::EndTable();
        }
        if (ImGui::BeginTable("##presets", 2)) {
            ImGui::TableSetupColumn("Inactive");
            ImGui::TableSetupColumn("Active");
            ImGui::TableHeadersRow();
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::BeginTable("##inactive", 1, ImGuiTableFlags_ScrollY|ImGuiTableFlags_RowBg|ImGuiTableFlags_BordersInnerH)) {
                for (auto &preset : data->presets) {
                    if (preset.active) continue;
                    ImGui::PushID(preset.name.c_str());
                    ImGui::PushItemWidth(-FLT_MIN);
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::PushFont(state->bold);
                    ImGui::Text("%s", preset.name.c_str());
                    ImGui::PopFont();
                    if (!preset.hint.empty()) ImGui::TextWrapped("%s", preset.hint.c_str());
                    if (!preset.subsets.empty()) {
                        if (ImGui::BeginCombo("##subsets", preset.subsets[preset.subset].c_str())) {
                            for (size_t i = 0; i < preset.subsets.size(); i++) {
                                if (ImGui::Selectable(preset.subsets[i].c_str(), i == preset.subset)) preset.subset = i;
                            }
                            ImGui::EndCombo();
                        }
                    }
                    if (ImGui::Button(">", ImVec2(-FLT_MIN, 0))) {
                        preset.active = data->presets_active.size()+1;
                        data->update_presets(data);
                    }
                    ImGui::PopItemWidth();
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::TableNextColumn();
            if (ImGui::BeginTable("##active", 1, ImGuiTableFlags_ScrollY|ImGuiTableFlags_RowBg|ImGuiTableFlags_BordersInnerH)) {
                ImVec2 table_size =  ImGui::GetItemRectSize();
                for (auto &preset : data->presets_active) {
                    ImGui::PushID(preset->name.c_str());
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::PushFont(state->bold);
                    ImGui::Text("%s", preset->name.c_str());
                    ImGui::PopFont();
                    ImGui::PushItemWidth(-FLT_MIN);
                    if (!preset->hint.empty()) ImGui::TextWrapped("%s", preset->hint.c_str());
                    if (!preset->subsets.empty()) {
                        if (ImGui::BeginCombo("##subsets", preset->subsets[preset->subset].c_str())) {
                            for (size_t i = 0; i < preset->subsets.size(); i++) {
                                if (ImGui::Selectable(preset->subsets[i].c_str(), i == preset->subset)) preset->subset = i;
                            }
                            ImGui::EndCombo();
                        }
                    }
                    ImGui::PopItemWidth();
                    ImVec2 item_size = ImVec2(table_size.x/3, 0);
                    if (ImGui::Button("<", item_size)) {
                        preset->active = 0;
                        data->update_presets(data);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("↑", item_size)) {
                        size_t active = preset->active;
                        if (active > 1) {
                            data->presets_active[active-2]->active++;
                            preset->active--;
                            data->update_presets(data);
                        }
                    };
                    ImGui::SameLine();
                    if (ImGui::Button("↓", item_size)) {
                        size_t active = preset->active;
                        if (active < data->presets_active.size()) {
                            data->presets_active[active]->active--;
                            preset->active++;
                            data->update_presets(data);
                        }
                    }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::EndTable();
        }
    } else {
        ImGui::PushFont(state->bold);
        ImGui::Text("Neyulw");
        ImGui::PopFont();
        ImGui::SameLine();
        if (ImGui::Button("Presets")) data->presets_ = true;
        ImGui::SameLine();
        if (ImGui::Button("Save")) data->save(state);
        ImGui::SameLine();
        if (ImGui::Button("Quit")) state->run = false;
        ImGui::SameLine();
        if (ImGui::Checkbox("Only Modified", &data->modified)) data->update(data);
        ImGui::SameLine();
        if (ImGui::Checkbox("Search Hints", &data->search_hints)) data->update(data);
        ImGui::SameLine();
        if (ImGui::Checkbox("Strict Search", &data->search_strict)) data->update(data);
        ImGui::PushItemWidth(-FLT_MIN);
        if (ImGui::InputTextWithHint("##Search", "Search Options", &data->search)) data->update(data);
        ImGui::PopItemWidth();
        ImGui::BeginTable("##options", 5, ImGuiTableFlags_BordersInnerH|ImGuiTableFlags_ScrollY|ImGuiTableFlags_RowBg|ImGuiTableFlags_SizingFixedFit|ImGuiTableFlags_Resizable|ImGuiTableFlags_Hideable);
        ImGui::TableSetupColumn("File");
        ImGui::TableSetupColumn("Section", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Option", ImGuiTableColumnFlags_WidthStretch|ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Hint", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch|ImGuiTableColumnFlags_NoHide);
        ImGui::TableHeadersRow();
        for (auto &option : data->options) {
            ImGui::TableNextRow();
            ImGui::PushID(option.option.c_str());
            if (ImGui::TableNextColumn()) ImGui::Text("%s", option.file.c_str());
            if (ImGui::TableNextColumn()) ImGui::Text("%s", option.section.c_str());
            if (ImGui::TableNextColumn()) ImGui::Text("%s", option.option.c_str());
            if (ImGui::TableNextColumn()) ImGui::TextWrapped("%s", option.hint.c_str());
            if (ImGui::TableNextColumn()) {
                ImGui::PushItemWidth(-FLT_MIN);
                ImGui::InputText("##value", option.value);
                ImGui::PopItemWidth();
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

bool _ConfigureSetupSortA(_ConfigureDataPreset &a, _ConfigureDataPreset &b) {
    if (a.id != 0 && (b.id == 0 || a.id < b.id)) return true;
    return false;
}

bool _ConfigureSetupSortB(_ConfigureDataPreset *a, _ConfigureDataPreset *b) {
    if (a->active < b->active) return true;
    return false;
}

void ConfigureSetup(NeyulwState *state) {
    struct WInitData *wdata = (struct WInitData*)state->ui_data;
    struct ConfigureData *data = new struct ConfigureData();
    state->ui = Configure;
    state->ui_data = data;
    data->config = state->configs[wdata->config];
    data->uconf = new UserConfig();
    Config *config = data->config;
    LoadProgramConfig(config, wdata->vars, data->uconf);
    state->done = [](NeyulwState *state) {
        struct ConfigureData *data = (struct ConfigureData*)state->ui_data;
        SaveProgramConfig(data->config, data->uconf);
    };
    for (auto &config_preset : data->config->presets) {
        struct _ConfigureDataPreset preset {
            config_preset.first,
            config_preset.second.hint,
            config_preset.second.id,
            0,
            0,
            std::vector<std::string>()
        };
        if (!config_preset.second.subsets.empty()) {
            for (auto &subset : config_preset.second.subsets) {
                preset.subsets.push_back(subset.first);
            }
            std::sort(preset.subsets.begin(), preset.subsets.end());
            preset.subsets.insert(preset.subsets.begin(), "None");
        }
        data->presets.push_back(preset);
    }
    std::sort(data->presets.begin(), data->presets.end(), _ConfigureSetupSortA);
    data->save = [](NeyulwState *state) {
        state->done(state);
    };
    data->update = [](struct ConfigureData *data) {
        data->options.clear();
        data->options.reserve(data->config->option_n);
        try {
            std::regex rsearch(data->search, std::regex_constants::ECMAScript|std::regex_constants::icase);
            bool (*match)(std::string, std::regex) = data->search_strict ?
                [](std::string s, std::regex r) {return std::regex_match(s,r);}
                    :
                [](std::string s, std::regex r) {return std::regex_search(s,r);};
            for (auto &file : data->config->files) {
                for (auto &section : file.second) {
                    auto &section_data = section.second;
                    for (auto &option : section_data.options) {
                        std::string *val = &data->uconf->options[file.first][section.first][option.first];
                        if (data->modified && val->empty()) continue;
                        if (!data->search.empty()) {
                            if (data->search_hints) {
                                if (!match(option.second.hint, rsearch) && !match(option.first, rsearch)) continue;
                            } else if (!match(option.first, rsearch)) continue;
                        }
                        data->options.push_back(_ConfigureDataOption {
                            file.first,
                            section_data.name,
                            option.first,
                            option.second.hint,
                            val
                        });
                    }
                }
            }
        } catch (...) {}
    };
    data->apply_presets = [](ConfigureData *data) {
        UserConfig *uconf = data->uconf;
        for (auto &preset : data->presets_active) {
            for (auto &file : data->config->presets[preset->name].files) {
                for (auto &section : file.second) {
                    for (auto &option : section.second) {
                        uconf->options[file.first][section.first][option.first] = option.second;
                    }
                }
            }
            if (preset->subset == 0) continue;
            for (auto &file : data->config->presets[preset->name].subsets[preset->subsets[preset->subset]].files) {
                for (auto &section : file.second) {
                    for (auto &option : section.second) {
                        uconf->options[file.first][section.first][option.first] = option.second;
                    }
                }
            }
        }
    };
    data->update_presets = [](ConfigureData *data) {
        data->presets_active.clear();
        data->presets_active.reserve(data->presets.size());
        for (auto &preset : data->presets) {
            if (preset.active) data->presets_active.push_back(&preset);
        }
        std::sort(data->presets_active.begin(), data->presets_active.end(), _ConfigureSetupSortB);
    };
    data->reset_presets = [](ConfigureData *data) {
        for (auto &preset : data->presets) {
            preset.active = 0;
            preset.subset = 0;
        }
    };
    data->update(data);

    delete wdata;
}
