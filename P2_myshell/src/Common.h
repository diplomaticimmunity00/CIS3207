#pragma once

#include <iostream>
#include <vector>

extern std::string target_command;
extern bool running_script;
extern bool reading_from_file;

#include "Shell.h"

extern int stdout_fd;
extern int cin_fd;
extern Shell myshell;
