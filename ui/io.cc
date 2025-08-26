#include <filesystem>
#include "io.hh"
#include "ini.hh"

void LoadProgramConfig(Config *config, std::unordered_map<std::string, std::string> vars, UserConfig *uconf) {
    for (auto& target : config->targets) {
        std::string path(target.second);
        for (auto& var : vars) {
            std::string pattern = "${" + std::string(var.first) + "}";
            size_t index = 0, len = pattern.length();
            while ((index = path.find(pattern)) != std::string::npos) {
                path.replace(index, len, var.second);
            }
            config->file_map[target.first] = path.c_str();
        }
        try {
            Ini data = Ini::FromFile(path.c_str());
            std::map<std::string, std::map<std::string, std::string>> ufile;
            for (auto section : *data.Sections()) {
                std::string section_id = section.first;
                std::map<std::string, std::string> usection;
                for (auto option : section.second) {
                    usection[option.first] = std::string(option.second);
                }
                ufile[section_id] = usection;
            }
            uconf->options[target.first] = ufile;
        } catch (...) {
            NYW_LOG("Failed to parse %s\n", path.c_str());
            uconf->options[target.first];
        }
    }
}

void SaveProgramConfig(Config *config, UserConfig *uconf) {
    for (auto& file : uconf->options) {
        const char *file_id = file.first.c_str();
        const char *path = config->file_map[file_id].c_str();
        Ini data;
        for (auto& section : file.second) {
            std::string section_id = section.first;
            for (auto& option : section.second) {
                if (!option.second.empty()) data[section_id][option.first] = option.second.c_str();
            }
        }
        try {
            std::filesystem::path p(path);
            if (!std::filesystem::exists(p)) {
                std::filesystem::create_directories(p.parent_path());
            }
            data.SaveTo(path);
        } catch (...) {
            NYW_LOG("Failed to save %s\n", file_id);
        }
    }
}
