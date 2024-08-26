#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "hittable.h"

class camera {
  public:
    double aspect_ratio = 16.0 / 9.0; // Width / Height
    int image_width = 400; // In pixels
    int samples_per_pixel = 10; // Count of random samples for each pixel
    int max_depth = 10; // Maximum number of ray bounces into scene

    void render(const hittable& world, std::string& out) {
        initialize();

        out += "P3\n" + std::to_string(image_width) + ' ' + std::to_string(image_height) + "\n255\n";

        for (int j = 0; j < image_height; j++) {
            std::cout << "\rScanlines remaining: " << (image_height - j) << ' ' << std::endl;
            for (int i = 0; i < image_width; i++) {
                color pixel_color(0, 0, 0);
                for(int sample = 0; sample < samples_per_pixel; sample++) {
                    ray r = get_ray(i, j); // Pick a range in a box around the original point to sample
                    pixel_color += ray_color(r, max_depth, world); // Add all samples into one color
                }
                write_color(out, pixel_samples_scale * pixel_color); // Divide the sum of colors by the total number of samples
            }
        }
        std::cout << "\rDone.                 \n";
    }

  private:
    int image_height; // Rendered image height
    double pixel_samples_scale; // Color scale factor for a sum of pixel samples
    point3 camera_center; // Camera center
    point3 pixel00_loc; // Location of pixel 0, 0
    vec3 pixel_delta_u; // Offset to pixel to the right
    vec3 pixel_delta_v; // Offset to pixel below

    void initialize() {
        // Calculate the image height, and ensure that it's at least 1.
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        // Multiply with final color to get an average of all samples used
        pixel_samples_scale = 1.0 / samples_per_pixel;

        double focal_length = 1.0; // Distance between camera and viewport
        double viewport_height = 2.0;
        double viewport_width = viewport_height * (double(image_width)/image_height);
        camera_center = point3(0, 0, 0);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = vec3(viewport_width, 0, 0);
        vec3 viewport_v = vec3(0, -viewport_height, 0);

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        vec3 viewport_upper_left = camera_center
                                - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2; // This finds the very top corner
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v); // This finds the center of pixel 00
    }

        ray get_ray(int i, int j) const {
        // Construct a camera ray originating from the origin and directed at randomly sampled
        // point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = camera_center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    // Doesn't have to sample from the center of the surrounding pixels
    vec3 sample_square() const {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0) {
            return color(0,0,0);
        }

        hit_record rec;
        // interval low of 0.001 is to solve shadow acne
        if(world.hit(r, interval(0.001, infinity), rec)) {
            // The ray has hit an object in the scene

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
            vec3 direction = rec.normal + random_unit_vector();

            // Recursively trace a new ray from the hit point into the random direction
            // Attenuate (reduce) the color by 0.5 to simulate diffuse reflection

            // Eventually take on the background color, but darker because of
            // the 0.5 multiplication
            return 0.1 * ray_color(ray(rec.p, direction), depth - 1, world);
        }

        // Make a unit vector with range of [-1, 1]
        vec3 unit_direction = unit_vector(r.direction());

        // Add 1 to y component to get a range of [0, 2] 
        // Then multiply by 0.5 to get a range of [0, 1]
        double a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*color(1.0, 1.0, 1.0) + a*color(0.5, 0.7, 1.0);
    }
};

#endif