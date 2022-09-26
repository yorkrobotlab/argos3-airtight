add_library(circle_opengl_functions SHARED
        circle_opengl_functions.h
        circle_opengl_functions.cpp)

target_link_libraries(circle_opengl_functions
  argos3core_simulator
  argos3plugin_simulator_pipuck
  argos3plugin_simulator_qtopengl)

add_library(circle_logging_loop_functions SHARED
        circle_loop_functions.h
        circle_loop_functions.cpp)

target_link_libraries(circle_logging_loop_functions
        argos3core_simulator
        argos3plugin_simulator_pipuck)