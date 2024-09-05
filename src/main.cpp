#include "math.h"
#include "physics.h"
#include "io.h"
#include "estimate_power.h"
#include "getopt.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>


int main(int argc, char* argv[])
{
    std::string usage = "Usage: \n\
	-f <.fit>, will use interpidsport parsing service to get json file and continue\n\
	-j <.json>, read parsed json file\n\
    -p parameters(total_weight_kg cda rolling_coeff), divide by space\n\n";

    std::map<std::string, int> opts_nvals =
    {
        {"-f", 1},
        {"-j", 1},
        {"-p", 3},
        {"-o", 1},
        {"-h", 0},
    };

    std::map<std::string, std::vector<std::string>> opts_vals = GetOpt(argc, argv, opts_nvals);
    std::string fit_file;
    std::filesystem::path input_path;
    std::filesystem::path json_path;
    std::filesystem::path output_path;

    float total_mass = 80.0;
    float cda = 0.32;
    float rolling_coeff = 0.005;
    bool need_decode = false;

    for (auto& opt_val : opts_vals)
    {
        std::string opt = opt_val.first;
        std::vector<std::string> val = opt_val.second;

        if (opt == "-f")
        {
            input_path = std::filesystem::path(val[0]);
            json_path = input_path;
            output_path = input_path;
            json_path.replace_extension(".json");
            output_path.replace_extension(".csv");
            InterpidsportFitService(input_path.string(), json_path.string());
        }
        else if (opt == "-j")
        {
            input_path = std::filesystem::path(val[0]);
            json_path = input_path;
            output_path = input_path;
            output_path.replace_extension(".csv");
        }
        else if (opt == "-p")
        {
            total_mass = std::stof(val[0]);
            cda = std::stof(val[1]);
            rolling_coeff = std::stof(val[2]);
        }
        else if (opt == "-o")
        {
            output_path = std::filesystem::path(val[0]);
        }
        else if (opt == "-h")
        {
            std::cout << usage;
            return 0;
        }
    }

    std::cout << "input from " << json_path.string() << std::endl;
    FitData data = parseJsonData(json_path.string());
    CyclistParams cp(total_mass, cda, rolling_coeff);
    EstimatePower(data, cp);

    std::ofstream output(output_path);
    std::cout << "output to " << output_path.string() << std::endl;
    output << "timestamp,distance_m,altitude_m,gradient_percent,speed_kph,power_w,estimated_power_w\n";
    for (size_t i = 0; i < data.distance_m.size(); ++i)
    {
        output << data.timestamp[i] << ","
            << data.distance_m[i] << ","
            << data.altitude_m[i] << ","
            << data.gradient_percent[i] << ","
            << data.speed_mps[i] * 3.6 << ","
            << data.power_w[i] << ","
            << data.estimated_power_w[i] << "\n";
    }
}
