#ifndef HITTABLE_H
#define HITTABLE_H

class hit_record {
  public:
    point3 p;
    vec3 normal;
    double t; // How far along the ray did the intersection occur
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // Sets the hit record normal vector.
        // NOTE: the parameter `outward_normal` is assumed to have unit length.

        // Figure out which side of an object a ray is hitting.
        // This is because the light could be coming from either inside
        // or outside the object.
        // If the dot product of the normal and the ray is >= 0, 
        // the ray is at a 90 degree angle or less with the normal vector.
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
  public:
    virtual ~hittable() = default;

    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
};

#endif