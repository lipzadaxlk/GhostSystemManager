# HWID Research Driver Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add an isolated, signed-driver-ready research harness that exposes only synthetic identities and read-only diagnostics.

**Architecture:** A shared C ABI protocol is consumed by a KMDF control device, a user-mode loader/client, and a virtual identity provider. Hardware readers stay user-mode and read-only. The loader uses SCM APIs only; no manual mapping or vulnerable driver path exists.

**Tech Stack:** C++20 user mode, C-compatible KMDF source, Windows SDK/WDK, CMake for user-mode components, Visual Studio/WDK project metadata for the driver.

**Spec:** User request dated 2026-10-09: “Projeto: Driver de Kernel + Loader para Windows (C++)”.

## Global Constraints

- Windows 10/11 x64 only; driver must be test-signed in an isolated VM before installation.
- Synthetic identifiers only; no modification or interception of real hardware identifiers.
- Official SCM, device interface, buffered IOCTL, SetupAPI and documented inventory APIs only.
- No BYOVD, manual mapper, arbitrary kernel memory, security disablement, firmware writes, or anti-cheat interaction.

### Task 1: Shared protocol and virtual identity

**Files:** `HWIDResearch/Shared/Protocol.h`, `HWIDResearch/VirtualIdentity/IdentityGenerator.cpp`, `HWIDResearch/VirtualIdentity/IdentityProvider.cpp`, `HWIDResearch/tests/VirtualIdentityProbe.cpp`.

- Define versioned IOCTLs for `GET_DIAGNOSTICS` and `GET_SYNTHETIC_IDENTITY` using `METHOD_BUFFERED` and explicit size/version fields.
- Generate deterministic fictitious UUID/MAC/disk values from a caller-supplied profile seed; never read or write machine identity.
- Test determinism, format validation, and profile isolation.

### Task 2: Read-only hardware modules

**Files:** `HWIDResearch/Hardware/SMBIOS.cpp`, `Storage.cpp`, `Network.cpp`, `GPU.cpp`, `HardwareInventory.h`.

- Reuse documented WMI/registry/SetupAPI reads, returning structured records and source/error metadata.
- Keep these modules independent from the synthetic provider and driver.

### Task 3: KMDF driver source and project metadata

**Files:** `HWIDResearch/Driver/Driver.c`, `Driver.h`, `Device.c`, `Ioctl.c`, `HWIDResearch/Driver/HWIDResearch.vcxproj`.

- Create a WDF control device and symbolic interface, configure buffered IOCTL queues, validate every request, and return only diagnostics/synthetic values.
- Add unload/cleanup paths and WPP-style event logging hooks without persistence.
- Because this machine currently has Windows SDK but no WDK build environment, verify syntax/project metadata and document the exact WDK prerequisite; do not claim `.sys` compilation here.

### Task 4: Loader and client

**Files:** `HWIDResearch/Loader/main.cpp`, `ServiceManager.cpp/.h`, `DriverClient.cpp/.h`.

- Check Windows version/architecture and administrator token.
- Register/start/query/stop/delete a driver service through `OpenSCManager`/`CreateServiceW`/`StartServiceW`/`ControlService` only when explicitly requested.
- Open the device interface and issue the two IOCTLs with strict response validation; default mode is status-only and never installs.

### Task 5: Build, tests, and documentation

**Files:** `HWIDResearch/CMakeLists.txt`, `README.md`, `tests/LoaderProtocolProbe.cpp`, `docs/debugging-windbg.md`.

- Build user-mode provider/client/tests with MSVC/CMake.
- Add static checks for IOCTL buffer sizes, synthetic-only wording, and no driver installation during tests.
- Document VM-only test-signing, WinDbg commands, service lifecycle, uninstall, and known WDK limitation.

