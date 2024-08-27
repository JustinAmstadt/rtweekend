cmake_minimum_required(VERSION 3.29)
project(raytracing)

set(CMAKE_CXX_STANDARD 14)

add_executable(raytracing
    src/main.cpp
    include/color.h
    include/vec3.h
    include/ray.h
    include/hittable.h
    include/sphere.h
    include/hittable_list.h
    include/rtweekend.h
    include/interval.h
    include/camera.h
    include/material.h
)