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
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        // Metal will always reflect the light across the normal
        vec3 reflected = unit_vector(reflect(r_in.direction(), rec.normal)) + (fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

  private:
    color albedo;
    double fuzz;
};

class dielectric : public material {
  public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered)
    const override {
        attenuation = color(1.0, 1.0, 1.0);
        // Divide by 1 for front_face == true because the ray is going through
        // air (IOR of 1) to the glass sphere. When exiting, use the IOR of the
        // glass sphere.
        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        // Reflect if cannot_refract == false due to Snell's law or by chance
        // due to the percentage of light the material reflects based on IOR
        if (cannot_refract || reflectance(cos_theta, ri) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri);

        scattered = ray(rec.p, direction);
        return true;
    }

  private:
    // Refractive index in vacuum or air, or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;

    // Will give an approximation of the percentage of light that will reflect
    // given the cosine of an angle and the refraction index of the material that
    // is being looked at.
    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }
};

#endif