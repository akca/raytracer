#ifndef VECTOR3D_H__
#define VECTOR3D_H__

#include <iostream>
#include <cmath>

class Vector3D {
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

		Vector3D() {
			x = 0;
			y = 0;
			z = 0;
		}

		Vector3D(double i, double j, double k) {
			x = i;
			y = j;
			z = k;
		}
		
		Vector3D operator- (Vector3D v) {
			return Vector3D(x-v.x, y-v.y, z-v.z);
		}
		
		// Dot product
		double operator* (Vector3D v) { 
			return x * v.x + y * v.y + z * v.z;
		}
};

#endif

