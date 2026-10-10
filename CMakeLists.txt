cmake_minimum_required(VERSION 3.24)
project(GhostSystemManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()
qt_add_executable(GhostSystemManager WIN32
    src/main.cpp
    ui/MainWindow.h ui/MainWindow.cpp
    hardware/HardwareInventory.h hardware/HardwareInventory.cpp
    network/NetworkConfiguration.h network/NetworkConfiguration.cpp
    backup/BackupStore.h backup/BackupStore.cpp
    logs/OperationLogger.h logs/OperationLogger.cpp
    include/AppTypes.h
)
target_include_directories(GhostSystemManager PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/hardware"
    "${CMAKE_CURRENT_SOURCE_DIR}/network"
    "${CMAKE_CURRENT_SOURCE_DIR}/backup"
    "${CMAKE_CURRENT_SOURCE_DIR}/logs"
    "${CMAKE_CURRENT_SOURCE_DIR}/ui"
)
target_compile_definitions(GhostSystemManager PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(GhostSystemManager PRIVATE Qt6::Widgets iphlpapi setupapi tbs advapi32)
add_executable(InventoryProbe tests/InventoryProbe.cpp hardware/HardwareInventory.cpp include/AppTypes.h)
target_include_directories(InventoryProbe PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/hardware")
target_link_libraries(InventoryProbe PRIVATE Qt6::Core iphlpapi setupapi tbs advapi32 wbemuuid ole32 oleaut32)
set_target_properties(GhostSystemManager PROPERTIES WIN32_EXECUTABLE TRUE)
