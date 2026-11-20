# LineageOS PSP

**Version:** 0.1.0  
**Status:** Early development  
**Target:** Sony PSP (initial target: PSP-3000)

LineageOS PSP is an Android-inspired, lightweight homebrew environment for the PlayStation Portable. Its visual direction takes inspiration from Android 11 and Android 14.

> This is not an Android or Linux port and does not replace the official PSP firmware or XMB.

## Version 0.1.0

The first development milestone provides:

- A bootable PSP homebrew entry point.
- A basic home screen navigable with the PSP D-pad and buttons.
- Initial Settings and Recovery screens.
- A first-start Quick Setup that is not repeated once saved.
- A persistent dark/light appearance preference on the Memory Stick.
- An automated GitHub Actions build producing EBOOT.PBP.

The Applications entry is currently a placeholder. The desktop environment, browser integration, connectivity services, AI features, PSP Store, and .PSP package installer/runtime are future work.

## Install and launch

1. Download EBOOT.PBP from the GitHub Actions artifact for a successful build.
2. On the Memory Stick, create PSP/GAME/LineageOS-PSP/.
3. Place EBOOT.PBP in that folder.
4. Launch LineageOS PSP from the PSP Game menu.

The PSP's existing firmware and XMB are left untouched. Choose the exit option to return to the XMB.

## Controls

- D-pad: navigate.
- CROSS: select / confirm.
- CIRCLE: go back or exit.
- In Recovery: CROSS returns to the interface; CIRCLE exits to XMB.

## Build

The project uses PSPSDK and the PSPDEV toolchain. GitHub Actions builds the project automatically on pushes to main, pull requests, and manual workflow dispatches.

For a local PSPDEV environment:

    make clean
    make

The resulting EBOOT.PBP is generated in the repository root.

## Application package format

The proposed .PSP package format is documented in docs/PSP-PACKAGE-FORMAT-0.1.md. It is a separate application container format, not the system executable. The package manager and runtime are not implemented in 0.1.0.

## Project documentation

- Architecture: docs/ARCHITECTURE.md
- PSP package format draft: docs/PSP-PACKAGE-FORMAT-0.1.md
