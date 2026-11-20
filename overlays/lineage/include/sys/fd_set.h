#ifndef LINEAGE_PSP_SYS_FD_SET_H
#define LINEAGE_PSP_SYS_FD_SET_H

/*
 * Compatibility shim for OSLib's legacy include path.
 * Current PSPDEV exposes fd_set and FD_* through sys/select.h.
 */
#include <sys/select.h>

#endif
