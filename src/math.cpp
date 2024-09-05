#include <vector>
#include <algorithm>
#include <cmath>
#include <stdexcept>

/**
 * @brief 获取唯一的正根
 *
 * 从给定的根集合中筛选出唯一的正根，并返回该正根的值。
 * 如果不存在正根或存在多个不同的正根，则抛出异常。
 *
 * @param roots 根集合
 * @param tolerance 容忍度，用于判断多个根是否相同，默认为 1e-6f
 *
 * @return 唯一的正根
 * @throw std::runtime_error 如果不存在正根或存在多个不同的正根
 */
float GetUniquePositiveRoot(const std::vector<float>& roots, float tolerance = 1e-6f)
{
    std::vector<float> positiveRoots;

    for (float root : roots) {
        if (root > 0) {
            positiveRoots.push_back(root);
        }
    }

    if (positiveRoots.empty()) {
        throw std::runtime_error("No positive real roots found");
    }

    std::sort(positiveRoots.begin(), positiveRoots.end());

    for (size_t i = 1; i < positiveRoots.size(); ++i) {
        if (std::abs(positiveRoots[i] - positiveRoots[0]) > tolerance) {
            throw std::runtime_error("Multiple distinct positive real roots found");
        }
    }

    return positiveRoots[0];
}

/**
 * @brief 解三次方程
 *
 * 根据给定的系数 a、b、c和d，求解三次方程ax^3 + bx^2 + cx + d = 0的根，并将根存储在向量中返回。
 *
 * @param a 三次项系数
 * @param b 二次项系数
 * @param c 一次项系数
 * @param d 常数项
 *
 * @return 存储三次方程根的向量
 */
std::vector<float> SolveCubic(float a, float b, float c, float d) {
    const float eps = 1e-6f;
    std::vector<float> roots;

    if (std::abs(a) < eps) {
        if (std::abs(b) < eps) {
            if (std::abs(c) < eps) {
                return roots;
            }
            roots.push_back(-d / c);
            return roots;
        }
        float disc = c * c - 4 * b * d;
        if (disc > eps) {
            roots.push_back((-c + std::sqrt(disc)) / (2 * b));
            roots.push_back((-c - std::sqrt(disc)) / (2 * b));
        }
        else if (std::abs(disc) < eps) {
            roots.push_back(-c / (2 * b));
        }
        return roots;
    }

    b /= a; c /= a; d /= a;

    float p = c - b * b / 3;
    float q = d + (2 * b * b * b - 9 * b * c) / 27;
    float D = q * q / 4 + p * p * p / 27;

    if (D > eps) {
        float u = std::cbrt(-q / 2 + std::sqrt(D));
        float v = std::cbrt(-q / 2 - std::sqrt(D));
        roots.push_back(u + v - b / 3);
    }
    else if (D < -eps) {
        float phi = std::acos(-q / 2 / std::sqrt(-p * p * p / 27));
        float r = 2 * std::sqrt(-p / 3);
        for (int k = 0; k < 3; k++) {
            roots.push_back(r * std::cos((phi + 2 * M_PI * k) / 3) - b / 3);
        }
    }
    else {
        float r = 3 * q / (2 * p);
        roots.push_back(-r - b / 3);
        roots.push_back(r / 2 - b / 3);
        roots.push_back(r / 2 - b / 3);
    }

    return roots;
}