#pragma once

#include <map>
#include <string>
#include <vector>

std::map<std::string, std::vector<std::string>> GetOpt(int argc, char* argv[], std::map<std::string, int> opts_nvals);