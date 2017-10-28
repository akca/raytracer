#ifndef MESH_H__
#define MESH_H__
#include <limits>
#include "vector3d.h"
#include "object.h"
#include "face.h"

const float FLOAT_MAX = std::numeric_limits<float>::max();

class Mesh : public Object {
	public:
		
		std::vector<Face> faces;

		Mesh (std::vector<Face> &f, int m) {
			faces = f;
			material_id = m;
		}

		bool intersects(const Vector3D &origin, const Vector3D &direction, float &t) const {
		
			float tmin = FLOAT_MAX;
			
			for (auto &f : faces) {
				if (f.intersects(origin, direction, t) && t<tmin) {
					tmin = t;
				}
			}
			
			if (tmin < FLOAT_MAX)
				return true;
			else
				return false;
		}
		
		void getSurfaceData(const Vector3D &Phit, Vector3D &Nhit) const
		{

		}

};

#endif

