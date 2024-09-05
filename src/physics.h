#pragma once

#include <vector>

// constructor 1: total_mass_kg, cda, rolling_coeff
// constructor 2: total_mass_kg, cda, rolling_coeff, drivetrain_penalty
struct CyclistParams
{
    float total_mass_kg;
    float cda;
    float rolling_coeff;
    float drivetrain_penalty = 0.02f;

    CyclistParams(float total_mass_kg, float cda, float rolling_coeff) : total_mass_kg(total_mass_kg), cda(cda), rolling_coeff(rolling_coeff) {}
    CyclistParams(float total_mass_kg, float cda, float rolling_coeff, float penalty) : total_mass_kg(total_mass_kg), cda(cda), rolling_coeff(rolling_coeff), drivetrain_penalty(penalty) {}
    ~CyclistParams() {}
};

// constructor 1: gradient_percent, headwind_kph, altitude_m, humidity_percent, temperature_c
// constructor 2: gradient_percent, altitude_m, temperature_c
struct EnvironmentParams
{
    float gradient_percent = 0.0f;

    float headwind_kph = 0.0f;
    float altitude_m = 0.0f;
    float humidity_percent = 50.0f;
    float temperature_c = 25.0f;

    float air_density_kgm3 = 1.22f;

    EnvironmentParams(float g, float hw, float alt, float hum, float temp)
        : gradient_percent(g), headwind_kph(hw), altitude_m(alt), humidity_percent(hum), temperature_c(temp) {}
    EnvironmentParams(float g, float alt, float temp) : gradient_percent(g), altitude_m(alt), temperature_c(temp) {}

    ~EnvironmentParams() {}

    float GetAirDensity();
};


float PowerEval(CyclistParams& cp, EnvironmentParams& ep, float speed_kph);
float PowerEvalByDynamics(CyclistParams& cp, EnvironmentParams& ep, EnvironmentParams& before_ep, float speed_kph, float before_speed_kph, float dt);
float SpeedEval(CyclistParams& cp, EnvironmentParams& ep, float power_watt);