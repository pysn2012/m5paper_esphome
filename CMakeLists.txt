# M5Paper Display Component CMakeLists.txt
#
# CMake configuration for building the M5Paper Display component
# as an external component for ESPHome.
#
# Author: MiniMax Agent
# Date: 2026-03-24

idf_component_register(
    SRCS
        "src/m5paper_display.cpp"
        "src/it8951_driver.cpp"
    INCLUDE_DIRS
        "src"
    REQUIRES
        esp_idf_lib_helpers
        driver
        spi
        freertos
        esphome
)
