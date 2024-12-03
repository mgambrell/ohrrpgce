/* OHRRPGCE - Special Web/Emscripten/Js code. Not used by any other port
 * (C) Copyright 1997-2024 James Paige, Ralph Versteegen, and the OHRRPGCE Developers
 * Dual licensed under the GNU GPL v2+ and MIT Licenses. Read LICENSE.txt for terms and disclaimer of liability.
 */

#include "config.h"
#include <emscripten.h>

extern "C" {

void web_mount_persistent_storage(const char *foldername) {
  EM_ASM_({
    var persistDir = UTF8ToString($0);
    var pathInfo = FS.analyzePath(persistDir);
    var isMounted = (pathInfo.object != null && pathInfo.object.mount.mountpoint == persistDir);
    
    if (isMounted) {
      console.log("web_mount_persistent_storage:", persistDir, "is already mounted");
    } else {
      FS.mkdir(persistDir);
      FS.mount(IDBFS, {}, persistDir);
    }
    console.log("web_mount_persistent_storage: Mounted IDBFS:", persistDir);
    // Ask IDBFS to populate the mount with previously saved persistent files
    FS.syncfs(true, function(err) {
      if (err) {
        console.error("web_mount_persistent_storage: Failed syncing filesystem:", err);
      } else {
        console.log("web_mount_persistent_storage: syncfs from IndexDB succeeded");
        var filestAtMount = FS.readdir(persistDir);
        filestAtMount = filestAtMount.filter(item => item !== "." && item !== "..");
        console.log('web_mount_persistent_storage: Folder contents:', filestAtMount);
      }
    });
  }, foldername);
}

void web_sync_persistent_storage() {
  EM_ASM_({
  // Ask IDBFS mounts to save their contents for later sessions
    FS.syncfs(false, function(err) {
      if (err) {
        console.error("web_sync_persistent_storage: Failed syncing filesystem:", err);
      } else {
        console.log("web_sync_persistent_storage: syncfs to IndexDB succeeded");
      }
    });
  });
}

} // end extern "C"
