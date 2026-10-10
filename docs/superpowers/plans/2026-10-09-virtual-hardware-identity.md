# Virtual Hardware Identity Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Add persistent synthetic identity profiles and an evidence-based comparison engine for a virtual test environment.

**Architecture:** Identity profiles are local JSON data and never replace host hardware. The existing KMDF/IOCTL research harness remains diagnostic-only. Verification compares independent guest observations against the selected profile and marks unsupported fields explicitly.

**Tech Stack:** C++20, MSVC, Windows SDK, existing CMake user-mode harness, JSON-free portable serialization for the profile core, and documented VM configuration guidance.

## Constraints

- Synthetic values only; no host firmware, TPM, disk, kernel structure, anti-cheat, or third-party process interception.
- No success result without an independent observed value.
- VM hypervisor configuration remains an explicit operator step and is not silently changed by the loader.

### Task 1: Profile model and persistence

Create `VirtualIdentity/IdentityProfile.h/.cpp` with all requested categories, strict format validation, deterministic generation from a seed, and atomic text persistence. Add `ProfileProbe` covering round-trip and invalid data.

### Task 2: Verification engine

Create `VirtualIdentity/VerificationEngine.h/.cpp` with `IdentityObservation`, `ComparisonRow`, and statuses `ALTERADO`, `NAO ALTERADO`, `NAO SUPORTADO`, `VERIFICACAO FALHOU`. Compare observed guest values rather than echoing profile values.

### Task 3: Snapshot/control flow

Create `VirtualIdentity/SnapshotManager.h/.cpp` for BEFORE/AFTER/RESTORE profiles and `IdentityFlowProbe` for scan/generate/backup/compare/restore. No reboot is automated.

### Task 4: Documentation and integration

Extend `README.md` with Hyper-V/VMware/VirtualBox examples limited to documented virtual hardware settings, WDK/VM test instructions, and exact limitations. Add CMake targets and run all probes in Release.

