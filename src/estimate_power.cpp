#include "physics.h"
#include "io.h"
#include "estimate_power.h"

#include <string>
#include <vector>

/**
 * @brief 根据Fit数据预测功率
 *
 * 根据给定的Fit数据和自行车手参数，预测每个数据点的功率。
 *
 * @param data Fit数据 (FitData)
 * @param cp 自行车手参数
 */
void EstimatePower(FitData& data, CyclistParams& cp)
{

    // RecalcGradient(data);
    std::vector<float> estimated_power_w(data.power_w.size(), 0.0f);
    for (size_t i = 0; i < data.power_w.size(); ++i)
    {
        if (data.distance_m[i] < 0)
        {
            data.estimated_power_w[i] = 0.0f;
            continue;
        }
        EnvironmentParams ep(data.gradient_percent[i], data.altitude_m[i], data.temperature_c[i]);
        estimated_power_w[i] = PowerEval(cp, ep, data.speed_mps[i] * 3.6);

        if (estimated_power_w[i] > 2000 and i > 0)
        {
            estimated_power_w[i] = estimated_power_w[i - 1];
        }
    }

    data.estimated_power_w = estimated_power_w;
}

void RecalcGradient(FitData& data)
{
    std::vector<float> recalced_gradient_percent(data.gradient_percent.size(), 0.0f);

    for (size_t i = 5; i < data.distance_m.size(); ++i)
    {
        if (data.altitude_m[i - 5] * data.altitude_m[i] == 0)
        {
            continue;
        }
        if (data.distance_m[i] - data.distance_m[i - 5] == 0)
        {
            continue;
        }
        else
        {
            recalced_gradient_percent[i] = (data.altitude_m[i] - data.altitude_m[i - 5]) / (data.distance_m[i] - data.distance_m[i - 5]) * 100;
        }
    }

    data.gradient_percent = recalced_gradient_percent;
}