# LineageOS PSP — Architecture

## Purpose

LineageOS PSP is a lightweight PSP homebrew environment inspired by the Android 11/14 user experience. It is not an Android port, Linux distribution, or replacement for PSP firmware.

## Platform

- Initial target: Sony PSP-3000.
- Display: 480 × 272.
- Language and SDK: C with PSPSDK.
- XMB-launchable executable: EBOOT.PBP.
- Build: GitHub Actions with the PSPDEV toolchain.

## Safety and fallback

The official PSP XMB and firmware remain intact. Exiting this homebrew returns control to the PSP environment. The initial Recovery screen is an in-application fallback; it does not replace the PSP recovery menu.

## 0.1.0 modules

- src/main.c: startup, screen routing, controller input, and initial interface.
- src/config.c: persistent settings on the Memory Stick.
- src/config.h: versioned configuration interface.

Future releases will separate the launcher, settings, desktop shell, connectivity, recovery, package manager, and system applications into independent modules.
