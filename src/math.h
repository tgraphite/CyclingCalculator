#pragma once

#include <cmath>
#include <vector>
#include <complex>

float GetUniquePositiveRoot(const std::vector<float>& roots, float tolerance = 1e-6f);
std::vector<float> SolveCubic(float a, float b, float c, float d);