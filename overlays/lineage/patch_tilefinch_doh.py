#!/usr/bin/env python3
"""Apply LineageOS PSP's fail-closed Cloudflare DoH transport overlay to Tilefinch."""

from pathlib import Path
import sys

if len(sys.argv) != 2:
    raise SystemExit("usage: patch_tilefinch_doh.py <tilefinch>/src/fetch.c")

path = Path(sys.argv[1])
source = path.read_text(encoding="utf-8")

declaration = """#if defined(__PSP__) && defined(TILEFINCH_PSP_OWNED_TRANSPORT)
/* Bootstrap only the DoH endpoint itself. All other host lookups go through
   Cloudflare DoH; keeping this list static also keeps it alive for every
   libcurl easy handle that uses the resolver. */
static char fetch_doh_bootstrap_entry[] =
    "cloudflare-dns.com:443:1.1.1.1";
static struct curl_slist fetch_doh_bootstrap = {
    fetch_doh_bootstrap_entry, NULL
};
#endif

"""

function_marker = "static bool fetch_configure_tls(CURL *easy)\n{\n"
if source.count(function_marker) != 1:
    raise SystemExit("Tilefinch fetch_configure_tls() layout changed; refusing an unsafe patch")

if "CURLOPT_DOH_URL" in source or "fetch_doh_bootstrap" in source:
    if "CURLOPT_DOH_URL" in source and "static struct curl_slist fetch_doh_bootstrap =" in source:
        print("Tilefinch DoH overlay already present")
        raise SystemExit(0)
    raise SystemExit("Partial DoH overlay detected; refusing to patch")

source = source.replace(function_marker, declaration + function_marker, 1)

tls_check = """        || curl_easy_setopt(easy, CURLOPT_SSL_VERIFYHOST, 2L) != CURLE_OK) {
        return false;
    }
"""
doh_config = tls_check + """#if defined(__PSP__) && defined(TILEFINCH_PSP_OWNED_TRANSPORT)
    /* CURLOPT_RESOLVE pins the DoH server bootstrap address so resolving
       cloudflare-dns.com does not fall back to plaintext system DNS. The
       DoH endpoint's hostname is retained for normal certificate checks. */
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
if source.count(tls_check) != 1:
    raise SystemExit("Tilefinch TLS configuration changed; refusing an unsafe patch")

source = source.replace(tls_check, doh_config, 1)
if source.count("CURLOPT_DOH_URL") != 1 or source.count("static struct curl_slist fetch_doh_bootstrap =") != 1 or source.count("&fetch_doh_bootstrap") != 1:
    raise SystemExit("DoH overlay verification failed")

path.write_text(source, encoding="utf-8")
print("Applied fail-closed Cloudflare DNS-over-HTTPS overlay to Tilefinch")
