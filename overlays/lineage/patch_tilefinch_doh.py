#!/usr/bin/env python3
"""Patch the pinned Tilefinch PSP transport for fail-closed Cloudflare DoH."""

from pathlib import Path
import sys


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise SystemExit(f"{label}: expected one patch point, found {count}")
    return text.replace(old, new, 1)


if len(sys.argv) != 2:
    raise SystemExit("usage: patch_tilefinch_doh.py <Tilefinch source directory>")

root = Path(sys.argv[1])
fetch_path = root / "src/fetch.c"
transport_path = root / "cmake/PspOwnedTransport.cmake"
lock_path = root / "third_party/psp_transport/dependencies.lock"
stage_path = root / "cmake/StagePspInstall.cmake"
targets_path = root / "cmake/TilefinchTargets.cmake"
manifest_path = root / "cmake/PspNoticesManifest.cmake"
notices_path = root / "THIRD_PARTY_NOTICES.md"

for required in (fetch_path, transport_path, lock_path, stage_path,
                 targets_path, manifest_path, notices_path):
    if not required.is_file():
        raise SystemExit(f"Missing expected Tilefinch source file: {required}")

# libcurl's DoH implementation needs an asynchronous resolver. Tilefinch's
# upstream PSP profile disables DoH and threaded DNS; build c-ares instead.
CARES_VERSION = "1.34.9"
CARES_ARCHIVE = "c-ares-1.34.9.tar.gz"
CARES_SHA256 = "9364b70a12eb59a06830fb488d9357f90100dff0d98b23ea3593a183d43e6f2f"
CARES_URL = "https://github.com/c-ares/c-ares/releases/download/v1.34.9/c-ares-1.34.9.tar.gz"

# 1) Apply the DoH settings to every Tilefinch request handle.
source = fetch_path.read_text(encoding="utf-8")
function_marker = "static bool fetch_configure_tls(CURL *easy)\n{\n"
tls_check = """        || curl_easy_setopt(easy, CURLOPT_SSL_VERIFYHOST, 2L) != CURLE_OK) {
        return false;
    }
"""
declaration = """#if defined(__PSP__) && defined(TILEFINCH_PSP_OWNED_TRANSPORT)
/* Pin only the DoH server bootstrap. The endpoint hostname remains in the
   URL so libcurl can verify its TLS certificate against that hostname. */
static char fetch_doh_bootstrap_entry[] =
    "cloudflare-dns.com:443:1.1.1.1";
static struct curl_slist fetch_doh_bootstrap = {
    fetch_doh_bootstrap_entry, NULL
};
#endif

"""
doh_config = tls_check + """#if defined(__PSP__) && defined(TILEFINCH_PSP_OWNED_TRANSPORT)
    if (curl_easy_setopt(easy, CURLOPT_RESOLVE,
                         &fetch_doh_bootstrap) != CURLE_OK
        || curl_easy_setopt(easy, CURLOPT_DOH_URL,
                            "https://cloudflare-dns.com/dns-query") != CURLE_OK
        || curl_easy_setopt(easy, CURLOPT_DOH_SSL_VERIFYPEER, 1L) != CURLE_OK
        || curl_easy_setopt(easy, CURLOPT_DOH_SSL_VERIFYHOST, 2L) != CURLE_OK) {
        return false;
    }
#endif
"""
if "CURLOPT_DOH_URL" not in source:
    source = replace_once(source, function_marker,
                          declaration + function_marker,
                          "fetch.c TLS function")
    source = replace_once(source, tls_check, doh_config,
                          "fetch.c TLS verification block")
elif "static struct curl_slist fetch_doh_bootstrap =" not in source:
    raise SystemExit("Partial DoH patch found in fetch.c")
if source.count("CURLOPT_DOH_URL") != 1:
    raise SystemExit("fetch.c DoH option verification failed")
fetch_path.write_text(source, encoding="utf-8")

# 2) Build c-ares as the asynchronous resolver required by libcurl DoH.
transport = transport_path.read_text(encoding="utf-8")
if "tilefinch_psp_cares" not in transport:
    transport = replace_once(
        transport,
        'set(_curl_archive "${TILEFINCH_PSP_TRANSPORT_CACHE}/curl-8.22.0.tar.xz")\n',
        'set(_curl_archive "${TILEFINCH_PSP_TRANSPORT_CACHE}/curl-8.22.0.tar.xz")\n'
        'set(_cares_archive "${TILEFINCH_PSP_TRANSPORT_CACHE}/c-ares-1.34.9.tar.gz")\n',
        "PSP transport archive declarations")
    transport = replace_once(
        transport,
        '        "nghttp2|1.69.0|nghttp2-1.69.0.tar.xz|1fb324b6ec2c56f6bde0658f4139ffd8209fa9e77ce98fd7a5f63af8d0e508ad|https://github.com/nghttp2/nghttp2/releases/download/v1.69.0/nghttp2-1.69.0.tar.xz")',
        '        "nghttp2|1.69.0|nghttp2-1.69.0.tar.xz|1fb324b6ec2c56f6bde0658f4139ffd8209fa9e77ce98fd7a5f63af8d0e508ad|https://github.com/nghttp2/nghttp2/releases/download/v1.69.0/nghttp2-1.69.0.tar.xz"\n'
        '        "cares|1.34.9|c-ares-1.34.9.tar.gz|9364b70a12eb59a06830fb488d9357f90100dff0d98b23ea3593a183d43e6f2f|https://github.com/c-ares/c-ares/releases/download/v1.34.9/c-ares-1.34.9.tar.gz")',
        "PSP transport locked dependency list")
    transport = replace_once(
        transport,
        'foreach(_archive IN ITEMS "${_mbedtls_archive}" "${_curl_archive}")',
        'foreach(_archive IN ITEMS "${_mbedtls_archive}" "${_curl_archive}" "${_cares_archive}")',
        "PSP transport archive existence check")

    cares_project = """ExternalProject_Add(tilefinch_psp_cares
    URL "${_cares_archive}"
    URL_HASH
        SHA256=9364b70a12eb59a06830fb488d9357f90100dff0d98b23ea3593a183d43e6f2f
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/psp-transport/cares"
    PATCH_COMMAND
        "${CMAKE_COMMAND}"
        "-DPATCH_SOURCE_DIR=<SOURCE_DIR>"
        "-DPATCH_FILE=${CMAKE_CURRENT_SOURCE_DIR}/../../overlays/lineage/patches/c-ares-1.34.9-psp.patch"
        "-DPATCH_EXECUTABLE=${PATCH_EXECUTABLE}"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/apply_patch.cmake"
    CMAKE_ARGS
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DCMAKE_INSTALL_PREFIX=${_transport_prefix}"
        "-DCMAKE_BUILD_TYPE=MinSizeRel"
        "-DCMAKE_C_FLAGS=-G0 -D_DEFAULT_SOURCE${_transport_prefix_map_flags}"
        "-DCARES_STATIC=ON"
        "-DCARES_SHARED=OFF"
        "-DCARES_INSTALL=ON"
        "-DCARES_BUILD_TESTS=OFF"
        "-DCARES_BUILD_TOOLS=OFF"
        "-DCARES_THREADS=OFF"
    BUILD_BYPRODUCTS "${_transport_prefix}/lib/libcares.a")

"""
    transport = replace_once(
        transport,
        "set(_curl_dependencies tilefinch_psp_mbedtls)",
        cares_project + "set(_curl_dependencies tilefinch_psp_mbedtls tilefinch_psp_cares)",
        "PSP c-ares external project")
    transport = replace_once(
        transport,
        'set(_curl_nghttp2_args "-DUSE_NGHTTP2=OFF")',
        'set(_curl_cares_args\n'
        '    "-DENABLE_ARES=ON"\n'
        '    "-DCARES_INCLUDE_DIR=${_transport_prefix}/include"\n'
        '    "-DCARES_LIBRARY=${_transport_prefix}/lib/libcares.a"\n'
        '    "-DCARES_USE_STATIC_LIBS=ON")\n'
        'set(_curl_nghttp2_args "-DUSE_NGHTTP2=OFF")',
        "PSP c-ares curl options")
    transport = replace_once(
        transport,
        '        ${_curl_nghttp2_args}',
        '        ${_curl_cares_args}\n        ${_curl_nghttp2_args}',
        "curl dependency options")
    transport = replace_once(
        transport,
        '        "-DCURL_DISABLE_DOH=ON"',
        '        "-DCURL_DISABLE_DOH=OFF"',
        "curl DoH build switch")
    transport = replace_once(
        transport,
        'add_dependencies(tilefinch_psp_transport tilefinch_psp_curl)',
        'add_dependencies(tilefinch_psp_transport tilefinch_psp_curl tilefinch_psp_cares)',
        "PSP transport dependency graph")
    transport = replace_once(
        transport,
        '    "${_transport_prefix}/lib/libcurl.a")',
        '    "${_transport_prefix}/lib/libcurl.a"\n'
        '    "${_transport_prefix}/lib/libcares.a")',
        "PSP transport link libraries")
    transport_path.write_text(transport, encoding="utf-8")

# 3) Keep the additional source archive in Tilefinch's existing verified lock.
lock = lock_path.read_text(encoding="utf-8")
lock_line = f"{'cares'}|{CARES_VERSION}|{CARES_ARCHIVE}|{CARES_SHA256}|{CARES_URL}"
if lock_line not in lock:
    lock_path.write_text(lock.rstrip() + "\n" + lock_line + "\n",
                         encoding="utf-8")

# 4) Carry c-ares' MIT license into the install tree and require it in the
# notices manifest, matching Tilefinch's redistribution policy.
stage = stage_path.read_text(encoding="utf-8")
if "CARES_SOURCE_DIR" not in stage:
    stage = replace_once(
        stage,
        "        OR NOT XMB_REDIRECT_PRX\n        OR NOT ASSET_DIR",
        "        OR NOT XMB_REDIRECT_PRX OR NOT CARES_SOURCE_DIR\n        OR NOT ASSET_DIR",
        "StagePspInstall required arguments")
    stage = replace_once(
        stage,
        '"XMB_REDIRECT_PRX, ASSET_DIR, SOURCE_DIR, and OUTPUT")',
        '"XMB_REDIRECT_PRX, CARES_SOURCE_DIR, ASSET_DIR, SOURCE_DIR, and OUTPUT")',
        "StagePspInstall argument message")
    stage = replace_once(
        stage,
        '        "${SOURCE_DIR}/psp-assets/voice-model/en-us/README")',
        '        "${SOURCE_DIR}/psp-assets/voice-model/en-us/README"\n'
        '        "${CARES_SOURCE_DIR}/LICENSE.md")',
        "StagePspInstall required files")
    stage = replace_once(
        stage,
        'file(MAKE_DIRECTORY "${OUTPUT}/NOTICES")',
        'file(MAKE_DIRECTORY "${OUTPUT}/NOTICES/c-ares")\n'
        'file(COPY_FILE "${CARES_SOURCE_DIR}/LICENSE.md"\n'
        '    "${OUTPUT}/NOTICES/c-ares/LICENSE.md")',
        "StagePspInstall c-ares license staging")
    stage_path.write_text(stage, encoding="utf-8")

targets = targets_path.read_text(encoding="utf-8")
if "CARES_SOURCE_DIR=" not in targets:
    targets = replace_once(
        targets,
        '"-DOUTPUT=${TILEFINCH_PSP_INSTALL_TREE}"',
        '"-DOUTPUT=${TILEFINCH_PSP_INSTALL_TREE}"\n'
        '                "-DCARES_SOURCE_DIR=${CMAKE_CURRENT_BINARY_DIR}/psp-transport/cares/src/tilefinch_psp_cares"',
        "Tilefinch install target c-ares source path")
    targets_path.write_text(targets, encoding="utf-8")

manifest = manifest_path.read_text(encoding="utf-8")
if "NOTICES/c-ares/LICENSE.md" not in manifest:
    manifest = replace_once(
        manifest,
        "    NOTICES/curl/COPYING",
        "    NOTICES/curl/COPYING\n    NOTICES/c-ares/LICENSE.md",
        "PSP notices manifest")
    manifest_path.write_text(manifest, encoding="utf-8")

notices = notices_path.read_text(encoding="utf-8")
if "c-ares 1.34.9" not in notices:
    notices = replace_once(
        notices,
        "- **libcurl 8.22.0**:",
        "- **c-ares 1.34.9**: MIT License. The PSP-owned libcurl build uses c-ares as its asynchronous resolver so DNS-over-HTTPS can run without a POSIX resolver thread; the license is staged at `NOTICES/c-ares/LICENSE.md`.\n"
        "- **libcurl 8.22.0**:",
        "third-party notices c-ares attribution")
    notices_path.write_text(notices, encoding="utf-8")

print("Applied Tilefinch PSP Cloudflare DoH overlay with pinned c-ares async resolver")
