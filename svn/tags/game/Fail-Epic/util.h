#pragma once

template <typename T>
T deadzone(const T& val, const T& target, const T& range)
{ return ( val <= (target + range) && val >= (target - range) ) ? target : val; }

template <typename T>
T abs(const T& x)
{ return x < static_cast<T>(0) ? -x : x; }

template <typename T>
T pow(const T& number, unsigned int power)
{
	T r = static_cast<T>(1);
	for (unsigned int i = 0; i < power; i++)
		r *= number;
	return r;
}

template <typename T>
bool isBitSet(const T& value, unsigned char bit)
{ return (value & pow(2, bit)) != 0; }

template <typename T>
void flipBit(T& value, unsigned char bit)
{ value = value ^ 1 << bit; }

