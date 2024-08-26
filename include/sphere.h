#ifndef SPHERE_H
#define SPHERE_H

#include "rtweekend.h"
#include "hittable.h"

class sphere : public hittable {
  public:
    sphere(const point3& center, double radius) : center(center), radius(std::fmax(0,radius)) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // This was a derived formula to calculate whether or not the sphere was hit
        vec3 oc = center - r.origin(); // The ray from the sphere center to the ray origin
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius*radius;

        auto discriminant = h*h - a*c;
        if (discriminant < 0)
            return false; // Return if there are no real roots

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a; // Calculate root for minus sign
        if (!ray_t.surrounds(root)) { // If this root falls outside the range set, try the plus sign
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root)) // If both fall outside the range, fail it
                return false;
        }

        // Store the valid root that proves the intersection happened.
        // This is the t value in a ray that defines a distance from
        // the origin of the ray
        rec.t = root;

        // Calculate the point3 using the calculated t value
        rec.p = r.at(rec.t);

        // Divide by radius here because we are subtracting to get a 
        // vector that goes from the center of the sphere to the
        // very edge. To get a normal vector, its length has to be 1,
        // so we divide the vector by the length, which is the radius
        // of the sphere.
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

  private:
    point3 center;
    double radius;
};

#endif