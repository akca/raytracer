#ifndef SPHERE_H__
#define SPHERE_H__

#include "vector3d.h"
#include "object.h"

class Sphere : public Object {
	public:
		Vector3D center; // center of the sphere
		float r;
		float r2;
		int material_id;
		
		Sphere (Vector3D &c, float radius, int material) {
		
			center = c;
			r = radius;
			r2 = r * r;
			material_id = material;
		}

		bool intersects(const Vector3D &origin, const Vector3D &direction, float &t) const {

			Vector3D L = origin - center;

			float b = 2 * direction.dotProduct(L);
			float c = L.dotProduct(L) - r2;
			//std::cout << "ORG: " << origin.x << " " << origin.y << " " << origin.z << std::endl;
			//std::cout << "EHEH: " << 1 << " " << b << " " << c << std::endl;
			float t0, t1; // these will become roots
			if (!quadraticSolve(1, b, c, t0, t1)) {
				return false;
			}

			if (t0 > t1) std::swap(t0, t1);
			//std::cout << "ilginc: " << t0 << " " << t1 << std::endl;

			if (t1 < 0) return false; // both roots are negative, no intersection

			if (t0 < 0) {
				t = t1; // if t0 is negative, t1 should be the visible param.
			} else {
				t = t0;
			}

			return true;
		}

		void getSurfaceData(const Vector3D &Phit, Vector3D &Nhit) const
		{
			Nhit = Phit - center;
			Nhit.normalize();
		}
};

#endif

