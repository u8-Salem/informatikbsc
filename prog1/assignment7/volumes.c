#include "base.h"

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

typedef enum typetag_e {
	/*TypeTag sorgt dafür, dass man später die verschiedenen 
	Körper den verschiedenen strukturen im Body zuordnen kann.*/
    CYLINDER,
    CUBOID,
    SPHERE,
} TypeTag;

typedef struct cylinder_s {
    double r, h;
} Cylinder;

typedef struct cuboid_s {
    double a, b, c;
} Cuboid;

typedef struct sphere_s {
    double r;
} Sphere;

typedef struct body_s {
    // todo
	 TypeTag tag;
    union {
        Cylinder cylinder;
        Cuboid cuboid;
        Sphere sphere;
    } types;
} Body;

Body make_cylinder(double r, double h) {
    // todo
	Body body;
	body.tag = CYLINDER;
	body.types.cylinder.r = r;
	body.types.cylinder.h = h;
	return body;
}

Body make_cuboid(double a, double b, double c) {
    // todo
	Body body;
	body.tag = CUBOID;
	body.types.cuboid.a = a;
	body.types.cuboid.b = b;
	body.types.cuboid.c = c;
	return body;
}

Body make_sphere(double r) {
    // todo
	Body body;
	body.tag = SPHERE;
	body.types.sphere.r = r;
	return body;
}

double volume(Body body) {
    // todo
	switch(body.tag){
		case CYLINDER:
			return M_PI * body.types.cylinder.r * body.types.cylinder.r * body.types.cylinder.h;
		case CUBOID:
			return body.types.cuboid.a * body.types.cuboid.b * body.types.cuboid.c;
		case SPHERE:
			return 4. / 3. * M_PI * body.types.sphere.r * body.types.sphere.r * body.types.sphere.r;
	}
    return -1;
}

void volume_test(void) {
    test_within_d(volume(make_sphere(2)), 4 /3.0 * M_PI * 2 * 2 * 2, 1e-6);
    test_within_d(volume(make_cuboid(2, 3, 4)), 2 * 3 * 4, 1e-6);
    test_within_d(volume(make_cylinder(3, 4)), 4 * M_PI * 3 * 3 , 1e-6);
}

int main(void) {
    volume_test();
    return 0;
}
