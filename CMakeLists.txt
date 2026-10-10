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
    core/SafetyEngine.h core/SafetyEngine.cpp
    core/WindowsConfiguration.h core/WindowsConfiguration.cpp
    core/PersistenceVerification.h core/PersistenceVerification.cpp
    backup/SnapshotStore.h backup/SnapshotStore.cpp
    include/AppTypes.h
)
target_include_directories(GhostSystemManager PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/hardware"
    "${CMAKE_CURRENT_SOURCE_DIR}/network"
    "${CMAKE_CURRENT_SOURCE_DIR}/backup"
    "${CMAKE_CURRENT_SOURCE_DIR}/logs"
    "${CMAKE_CURRENT_SOURCE_DIR}/ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/core"
)
target_compile_definitions(GhostSystemManager PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN)
target_link_libraries(GhostSystemManager PRIVATE Qt6::Widgets iphlpapi setupapi tbs advapi32)
add_executable(InventoryProbe tests/InventoryProbe.cpp hardware/HardwareInventory.cpp include/AppTypes.h)
target_include_directories(InventoryProbe PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/hardware")
target_link_libraries(InventoryProbe PRIVATE Qt6::Core iphlpapi setupapi tbs advapi32 wbemuuid ole32 oleaut32)
add_executable(PersistenceProbe tests/PersistenceProbe.cpp core/PersistenceVerification.cpp include/AppTypes.h)
target_include_directories(PersistenceProbe PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/core")
target_link_libraries(PersistenceProbe PRIVATE Qt6::Core)
add_executable(ConfigurationProbe tests/ConfigurationProbe.cpp core/SafetyEngine.cpp core/WindowsConfiguration.cpp include/AppTypes.h)
target_include_directories(ConfigurationProbe PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/core")
target_link_libraries(ConfigurationProbe PRIVATE Qt6::Core advapi32)
set_target_properties(GhostSystemManager PROPERTIES WIN32_EXECUTABLE TRUE)
