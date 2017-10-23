#ifndef VECTOR_H__
#define VECTOR_H__

#include <iostream>
#include <cmath>

class Vector {
public:
	double x;
	double y;
	double z;
	
	double getLength() const {
		return sqrt(x*x + y*y + z*z);
	}
	
	void normalize() {
		
		double length = getLength();
				
		x /= length;
		y /= length;
		z /= length;
	}
	
	Vector() {
		x = 0;
		y = 0;
		z = 0;
	}
};

#endif
