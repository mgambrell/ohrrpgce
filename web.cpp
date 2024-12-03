/* OHRRPGCE - Special Web/Emscripten/Js code. Not used by any other port
 * (C) Copyright 1997-2024 James Paige, Ralph Versteegen, and the OHRRPGCE Developers
 * Dual licensed under the GNU GPL v2+ and MIT Licenses. Read LICENSE.txt for terms and disclaimer of liability.
 */

#include <emscripten.h>

void init_web_persistent_storage() {
  EM_ASM({
    var savesDir = '/saves';
    FS.mkdir(savesDir);
    FS.mount(IDBFS, {}, savesDir);
  });
}

