#pragma once

#include <cmath>

// Wrap an angle to the range [-π, π]
inline float WrapAngle(float angle) noexcept
{
    // Ensure we're dealing with a normalized range
    constexpr float TWO_PI = 6.283185307179586f;
    const float result = std::fmod(angle, TWO_PI);

    // Adjust for negative angles to keep in [-π, π]
    if (result > 3.14159265358979323846f)
        return result - TWO_PI;
    else if (result < -3.14159265358979323846f)
        return result + TWO_PI;

    return result;
}

// Wrap an angle to a custom range [minVal, maxVal]
inline float WrapAngleRange(float angle, float minVal, float maxVal) noexcept
{
    const float width = maxVal - minVal;
    const float offsetValue = angle - minVal;

    // Wrap the offset to [0, width]
    const float wrappedOffset = std::fmod(offsetValue, width);

    // Handle negative values
    const float correctedOffset = wrappedOffset < 0.0f ? wrappedOffset + width : wrappedOffset;

    // Add back the minimum
    return correctedOffset + minVal;
}

// Clamp an angle to the range [minVal, maxVal]
inline float ClampAngle(float angle, float minVal, float maxVal) noexcept
{
    if (angle < minVal)
        return minVal;
    else if (angle > maxVal)
        return maxVal;

    return angle;
}
