#include "physics.h"
#include "math.h"

#include <vector>
#include <cmath>

/**
 * @brief 获取空气密度
 *
 * 根据给定的温度、高度和湿度，计算并返回空气的密度。
 *
 * @return 空气密度，单位为 kg/m^3
 */
float EnvironmentParams::GetAirDensity()
{
    const float R_spec = 287.05;
    const float gravity_constant = 9.80;
    const float molar_mass = 0.0289644;
    const float gas_constant = 8.31446;

    float temperature_K = temperature_c + 273.15;
    float P = 101325 * std::exp(-gravity_constant * molar_mass * altitude_m / (gas_constant * temperature_K));
    float P_sat_wat = 610.78 * std::exp((17.27 * temperature_c) / temperature_K);
    float P_wat = humidity_percent * P_sat_wat / 100.0;
    float P_dry = P - P_wat;

    air_density_kgm3 = P_dry / (R_spec * temperature_K);
    return air_density_kgm3;
}

/**
 * @brief 计算骑行功率
 *
 * 根据给定的骑行者参数、环境参数和速度，计算骑行者的功率。
 *
 * @param cp 骑行者参数引用
 * @param ep 环境参数引用
 * @param speed_kph 速度（千米/小时）
 *
 * @return 骑行功率（瓦特）
 */
float PowerEval(CyclistParams& cp, EnvironmentParams& ep, float speed_kph)
{
    float speed_mps = speed_kph / 3.6;
    float headwind_mps = ep.headwind_kph / 3.6;
    float airspeed_mps = speed_mps + headwind_mps;
    float air_density_kgm3 = ep.GetAirDensity();

    float rolling_drag = cp.rolling_coeff * cp.total_mass_kg * 9.80;
    float gradient_drag = ep.gradient_percent / 100.0 * cp.total_mass_kg * 9.80;
    float aero_drag = 0.50 * air_density_kgm3 * cp.cda * airspeed_mps * airspeed_mps;

    float power_w = (rolling_drag + gradient_drag + aero_drag) * speed_mps * (1 + cp.drivetrain_penalty);

    return (power_w < 0.0) ? 0.0 : power_w;
}

float PowerEvalByDynamics(CyclistParams& cp, EnvironmentParams& ep, EnvironmentParams& before_ep, float speed_kph, float before_speed_kph, float dt)
{
    float acceleration_mps2 = ((speed_kph - before_speed_kph) / 3.6) / dt;

    float speed_mps = (speed_kph + before_speed_kph) / 7.2;
    float headwind_mps = ep.headwind_kph / 3.6;
    float airspeed_mps = speed_mps + headwind_mps;
    float air_density_kgm3 = ep.GetAirDensity();

    float rolling_drag = cp.rolling_coeff * cp.total_mass_kg * 9.80;
    float gradient_drag = ep.gradient_percent / 100.0 * cp.total_mass_kg * 9.80;
    float aero_drag = 0.50 * air_density_kgm3 * cp.cda * airspeed_mps * airspeed_mps;

    float power_w = (rolling_drag + gradient_drag + aero_drag) * speed_mps * (1 + cp.drivetrain_penalty);

    return (power_w < 0.0) ? 0.0 : power_w;
}

/**
 * @brief 计算速度
 *
 * 根据给定的自行车参数、环境参数和功率值，计算对应的速度。
 *
 * @param cp 自行车参数引用
 * @param ep 环境参数引用
 * @param power_w 功率值（单位：瓦特）
 *
 * @return 计算得到的速度（单位：千米/小时）
 */
float SpeedEval(CyclistParams& cp, EnvironmentParams& ep, float power_w)
{

    //          aero_power      =  (0.50 * wp.air_density_kgm3 * cp.cda) * (speed_mps + headwind_mps)^2 * speed_mps
    //                 p_a      =  k * (x+h)^2 * x = k*x^3 + 2k*h*x^2 + k*h^2*x
    // rolling + gradient power =  ( rolling_drag + gradient_drag ) * speed_mps
    //                p_rg      =  m * x
    //                   p      =  (k)*x^3 + (2k*h)*x^2 + (k*h^2 + m)*x

    // Solve cubic func:
    //            (k)*x^3 + (2k*h)*x^2 + (k*h^2 + m)*x - p = 0

    float air_density_kgm3 = ep.GetAirDensity();

    float k = 0.50 * air_density_kgm3 * cp.cda;
    float h = ep.headwind_kph / 3.6;
    float m = cp.rolling_coeff * cp.total_mass_kg * 9.80 + ep.gradient_percent / 100.0 * cp.total_mass_kg * 9.80;

    float a = k;
    float b = 2 * k * h;
    float c = k * h * h + m;
    float d = -power_w / (1 + cp.drivetrain_penalty);

    std::vector<float> roots = SolveCubic(a, b, c, d);
    float speed_mps = GetUniquePositiveRoot(roots);

    return speed_mps * 3.6;
}