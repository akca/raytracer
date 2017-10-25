#ifndef OBJECT_H__
#define OBJECT_H__

#include "parser.h"

class Object 
{
	public: 
		Object() : color(255, 255, 255) {} 
		virtual ~Object() {} 

		virtual bool intersects(const Vector3D &, const Vector3D &, float &) const = 0; 
		virtual void getSurfaceData(const Vector3D &, Vector3D &) const = 0; 
		
		Vector3D color; // TODO change data type!
};

bool quadraticSolve(const float &a, const float &b, const float &c, float &t0, float &t1)
{ 
	float delta = b * b - (4 * a * c);
	//std::cout << delta << std::endl;
	
	if (delta < 0) {
		return false; //no soln
	}
	else {
		float q = (b > 0) ? 
			(b + sqrt(delta)) * -0.5 : 
			(b - sqrt(delta)) * -0.5; 
		t0 = q / a; 
		t1 = c / q;
    }
 
    return true; 
} 

#endif
