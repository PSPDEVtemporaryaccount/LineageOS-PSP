# LineageOS PSP

**Version:** 0.2.0  
**Status:** Early development  
**Target:** Sony PSP (initial target: PSP-3000)

LineageOS PSP is a lightweight PSP homebrew environment inspired by the Android 11/14 user experience. It is not an Android or Linux port and does not replace the official PSP firmware or XMB.

## What's new in 0.2.0

- A graphical 480 × 272 home screen drawn directly to the PSP framebuffer.
- Visual application cards, icons, selected-card outline, top bar, and bottom control hints.
- Graphical Settings, Quick Setup, and Recovery screens.
- D-pad navigation and CROSS/CIRCLE actions.
- Persistent dark/light appearance preference and first-start state.
- GitHub Actions builds EBOOT.PBP and publishes it as a standalone GitHub Release asset (not wrapped in a ZIP). A short-lived Actions artifact is also kept as a fallback.

## Download

Open the [LineageOS PSP 0.2.0 release](https://github.com/PSPDEVtemporaryaccount/LineageOS-PSP/releases/tag/v0.2.0) and download **EBOOT.PBP** under Assets.

Direct asset URL:
https://github.com/PSPDEVtemporaryaccount/LineageOS-PSP/releases/download/v0.2.0/EBOOT.PBP

## Install

1. On the Memory Stick, create `PSP/GAME/LineageOS-PSP/`.
2. Copy the downloaded `EBOOT.PBP` into that folder.
3. Launch LineageOS PSP from the PSP Game menu.

The browser on Android may save the file in Downloads. Android does not let a remote GitHub workflow silently place a file in PPSSPP's private folder or on a physical PSP Memory Stick; move/copy it using the file manager or USB transfer.

## Controls

- D-pad: navigate.
- CROSS: select / confirm.
- CIRCLE: go back or exit.
- Recovery: CROSS returns to the interface; CIRCLE exits to XMB.

## Build

The project uses PSPSDK and the PSPDEV toolchain. GitHub Actions builds on pushes to main, pull requests, and manual workflow dispatches. Main-branch builds publish/update the 0.2.0 prerelease asset.

For a local PSPDEV environment:

    make clean
    make

The resulting EBOOT.PBP is generated in the repository root.

## Application package format

The proposed .PSP package format is documented in [docs/PSP-PACKAGE-FORMAT-0.1.md](docs/PSP-PACKAGE-FORMAT-0.1.md). It remains separate from the system executable; the package manager and runtime are future work.
