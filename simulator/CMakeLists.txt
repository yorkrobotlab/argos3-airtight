add_library(argos3plugin_simulator_genericrobot-slotradio SHARED
            slot-radio/control_interface/ci_slot_radio_actuator.h
            slot-radio/control_interface/ci_slot_radio_sensor.h
            slot-radio/generic/slot_radio_frame.h
            slot-radio/generic/slot_radio_medium.h
            slot-radio/generic/slot_radio_actuator.h
            slot-radio/generic/slot_radio_sensor.h
            slot-radio/airtight/AirTightFrame.h
            slot-radio/airtight/AirTightBuffer.h
            slot-radio/airtight/AirTightActuator.h
            slot-radio/airtight/AirTightSensor.h
            slot-radio/naive-broadcast/NaiveBroadcastFrame.h
            slot-radio/naive-broadcast/NaiveBroadcastActuator.h
            slot-radio/naive-broadcast/NaiveBroadcastSensor.h
            slot-radio/naive-p2p/NaiveP2PFrame.h
            slot-radio/naive-p2p/NaiveP2PActuator.h
            slot-radio/naive-p2p/NaiveP2PSensor.h
            slot-radio/generic/slot_radio_medium.cpp
            slot-radio/airtight/AirTightActuator.cpp
            slot-radio/airtight/AirTightSensor.cpp
            slot-radio/naive-broadcast/NaiveBroadcastActuator.cpp
            slot-radio/naive-broadcast/NaiveBroadcastSensor.cpp
            slot-radio/naive-p2p/NaiveP2PActuator.cpp
            slot-radio/naive-p2p/NaiveP2PSensor.cpp)

target_link_libraries(argos3plugin_simulator_genericrobot-slotradio
                      argos3core_simulator 
                      argos3plugin_simulator_genericrobot 
                      argos3plugin_simulator_entities 
                      argos3plugin_simulator_media)

add_library(argos3plugin_simulator_genericrobot_bigled
            SHARED
            BigLEDActuator.h
            BigLEDActuator.cpp)

target_link_libraries(argos3plugin_simulator_genericrobot_bigled
                      argos3core_simulator
                      argos3plugin_simulator_genericrobot)
