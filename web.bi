'OHRRPGCE - Special Web/Emscripten/Js code. Not used by any other port
'(C) Copyright 1997-2024 James Paige, Ralph Versteegen, and the OHRRPGCE Developers
'Dual licensed under the GNU GPL v2+ and MIT Licenses. Read LICENSE.txt for terms and disclaimer of liability.

'Web emscripten specific os functions

#ifndef WEB_BI
#define WEB_BI
#ifdef __FB_JS__

extern "C"

declare sub init_web_persistent_storage ()

end extern

#endif
#endif
