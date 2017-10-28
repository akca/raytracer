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
		
		void getSurfaceData(const Vector3D &Phit, Vector3D &Nhit) const
		{

		}

};

#endif

