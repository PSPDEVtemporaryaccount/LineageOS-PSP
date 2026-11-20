# PSP Application Package (.PSP) — Draft 0.1

**Status:** design proposal. No installer or application runtime is implemented in LineageOS PSP 0.1.0.

## Goals

The .PSP extension identifies a LineageOS PSP application package. It is a package/container format, not a renamed PBP file and not an executable format understood by PSP firmware.

## Planned package types

### WebApp

A WebApp package contains metadata, an icon, and an entry URL. Website code remains hosted by the website operator; the package does not copy or redistribute the website.

### Native

A Native package contains metadata, assets, and a PSP-compatible native application payload. The payload-loading mechanism must be finalized and tested before native packages are supported.

## Proposed metadata

- Format identifier and format version
- Unique package ID
- Display name and application version
- Package type (webapp or native)
- Entry URL or native payload reference
- Icon and optional assets
- Dependencies and minimum LineageOS PSP version
- Payload sizes and cryptographic hashes
- Publisher/signature information when available

## Security requirements before release

The future package manager must validate paths, lengths, package metadata, hashes, and signatures before installation. It must reject path traversal, malformed archives, unsupported versions, and packages that overwrite files they do not own.

## Compatibility

A .PSP package is intended for LineageOS PSP's package manager. It is not an official Sony PSP firmware package and cannot be installed directly through the XMB.
