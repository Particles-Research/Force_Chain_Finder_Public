cmake_minimum_required(VERSION 3.1.0)
project(example)
set (CMAKE_CXX_STANDARD 20)

# Sorting VTK library
# Here we are not using the default system vtk library
# The address to the compiled folder of VTK (not install folder)
# Tested with VTK 9.0.3.
set(VTK_DIR "/home/user_name/Downloads/vtk/build")

add_subdirectory("./src/pybind")

find_package(VTK COMPONENTS 
  vtkCommonCore
  vtkCommonDataModel
  vtkIOXML
  QUIET
) 

if (NOT VTK_FOUND)
  message("Skipping WriteParticlesVTP: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")



include_directories("./src")

# Eigen doesn't need to be installed, it is a header
# only library.
include_directories("./src/eigen-3.4.0/")
link_libraries(${VTK_LIBRARIES})

# Example
add_executable(1-single ./examples/1-single.cpp)
add_executable(2-filter ./examples/2-filter.cpp)
add_executable(3-batch ./examples/3-batch.cpp)
add_executable(4-moving ./examples/4-moving.cpp)
# pybind11_add_module(forcechain "./src/pybind/userinterface.cpp")


