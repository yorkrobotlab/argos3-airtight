add_library(circle_controller MODULE controller.h controller.cpp)
target_link_libraries(circle_controller
        circle_logging_loop_functions)
#  argos3core_simulator
#  argos3plugin_simulator_pipuck)
