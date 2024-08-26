#include "../include/rtweekend.h"

#include "../include/hittable.h"
#include "../include/hittable_list.h"
#include "../include/sphere.h"
#include "../include/camera.h"

#include <fstream>
#include <string>

int main() {
    std::string out;
    std::ofstream outfile("out.ppm", std::ios::out);

    hittable_list world;

    world.add(make_shared<sphere>(point3(0,0,-1), 0.5));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100));

    camera cam;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.render(world, out);

    if(outfile.is_open()) {
        outfile << out;
        outfile.close();
    }

    return 0;
}

