cmake_minimum_required(VERSION 3.28.3)

set(TRACY "tracy")
project(${TRACY})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_COLOR_DIAGNOSTICS ON)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

set(SOURCES
    main.cpp

    src/app/gui.cpp
    src/app/cpu_viewer.cpp
    src/app/scene_factory.cpp

    src/core/random.cpp
    src/scene/camera.cpp
    src/scene/scene.cpp
    src/geometry/sphere.cpp
    src/geometry/triangle.cpp
    src/geometry/plane.cpp
    src/shading/materials/material_base.cpp
    src/shading/materials/lambert_material.cpp
    src/shading/materials/mirror_material.cpp
    src/shading/texture.cpp
    src/shading/image_texture.cpp
    src/shading/light.cpp
    src/shading/image_texture.cpp

    src/render/renderer.cpp
    src/render/direct_lighting_integrator.cpp
    src/render/whitted_integrator.cpp
    src/render/path_tracing_integrator.cpp

    src/accel/bbox.cpp
    src/accel/octree.cpp
    src/accel/bvh.cpp

    src/io/image.cpp
    src/io/obj_loader.cpp
)

set(HEADERS
    include/app/gui.h
    include/app/cpu_viewer.h
    include/app/iviewer.h
    include/app/scene_factory.h

    include/core/ray.h
    include/core/color.h
    include/core/random.h
    include/core/constants.h

    include/scene/hit_record.h
    include/scene/camera.h
    include/scene/scene.h
    include/geometry/primitive.h
    include/geometry/geometry.h
    include/geometry/sphere.h
    include/geometry/triangle.h
    include/geometry/plane.h
    include/shading/shading.h
    include/shading/material.h
    include/shading/materials/material_types.h
    include/shading/materials/material_base.h
    include/shading/materials/lambert_material.h
    include/shading/materials/mirror_material.h
    include/shading/texture.h
    include/shading/image_texture.h
    include/shading/light.h
    include/shading/image_texture.h

    include/render/renderer.h
    include/render/integrator.h
    include/render/direct_lighting_integrator.h
    include/render/whitted_integrator.h
    include/render/path_tracing_integrator.h

    include/accel/bbox.h
    include/accel/accel_structure.h
    include/accel/octree.h
    include/accel/bvh.h

    include/io/image.h
    include/io/obj_loader.h
)


find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Threads REQUIRED)

add_executable(${TRACY} ${SOURCES} ${HEADERS})

target_include_directories(${TRACY} PRIVATE
    include
    3rd_party
)

target_compile_definitions(${TRACY} PRIVATE
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(${TRACY}
    PRIVATE Qt6::Widgets
    ${CMAKE_THREAD_LIBS_INIT}
)
