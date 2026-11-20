# LineageOS PSP — Architecture

## Platform
- Initial target: Sony PSP-3000.
- Display: 480 × 272, 32-bit framebuffer with a 512-pixel row stride.
- Language and SDK: C with PSPSDK.
- XMB-launchable executable: EBOOT.PBP.
- Build: GitHub Actions with the PSPDEV toolchain.

## 0.2.0 graphical shell
- src/main.c draws the UI into two PSP VRAM framebuffers.
- A compact built-in 5×7 bitmap font renders interface labels without a text-console UI.
- Rectangles, cards, selection outlines, status bars, and icon tiles are rendered in software.
- Controller input routes between Home, Settings, Quick Setup, and Recovery.
- src/config.c stores the appearance and first-start preferences on the Memory Stick.

## Safety and fallback
The official PSP XMB and firmware remain intact. Exiting this homebrew returns control to the PSP environment. The in-app Recovery screen is not the PSP recovery menu and does not replace it.

## Future modules
The desktop shell, browser integration, connectivity, AI features, PSP Store, and .PSP package installer/runtime remain future work.
