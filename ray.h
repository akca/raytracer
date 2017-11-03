#ifndef RAY_H__
#define RAY_H__

#include <iostream>
#include <cmath>
#include "vector3d.h"

// r(t) = o + td

class Ray {
	public:
		Vector3D o; // origin
		Vector3D d; // direction

		Ray() {
			o = Vector3D();
			d = Vector3D();
		}

		Ray(Vector3D start, Vector3D direction) {
			o = start;
			d = direction;
		}
};

#endif

