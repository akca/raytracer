#ifndef OBJECT_H__
#define OBJECT_H__

#include "vector3d.h"
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

bool quadraticSolve(const float &a, const float &b, const float &c, float &t0, float &t1);
float det33 (float a, float b, float c, float d, float e, float f,
		float g, float h, float i);
#endif

