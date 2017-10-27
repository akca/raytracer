#ifndef SPHERE_H__
#define SPHERE_H__

#include "vector3d.h"
#include "ray.h"
#include "object.h"

class Triangle : public Object {
	public:
		using parser::Vec3f;
		
		Vec3f v1;
		Vec3f v2;
		Vec3f v3;

		Triangle (Vec3f vt1, Vec3f vt2, Vec3f vt3) {
			v1 = vt1;
			v2 = vt2;
			v3 = vt3;
		}

		bool intersects(const Vector3D &origin, const Vector3D &direction, float &t) const {
		
			float detA = det33(v1.x - v2.x, v1.x - v3.x, direction.x, 
							   v1.y - v2.y, v1.y - v3.y, direction.y, 
							   v1.z - v2.z, v1.z - v3.z, direction.z
							   );
							   
			// TODO if detA == 0
			
			float beta = det33(v1.x - origin.x, v1.x - v3.x, direction.x, 
							   v1.y - origin.y, v1.y - v3.y, direction.y, 
							   v1.z - origin.z, v1.z - v3.z, direction.z
							   ) / detA;
							   
			float gama = det33(v1.x - v2.x, v1.x - origin.x, direction.x, 
							   v1.y - v2.y, v1.y - origin.y, direction.y, 
							   v1.z - v2.z, v1.z - origin.z, direction.z
							   ) / detA;
							   
			t          = det33(v1.x - v2.x, v1.x - v3.x, v1.x - origin.x, 
							   v1.y - v2.y, v1.y - v3.y, v1.y - origin.y, 
							   v1.z - v2.z, v1.z - v3.z, v1.y - origin.y
							   ) / detA;
			
			
			// TODO FIXME Tmin Tmax should be checked!
			
			if (beta + gama <= 1 && 0 <= beta && 0 <= gama) {
				return true;
			}
			else {
				return false;
			}
		}

};

#endif

