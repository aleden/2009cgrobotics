#pragma once

template <typename T>
class Coords2D
{
public:
	T x, y;
	
	Coords2D(const T& x = static_cast<T>(0),
			 const T& y = static_cast<T>(0))
	{
		this->x = x;
		this->y = y;
	}
	~Coords2D(){}
};
typedef Coords2D<float> Coords2Df;
