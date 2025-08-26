#pragma once
#include <cstdint>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include "imgui.h"

#define NYW_LOG(...) printf("[Neyulw] ");printf(__VA_ARGS__)

struct _ConfigVar {
    std::string name, type, hint;
};

struct _ConfigPresetSubset {
    std::string hint;
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::string>>> files;
};

struct _ConfigPreset {
    std::string hint;
    int id;
    std::unordered_map<std::string, std::unordered_map<std::string, std::unordered_map<std::string, std::string>>> files;
    std::map<std::string, struct _ConfigPresetSubset> subsets;
};

struct _ConfigFileSectionOption {
    std::string hint;
};

struct _ConfigFileSection {
    std::string name;
    std::map<std::string, struct _ConfigFileSectionOption> options;
};

typedef struct {
    std::string name;
    std::map<std::string, struct _ConfigVar> vars;
    std::map<std::string, std::string> targets;
    std::map<std::string, struct _ConfigPreset> presets;
    std::map<std::string, std::map<std::string, _ConfigFileSection>> files;
    size_t option_n;
    std::unordered_map<std::string, std::string> file_map;
} Config;

typedef struct {
    std::map<std::string, std::map<std::string, std::map<std::string, std::string>>> options;
} UserConfig;

typedef struct _NeyulwState {
    bool run;
    int width, height;
    std::vector<Config*> configs;
    void *ui_data;
    ImFont *bold;
    void (*ui)(struct _NeyulwState*);
    void (*done)(struct _NeyulwState*);
} NeyulwState;

struct WInitData {
    size_t config;
    std::unordered_map<std::string, std::string> vars;
    bool (*ready)(NeyulwState*);
};

struct _ConfigureDataOption {
    std::string file, section, option, hint, *value;
};

struct _ConfigureDataPreset {
    std::string name, hint;
    int id;
    size_t active, subset;
    std::vector<std::string> subsets;
};

struct ConfigureData {
    Config *config;
    UserConfig *uconf;
    bool presets_, modified, search_hints, search_strict;
    uint8_t sort;
    std::string search;
    std::vector<_ConfigureDataOption> options;
    std::vector<_ConfigureDataPreset> presets;
    std::vector<_ConfigureDataPreset*> presets_active;
    void (*update)(ConfigureData*);
    void (*apply_presets)(ConfigureData*);
    void (*reset_presets)(ConfigureData*);
    void (*update_presets)(ConfigureData*);
    void (*save)(NeyulwState*);
};
