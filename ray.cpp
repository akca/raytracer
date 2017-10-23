#include <iostream>
#include <cmath>
#include <cmath>

class Ray {
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
