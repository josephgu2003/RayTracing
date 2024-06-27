# CMakeList.txt : CMake project for RayTracing, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("RayTracing")

# Add source to this project's executable.
add_executable (RayTracing "RayTracing.cpp" "RayTracing.h" "stb_image_write.h" "Color.h" "Ray.h" "Point.h"  "Hittable.h"  "Sphere.h" "Interval.h" "Vec.h" "HittableList.h" "Camera.h" "Material.h" "Random.h" "Quad.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RayTracing PROPERTY CXX_STANDARD 20)
endif()

find_package(glm CONFIG REQUIRED)
target_link_libraries(RayTracing PRIVATE glm::glm-header-only)

# TODO: Add tests and install targets if needed.
