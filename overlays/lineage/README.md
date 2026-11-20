# LineageOS PSP overlay

This directory contains the project-owned adaptation layer applied to the pinned CyanogenPSP source during CI.

The first layer changes the product/module identity and build version. The original first-boot state handling is intentionally retained: CyanogenPSP stores the completed setup state in `system/settings/boot.bin`, so the welcome flow is not supposed to repeat on every normal launch.

The build workflow applies these changes to a clean checkout of the upstream source. This avoids silently replacing upstream files or removing Joel16's notices.
