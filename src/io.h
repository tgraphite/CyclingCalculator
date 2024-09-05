#pragma once

#include <vector>
#include <string>

struct FitData
{
    std::vector<int> timestamp;
    std::vector<float> altitude_m;
    std::vector<float> distance_m;
    std::vector<float> gradient_percent;
    std::vector<int> temperature_c;

    std::vector<float> speed_mps;

    std::vector<int> power_w;
    std::vector<int> cadence;
    std::vector<int> heartrate;

    std::vector<float> estimated_power_w;
};

std::vector<std::string> splitByDelimiter(const std::string& str, const std::string& delim);
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
int InterpidsportFitService(const std::string& path, const std::string& output_path);
FitData parseJsonData(const std::string& filename);