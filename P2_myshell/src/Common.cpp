#include <vector>

#include "Common.h"
#include "Shell.h"

std::string target_command = "";
bool running_script = false;
bool reading_from_file = false;
int stdout_fd;
int cin_fd;
Shell myshell = Shell();
