#ifndef TRIANGLE_H__
#define TRIANGLE_H__

#include "vector3d.h"
#include "object.h"
#include "face.h"

class Triangle : public Object {
	public:
		
		Face face;

		Triangle (Face &f, int m) {
			face = f;
			material_id = m;
		}

		bool intersects(const Vector3D &origin, const Vector3D &direction, float &t) const {
			return face.intersects(origin, direction, t);
		}
		
		Vector3D& getNormalAt(const Vector3D &hitPoint) const
		{

		}

};

#endif

