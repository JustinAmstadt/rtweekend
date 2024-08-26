#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "rtweekend.h"
#include "hittable.h"

#include <vector>

class hittable_list : public hittable {
  public:
    // Has to inherit hittable. Right now, only the sphere class can be used for this
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
    }

    // The purpose of this function is to determine whether a given ray (r) 
    // intersects any object in the list of objects and to keep track of the 
    // closest intersection point if multiple intersections are found.
    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        // Loop through all objects to see whether a ray hit it or not
        for (const auto& object : objects) {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                
                // We know this is closer if there was another object already
                // because the hit function will check if the intersection point
                // is less than ray_tmin or greater than ray_tmax. If it is,
                // the function returns false
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};

#endif