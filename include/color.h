#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

#include <string>

using color = vec3;

void write_color(std::string& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Translate the [0,1] component values to the byte range [0,255].
    
    // Due to anti-aliasing, the color might result in a value outside
    // of the range [0, 1], so we first set an interval to clamp it
    static const interval intensity(0.0, 0.999);

    // From there, we use the interval to clamp
    int rbyte = int(255.999 * intensity.clamp(r));
    int gbyte = int(255.999 * intensity.clamp(g));
    int bbyte = int(255.999 * intensity.clamp(b));

    // Write out the pixel color components.
    out += std::to_string(rbyte) + ' ' + std::to_string(gbyte) + ' ' + std::to_string(bbyte) + '\n';
}
#endif