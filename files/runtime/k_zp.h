/* k_zp.h — Katlans Zip/Compression (zp prefix) */
#ifndef K_ZP_H
#define K_ZP_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/* Note: Full zip support requires zlib. For now, provide file-level pack/unpack
   using system tar/zip commands. On Windows, uses PowerShell Compress-Archive. */

static inline KVal *k_zppack(KVal *folder, KVal *out) {
    char cmd[8192];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd),
        "powershell -command \"Compress-Archive -Path '%s'/* -DestinationPath '%s' -Force\" 2>nul",
        folder->s, out->s);
#else
    snprintf(cmd, sizeof(cmd),
        "tar -czf '%s' -C '%s' . 2>/dev/null", out->s, folder->s);
#endif
    int rc = system(cmd);
    return kv_bool(rc == 0);
}

static inline KVal *k_zpunpack(KVal *file, KVal *outfolder) {
    char cmd[8192];
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd),
        "powershell -command \"Expand-Archive -Path '%s' -DestinationPath '%s' -Force\" 2>nul",
        file->s, outfolder->s);
#else
    snprintf(cmd, sizeof(cmd),
        "mkdir -p '%s' && tar -xzf '%s' -C '%s' 2>/dev/null",
        outfolder->s, file->s, outfolder->s);
#endif
    int rc = system(cmd);
    return kv_bool(rc == 0);
}

static inline KVal *k_zplist(KVal *file) {
    KVal *lst = kv_list();
    char cmd[8192];
    FILE *p;
#ifdef _WIN32
    snprintf(cmd, sizeof(cmd),
        "powershell -command \"(Get-ChildItem -Path '%s' -Recurse).FullName\"", file->s);
    p = popen(cmd, "r");
#else
    snprintf(cmd, sizeof(cmd), "tar -tzf '%s' 2>/dev/null", file->s);
    p = popen(cmd, "r");
#endif
    if (!p) return lst;
    char buf[4096];
    while (fgets(buf, sizeof(buf), p)) {
        size_t n = strlen(buf);
        if (n && buf[n-1] == '\n') buf[n-1] = '\0';
        if (strlen(buf) > 0) klist_push(lst->list, kv_str(buf));
    }
    pclose(p);
    return lst;
}

static inline KVal *k_zpadd(KVal *file, KVal *newfile) {
    /* Zip doesn't easily support adding individual files. Use system copy + repack. */
    (void)file; (void)newfile;
    return kv_bool(false);
}

static inline KVal *k_zprem(KVal *file, KVal *rmfile) {
    (void)file; (void)rmfile;
    return kv_bool(false);
}

#endif /* K_ZP_H */
