#pragma once

inline float deadzone(float value, float range = .15f)
{ return (value >= -range && value <= range) ? 0.0f : value; }

inline double deadzone(double value, double range = .15)
{ return (value >= -range && value <= range) ? 0.0 : value; }
