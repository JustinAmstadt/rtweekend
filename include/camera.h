#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"

class camera {
  public:
    double aspect_ratio = 16.0 / 9.0; // Width / Height
    int image_width = 400; // In pixels
    int samples_per_pixel = 10; // Count of random samples for each pixel
    int max_depth = 10; // Maximum number of ray bounces into scene

    double vfov = 90; // Vertical view angle (field of view)
    point3 lookfrom = point3(0,0,0); // Point camera is looking from
    point3 lookat = point3(0,0,-1); // Point camera is looking at
    vec3 vup = vec3(0,1,0); // Camera-relative "up" direction

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
    vec3 u, v, w; // Camera frame basis vectors

    void initialize() {
        // Calculate the image height, and ensure that it's at least 1.
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        // Multiply with final color to get an average of all samples used
        pixel_samples_scale = 1.0 / samples_per_pixel;

        camera_center = lookfrom;

        // Determine viewport dimensions
        double focal_length = (lookfrom - lookat).length(); // Distance between camera and viewport
        double theta = degrees_to_radians(vfov); // Convert vertical field of view to radians
        double h = std::tan(theta/2); // Get the half-height of viewport
        double viewport_height = 2 * h * focal_length; // Scale h by 2, then scale viewport_height to desired world space
        double viewport_width = viewport_height * (double(image_width)/image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat); // Forward/look-at vector
        u = unit_vector(cross(vup, w)); // Right vector
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = camera_center - (focal_length * w) - viewport_u/2 - viewport_v/2;
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

            ray scattered;
            color attenuation;
            if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                // Eventually take on the background color, but darker because of
                // the attenuation multiplication
                return attenuation * ray_color(scattered, depth-1, world);
            }
            return color(0,0,0);
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