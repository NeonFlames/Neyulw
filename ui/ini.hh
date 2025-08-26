#pragma once

#include <map>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <regex>

class Ini {
private:
    std::map<std::string, std::map<std::string, std::string>> data;
public:
    const char *newline;

    Ini() {
    }

    static Ini FromText(const char *text, const size_t size) {
        Ini ini;
        std::map<std::string, std::string> *section = &ini[""];
        std::string line;
        std::istringstream data(std::string(text, size));
        std::regex rsection(R"( *?\[(.*?)\].*)", std::regex_constants::ECMAScript);
        std::regex roption(R"((.+?) *?= *(.+?) *)", std::regex_constants::ECMAScript);
        std::smatch m;
        while (std::getline(data, line)) {
            if (std::regex_match(line, m, rsection)) {
                std::string id = m[1].str();
                section = &ini[id];
            } else {
                if (std::regex_match(line, m, roption)) {
                    std::string _option = m[1].str(), _value = m[2].str();
                    const char *option = _option.c_str(), *value = _value.c_str();
                    section->insert_or_assign(option, value);
                }
            }
        }
        return ini;
    }

    static Ini FromFile(const char *file) {
        FILE *f = fopen(file, "r");
        if (!f) throw 0;
        fseek(f, 0, SEEK_END);
        size_t size = ftell(f);
        char *data = new char[size+1];
        rewind(f);
        fread(data, sizeof(char), size, f);
        fclose(f);
        Ini ret = FromText(data, size);
        delete []data;
        return ret;
    }

    constexpr std::map<std::string, std::map<std::string, std::string>>* Sections() {
        return &data;
    }

    void SaveTo(const char *path) {
        std::ofstream file(path);
        for (auto& section : data) {
            file << '[' << section.first << ']' << std::endl;
            for (auto& option : section.second) {
                file << option.first << " = " << option.second << std::endl;
            }
        }
        file.close();
    }

    std::map<std::string, std::string>& operator[](std::string &section) {
        return data[section];
    }
    std::map<std::string, std::string>& operator[](const char *section) {
        return data[section];
    }
};
