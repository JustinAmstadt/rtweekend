#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable.h"

class material {
  public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const {
        return false;
    }
};

class lambertian : public material {
  public:
    // Albedo means whiteness
    lambertian(const color& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {

        // Simulate Lambertian Reflection.
        // We want to make the reflected ray more likely to be near the normal vector.
        // so first, we add a unit vector to the normal vector which is also a unit vector.
        // The normal vector is a unit length away from the surface, so now we add a 
        // random unit vector to get a point on the sphere around the normal vector.
        // From there, we get a vector that we can draw from the hit point to the point 
        // on the unit sphere around the normal vector.
        // By doing this, for a dot product of 0.5 - 1, we get a range
        // of 0 - 60 degrees from the normal vector. From 0.0 to 0.5, we get a range
        // of 60 - 90 degrees from the normal vector. 
        // Since the Lambertian Reflection model needs to be more likely to scatter
        // near the surface normal, we accomplish this with this model
        auto scatter_direction = rec.normal + random_unit_vector();

        // Catch degenerate scatter direction
        if (scatter_direction.near_zero())
            scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo;
        return true;
    }

  private:
    color albedo;
};

class metal : public material {
  public:
    metal(const color& albedo) : albedo(albedo) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        // Metal will always reflect the light across the normal
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return true;
    }

  private:
    color albedo;
};

#endif