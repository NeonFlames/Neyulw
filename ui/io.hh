#pragma once
#include "shared.hh"

void LoadProgramConfig(Config *config, std::unordered_map<std::string, std::string> vars, UserConfig *uconf);
void SaveProgramConfig(Config *config, UserConfig *uconf);
