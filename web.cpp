/* OHRRPGCE - Special Web/Emscripten/Js code. Not used by any other port
 * (C) Copyright 1997-2024 James Paige, Ralph Versteegen, and the OHRRPGCE Developers
 * Dual licensed under the GNU GPL v2+ and MIT Licenses. Read LICENSE.txt for terms and disclaimer of liability.
 */

#include "config.h"
#include <emscripten.h>

extern "C" {

//Syncs are asynchonous, so sometimes we might have to wait for them.
int sync_to_indexdb_pending = false;
int sync_from_indexdb_pending = false;

void sync_to_indexdb_done(){
  sync_to_indexdb_pending = false;
}

void sync_from_indexdb_done(){
  sync_from_indexdb_pending = false;
}

void web_mount_persistent_storage(const char *foldername) {
  sync_from_indexdb_pending = true;
  EM_ASM_({
    var persistDir = AsciiToString($0);
    var pathInfo = FS.analyzePath(persistDir);
    var isMounted = (pathInfo.object != null && pathInfo.object.mount.mountpoint == persistDir);
    
    if (isMounted) {
      console.log("web_mount_persistent_storage:", persistDir, "is already mounted");
    } else {
      if (!pathInfo.exists) {
        FS.mkdir(persistDir);
      }
      FS.mount(IDBFS, {}, persistDir);
      console.log("web_mount_persistent_storage: Mounted IDBFS:", persistDir);
    }
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
      Module.ccall('sync_from_indexdb_done', 'v');
    });
  }, foldername);
  // Wait for the sync to finish
  while (sync_from_indexdb_pending) {
    emscripten_sleep(5);
  }
}

void web_sync_persistent_storage() {
  sync_to_indexdb_pending = true;
  EM_ASM_({
    // Ask IDBFS mounts to save their contents for later sessions
    FS.syncfs(false, function(err) {
      if (err) {
        console.error("web_sync_persistent_storage: Failed syncing filesystem:", err);
      } else {
        console.log("web_sync_persistent_storage: syncfs to IndexDB succeeded");
      }
      Module.ccall('sync_to_indexdb_done', 'v');
    });
  });
}

void web_unmount_persistent_storage(const char *foldername) {
  // Sync before we unmount (this is just unnecessary caution, the files are already synced each time an RSAV is written)
  web_sync_persistent_storage();

  // Wait for the sync to finish
  while (sync_to_indexdb_pending) {
    emscripten_sleep(5);
  }
  
  EM_ASM_({
    var persistDir = AsciiToString($0);
    var pathInfo = FS.analyzePath(persistDir);
    var isMounted = (pathInfo.object != null && pathInfo.object.mount.mountpoint == persistDir);

    if (isMounted) {
      FS.unmount(persistDir);
      console.log("web_unmount_persistent_storage: unmounted", persistDir);
    } else {
      console.log("web_unmount_persistent_storage:", persistDir, "was not mounted, so we can't unmount it");
    }

  }, foldername);
}

void web_open_url(const char *url){
  // The Browser's pop-up blocker might not allow this, but at least it will probably notify the user
  // and give them the option to permit it.
  EM_ASM_({
    var url = AsciiToString($0);
    window.open(url, "_blank");
  }, url);
}

} // end extern "C"
