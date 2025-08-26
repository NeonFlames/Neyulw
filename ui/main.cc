#include <cstdio>
#include <cstring>
#include "SDL3/SDL.h"
#include "SDL3/SDL_render.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "yaml-cpp/yaml.h"
#include "shared.hh"
#include "winit.hh"
#include "theme.hh"

#include "ue4.yaml.h"
#include "ue5.0.yaml.h"
#include "ue5.5.yaml.h"
#include "IosevkaFixed-Regular.ttf.h"
#include "IosevkaFixed-Bold.ttf.h"

inline void _SetupConfig(NeyulwState *state, unsigned char data[], unsigned int size) {
    Config *config = new Config();
    YAML::Node node = YAML::Load(std::string((const char*)data, size)),
        config_name = node["name"], vars = node["vars"], targets = node["targets"], presets = node["presets"], files = node["files"];
    
    if (!config_name || !vars || !targets || !presets || !files) {
        NYW_LOG("Failed to load a config file\n");
        return;
    }
    config->name = config_name.as<std::string>();
    for (auto var : vars) {
        std::string var_id = var.first.as<std::string>(),
            name = var.second["name"].as<std::string>(),
            type = var.second["type"].as<std::string>(),
            hint = var.second["hint"].as<std::string>();
        if (name.empty()) name = var_id;
        config->vars[var_id] = _ConfigVar {name, type, hint};
    }
    for (auto target : targets) {
        std::string target_id = target.first.as<std::string>(),
            target_value = target.second.as<std::string>();
        config->targets[target_id] = target_value;
    }
    for (auto preset : presets) {
        struct _ConfigPreset &p = config->presets[preset.first.as<std::string>()];
        if (YAML::Node hint = preset.second["hint"]) p.hint = hint.as<std::string>();
        if (YAML::Node id = preset.second["id"]) p.id = id.as<int>();
        for (auto cfile : preset.second["files"]) {
            std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &file = p.files[cfile.first.as<std::string>()];
            for (auto csection : cfile.second) {
                std::unordered_map<std::string, std::string> &section = file[csection.first.as<std::string>()];
                for (auto coption : csection.second) {
                    section[coption.first.as<std::string>()] = coption.second.as<std::string>();
                }
            }
        }
        for (auto subset : preset.second["subsets"]) {
            struct _ConfigPresetSubset &s = p.subsets[subset.first.as<std::string>()];
            if (YAML::Node hint = subset.second["hint"]) s.hint = hint.as<std::string>();
            for (auto sfile : subset.second["files"]) {
                std::unordered_map<std::string, std::unordered_map<std::string, std::string>> &file = s.files[sfile.first.as<std::string>()];
                for (auto ssection : sfile.second) {
                    std::unordered_map<std::string, std::string> &section = file[ssection.first.as<std::string>()];
                    for (auto soption : ssection.second) {
                        section[soption.first.as<std::string>()] = soption.second.as<std::string>();
                    }
                }
            }
        }
    }
    for (auto cfile : files) {
        std::map<std::string, _ConfigFileSection> &file = config->files[cfile.first.as<std::string>()];
        for (auto csection : cfile.second) {
            struct _ConfigFileSection &section = file[csection.first.as<std::string>()];
            if (YAML::Node name = csection.second["name"]) section.name = name.as<std::string>();
            else section.name = csection.first.as<std::string>();
            for (auto coption : csection.second["options"]) {
                YAML::Node hint = coption.second["hint"];
                section.options[coption.first.as<std::string>()] = _ConfigFileSectionOption {
                    hint ? hint.as<std::string>() : ""
                };
                config->option_n++;
            }
        }
    }
    state->configs.push_back(config);
}

void Setup(NeyulwState *state) {
    state->run = true;
    state->done = [](NeyulwState*){};
    _SetupConfig(state, __ue4_yaml_data, __ue4_yaml_size);
    _SetupConfig(state, __ue5_0_yaml_data, __ue5_0_yaml_size);
    _SetupConfig(state, __ue5_5_yaml_data, __ue5_5_yaml_size);
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        NYW_LOG("SDL Failure: %s\n", SDL_GetError());
        return -1;
    }
    NeyulwState state;
    Setup(&state);

    float nyw_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_Window *nyw_window = SDL_CreateWindow("Neyulw", 1, 1, SDL_WINDOW_RESIZABLE|SDL_WINDOW_HIGH_PIXEL_DENSITY);
    SDL_MaximizeWindow(nyw_window);
    SDL_Renderer *nyw_renderer = SDL_CreateRenderer(nyw_window, 0);
    SDL_SetRenderVSync(nyw_renderer, -1);
    SDL_SetRenderScale(nyw_renderer, nyw_scale, nyw_scale);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &im_IO = ImGui::GetIO();
    im_IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    im_IO.IniFilename = 0;
    im_IO.LogFilename = 0;
    ImFontConfig nyw_font_reg_cfg;
    nyw_font_reg_cfg.FontDataOwnedByAtlas = false;
    ImFontConfig nyw_font_bold_cfg = nyw_font_reg_cfg;
    im_IO.Fonts->AddFontFromMemoryTTF((void*)__IosevkaFixed_Regular_ttf_data, (int)__IosevkaFixed_Regular_ttf_size, 16.0, &nyw_font_reg_cfg);
    state.bold = im_IO.Fonts->AddFontFromMemoryTTF((void*)__IosevkaFixed_Bold_ttf_data, (int)__IosevkaFixed_Bold_ttf_size, 20.0, &nyw_font_bold_cfg);

    ImGuiStyle &nyw_style = ImGui::GetStyle();
    nyw_style.ScaleAllSizes(nyw_scale);
    nyw_style.FontScaleDpi = nyw_scale;
    NeyulwStyle(nyw_style);

    ImGui_ImplSDL3_InitForSDLRenderer(nyw_window, nyw_renderer);
    ImGui_ImplSDLRenderer3_Init(nyw_renderer);
    
    WInitSetup(&state);

    while (state.run) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            ImGui_ImplSDL3_ProcessEvent(&ev);
            switch (ev.type) {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                case SDL_EVENT_QUIT:
                    state.run = false;
            }
        }
        SDL_WindowFlags wf = SDL_GetWindowFlags(nyw_window);
        if (wf&SDL_WINDOW_HIDDEN || wf&SDL_WINDOW_OCCLUDED) SDL_Delay(200);
        SDL_GetRenderOutputSize(nyw_renderer, &state.width, &state.height);
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        state.ui(&state);
        ImGui::Render();
        SDL_SetRenderScale(nyw_renderer, im_IO.DisplayFramebufferScale.x, im_IO.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(nyw_renderer, 0, 0, 0, 255);
        SDL_RenderClear(nyw_renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), nyw_renderer);
        SDL_RenderPresent(nyw_renderer);
    }

    state.done(&state);
    state.bold = NULL;

    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(nyw_renderer);
    SDL_DestroyWindow(nyw_window);
    SDL_Quit();
    return 0;
}
