# Advanced System Configuration Engine V2 Implementation Plan

> **For agentic workers:** Execute this plan task-by-task with checkpoints and verification.

**Goal:** Expand the existing native C++20 Windows application with evidence-based hardware inventory, safe supported configuration changes, snapshot persistence verification, TPM diagnostics, rollback, and a truthful professional GUI.

**Architecture:** Keep the existing Qt/Win32 application and add focused services around structured snapshots and operation results. Hardware remains read-only except for explicitly supported computer-name and adapter settings. Every write flows through capability checks, backup, confirmation, verification, and optional reboot continuation.

**Tech Stack:** C++20, MSVC, CMake, Qt 6 Widgets, Windows SDK, WMI/COM, SMBIOS `GetSystemFirmwareTable`, SetupAPI/Configuration Manager, Registry read APIs, IP Helper, TPM Base Services.

**Spec:** User-provided “GHOST SYSTEM MANAGER — ADVANCED SYSTEM CONFIGURATION ENGINE V2”.

## Global Constraints

- Windows 10/11 x64; native C++20; MSVC/CMake/Qt 6.
- No third-party drivers, kernel mappers, anti-cheat bypass, firmware writes, sector writes, shadow-copy deletion, log deletion, TPM clearing, or security disabling.
- No invented identifiers; preserve source, error, confidence, and cross-source discrepancies.
- No automatic reboot; no success message without independent verification.
- Existing executable is preserved until Release build, tests, and launch verification pass.

### Task 1: Checkpoint and baseline inventory

**Files:** Modify `CMakeLists.txt` only if required by later tasks. Test with existing `InventoryProbe`.

- [ ] Record current Git revision and working tree.
- [ ] Read `research/reports/*` and `research/security/*`.
- [ ] Run the existing Release build and probe; save output under `build/`.
- [ ] Commit a baseline checkpoint before source changes.

### Task 2: Structured inventory sources and diagnostics

**Files:**
- Create: `hardware/SMBIOSReader.h/.cpp`
- Create: `hardware/SystemUUIDReader.h/.cpp`
- Create: `hardware/TPMInspector.h/.cpp`
- Modify: `hardware/HardwareInventory.h/.cpp`, `include/AppTypes.h`, `CMakeLists.txt`
- Test: extend `tests/InventoryProbe.cpp`

**Interfaces:** `SMBIOSReader::scan()`, `SystemUUIDReader::read()`, and `TPMInspector::inspect()` return records with source, status, and reason. Existing `IdentityRecord` remains source-compatible.

- [ ] Add source-aware records and a UUID comparison that reports SMBIOS/WMI/registry disagreement instead of silently choosing.
- [ ] Move TPM logic into `TPMInspector`: WMI HRESULT classification, TBS presence, PNP device fallback, and spec/readiness fields when available.
- [ ] Keep raw identifiers out of diagnostics except in the inventory table; logs use source/status summaries.
- [ ] Add tests for valid/zero/placeholder UUID and WMI/TBS/PNP TPM states using injectable helpers where hardware is unavailable.

### Task 3: Safe configuration and capability engine

**Files:**
- Create: `core/ConfigurationEngine.h/.cpp`
- Create: `core/SafetyEngine.h/.cpp`
- Create: `core/WindowsConfiguration.h/.cpp`
- Modify: `network/NetworkConfiguration.h/.cpp`, `include/AppTypes.h`, `CMakeLists.txt`
- Test: `tests/ConfigurationEngineProbe.cpp`

- [ ] Define `Capability`, `ConfigurationProposal`, and `ConfigurationOutcome` structures.
- [ ] Implement computer-name proposal/apply/verify using documented `SetComputerNameEx` only; return reboot-required instead of rebooting.
- [ ] Keep adapter `NetworkAddress` driver-dependent; inspect the exact registry value, preserve existence/value, and refuse unsupported adapters.
- [ ] Centralize administrator/API/OS/device/dependency/rollback checks in `SafetyEngine`.
- [ ] Ensure one failure stops the operation without chained destructive actions.

### Task 4: Snapshots, persistence, backup and rollback

**Files:**
- Create: `core/PersistenceVerification.h/.cpp`
- Create: `backup/SnapshotStore.h/.cpp`
- Modify: `backup/BackupStore.h/.cpp`, `logs/OperationLogger.cpp`
- Test: `tests/PersistenceProbe.cpp`

- [ ] Define snapshots A/B/C with timestamp, operation ID, field, value hash/redacted value, and source.
- [ ] Compare fields as `UNCHANGED`, `CHANGED`, `PERSISTENT`, `REVERTED`, `UNSUPPORTED`, or `VERIFICATION FAILED`.
- [ ] Store JSON atomically under the application data directory with restrictive ACL where available.
- [ ] Restore only values with an explicit reversible method; report partial failure precisely.
- [ ] Do not claim persistence without a separately supplied post-reboot snapshot.

### Task 5: GUI sections and truthful workflow

**Files:** Modify `ui/MainWindow.h/.cpp`; create `ui/ConfigurationPanel.h/.cpp` only if the existing file becomes unmanageable.

- [ ] Add dashboard cards for hardware profile, capabilities, safety diagnostics, snapshot state, backup/restore, and logs.
- [ ] Wire SCAN, CHECK CAPABILITIES, PREPARE, APPLY, VERIFY, RESTORE, and VIEW LOGS to real services.
- [ ] Require explicit confirmation before writes and display reboot-required state.
- [ ] Disable unsupported actions with reasons; never show synthetic percentages or success before verification.

### Task 6: Tests, Release build and delivery

**Files:** Modify `CMakeLists.txt`, tests, installer version metadata, and release artifacts.

- [ ] Build `InventoryProbe`, configuration probe, persistence probe, and GUI in Release.
- [ ] Run non-destructive tests for parsing, invalid UUID, COM/WMI failure, TPM absent/permission, disconnected adapter, snapshot comparison, backup integrity, and cancellation.
- [ ] Launch the Release executable and verify it remains responsive.
- [ ] Compute SHA-256, update installer/RAR only after all checks, and commit the implementation.

## Coverage notes

The plan explicitly rejects unsafe repository mechanisms documented in `research/security/malware-audit.md`. VM identity management remains inspection/comparison-only unless a documented hypervisor API and an isolated test VM are supplied later.
