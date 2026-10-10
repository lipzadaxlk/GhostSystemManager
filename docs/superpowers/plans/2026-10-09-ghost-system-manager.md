# Ghost System Manager Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use `subagent-driven-development` (recommended) or `executing-plans` to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a native C++20 Windows GUI that inventories machine identifiers, classifies their configurability, and safely backs up, generates, applies, verifies, and restores only documented, reversible settings.

**Architecture:** A Qt 6 Widgets application coordinates focused C++ modules for inventory, compatibility classification, configuration planning, persistence, and audit logging. Hardware discovery is read-only through documented Windows APIs; the only planned mutable setting is the per-adapter `NetworkAddress` registry value, applied only after an elevation check and explicit in-app confirmation.

**Tech Stack:** C++20, Qt 6 Widgets, Win32 API, WMI via COM, Windows SetupAPI/IP Helper APIs, CMake, MSVC, Windows SDK.

**Spec:** `C:\Projetos\GhostSystemManager\docs\superpowers\plans\2026-10-09-ghost-system-manager.md`

## Global Constraints

- C++20 only; build a native Windows executable with MSVC, CMake, Windows SDK, and Qt 6 Widgets.
- All source is ASCII-only and lives under `C:\Projetos\GhostSystemManager`.
- Hardware, firmware, storage, TPM, and installation identifiers are inspection-only unless Windows documents a supported mutable interface.
- Do not implement bypasses, protected-identifier falsification, kernel components, or firmware mutation.
- Applying configuration requires an administrator token and an explicit GUI confirmation.
- Backups contain only supported mutable settings and are written beneath `backup\`.

---

### Task 1: Project and build configuration

**Files:**
- Create: `CMakeLists.txt`
- Create: `src/main.cpp`
- Create: `include/AppTypes.h`

**Interfaces:**
- Produces `GhostSystemManager` Windows GUI target and the `IdentityRecord`, `ChangePlan`, and `OperationResult` domain types.

- [ ] **Step 1: Create the CMake target**

```cmake
cmake_minimum_required(VERSION 3.24)
project(GhostSystemManager VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
find_package(Qt6 6.4 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()
qt_add_executable(GhostSystemManager WIN32 src/main.cpp)
target_link_libraries(GhostSystemManager PRIVATE Qt6::Widgets)
```

- [ ] **Step 2: Configure Release mode**

Run: `cmake -S C:\Projetos\GhostSystemManager -B C:\Projetos\GhostSystemManager\build -G "Visual Studio 17 2022" -A x64`

Expected: CMake creates a Visual Studio solution without configuration errors.

### Task 2: Read-only inventory and compatibility classification

**Files:**
- Create: `hardware/HardwareInventory.h`
- Create: `hardware/HardwareInventory.cpp`
- Create: `core/Compatibility.h`
- Create: `core/Compatibility.cpp`

**Interfaces:**
- Consumes: `IdentityRecord` from `include/AppTypes.h`.
- Produces: `std::vector<IdentityRecord> HardwareInventory::scan()` and `std::vector<IdentityRecord> Compatibility::classify(records)`.

- [ ] **Step 1: Add a failing inventory smoke test target**

```cpp
int main() {
  HardwareInventory inventory;
  return inventory.scan().empty() ? 1 : 0;
}
```

- [ ] **Step 2: Implement documented reads**

Use WMI `Win32_BaseBoard`, `Win32_BIOS`, `Win32_ComputerSystemProduct`, `Win32_Processor`, `Win32_VideoController`, `Win32_DiskDrive`, `Win32_Volume`, `Win32_NetworkAdapter`, `Win32_Tpm` when available, `GetAdaptersAddresses`, and registry reads for `MachineGuid`.

- [ ] **Step 3: Run the smoke test**

Run: `ctest --test-dir C:\Projetos\GhostSystemManager\build -C Release --output-on-failure`

Expected: at least one record and no write operation.

### Task 3: Reversible network setting, backup, restore, and verification

**Files:**
- Create: `network/NetworkConfiguration.h`
- Create: `network/NetworkConfiguration.cpp`
- Create: `backup/BackupStore.h`
- Create: `backup/BackupStore.cpp`
- Create: `core/Operations.h`
- Create: `core/Operations.cpp`

**Interfaces:**
- Consumes: `ChangePlan` and adapter records.
- Produces: `NetworkConfiguration::generatePlan`, `apply`, `verify`, and `restore` with `OperationResult`.

- [ ] **Step 1: Test valid locally administered MAC generation**

```cpp
auto mac = NetworkConfiguration::generateLocalAdminMac();
assert(mac.size() == 12 && mac[1] == '2');
```

- [ ] **Step 2: Implement planning without mutation**

Generate a locally administered unicast MAC, bind it to a selected adapter GUID, and mark the plan as `requiresAdministrator=true`.

- [ ] **Step 3: Implement safe apply and restore**

Resolve the adapter class registry key through documented SetupAPI properties, save the original `NetworkAddress` value, write only that value after an elevation check, and restart neither adapters nor services automatically.

- [ ] **Step 4: Verify independently**

Use `GetAdaptersAddresses` after re-enumeration and report exact observed state; do not report success merely because registry writing succeeded.

### Task 4: Qt control panel and audit log

**Files:**
- Create: `ui/MainWindow.h`
- Create: `ui/MainWindow.cpp`
- Create: `logs/OperationLogger.h`
- Create: `logs/OperationLogger.cpp`

**Interfaces:**
- Consumes inventory records and operation results.
- Produces a dark purple Qt Widgets interface for Scan Hardware, Check Compatibility, Generate Configuration, Apply Changes, Verify, Backup, Restore, and View Logs.

- [ ] **Step 1: Build the interface**

Use `QMainWindow`, `QTableWidget`, `QTextEdit`, `QComboBox`, and explicit action buttons. Disable Apply until a valid plan exists.

- [ ] **Step 2: Wire each operation**

Each action calls the matching service and appends timestamped success, failure, or unsupported messages to `logs\operations.log`.

- [ ] **Step 3: Test launch**

Run: `C:\Projetos\GhostSystemManager\build\Release\GhostSystemManager.exe`

Expected: the window opens, scanning shows detected identifiers, and no system setting changes without an in-app confirmation.

### Task 5: Virtual-machine identity testing guidance

**Files:**
- Create: `core/VmIdentityGuide.h`
- Create: `core/VmIdentityGuide.cpp`

**Interfaces:**
- Produces a GUI-displayed documented guide for Hyper-V and generic VM workflows; it never invokes a hypervisor or changes a VM.

- [ ] **Step 1: Add the guidance module**

Provide validated operator steps: create a new VM, create a new virtual disk, generate platform-supported VM UUID, create a fresh virtual NIC, install Windows fresh, then scan and compare exported inventories.

- [ ] **Step 2: Verify the guide is visible**

Run the executable and select the VM testing panel; confirm it contains no execution button and no claim of changing hardware identity.

### Task 6: Release verification

**Files:**
- Modify: `CMakeLists.txt`
- Create: `release/GhostSystemManager.exe`

- [ ] **Step 1: Configure and compile**

Run: `cmake --build C:\Projetos\GhostSystemManager\build --config Release --target GhostSystemManager`

- [ ] **Step 2: Validate the artifact**

Run: `Get-Item C:\Projetos\GhostSystemManager\build\Release\GhostSystemManager.exe`

Expected: a nonempty executable with a current build timestamp.

- [ ] **Step 3: Stage the release executable**

Run: `Copy-Item C:\Projetos\GhostSystemManager\build\Release\GhostSystemManager.exe C:\Projetos\GhostSystemManager\release\GhostSystemManager.exe -Force`

## Self-Review

The plan covers every requested inventory class, all named GUI controls, compatibility reporting, supported configuration generation, elevated apply, independent verification, backup/restore, logs, VM testing, CMake/MSVC/Qt requirements, compilation, launch testing, and release staging. Immutable and firmware-controlled identifiers remain read-only. No offsets, undocumented interfaces, or false change claims are used.
