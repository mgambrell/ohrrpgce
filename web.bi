'OHRRPGCE - Special Web/Emscripten/Js code. Not used by any other port
'(C) Copyright 1997-2024 James Paige, Ralph Versteegen, and the OHRRPGCE Developers
'Dual licensed under the GNU GPL v2+ and MIT Licenses. Read LICENSE.txt for terms and disclaimer of liability.

'Web emscripten specific os functions

#ifndef WEB_BI
#define WEB_BI
#ifdef __FB_JS__

extern "C"

declare sub web_mount_persistent_storage(byval foldername as zstring ptr)
declare sub web_sync_persistent_storage()
declare sub web_unmount_persistent_storage(byval foldername as zstring ptr)

'Don't call this one directly, use url_open() instead.
declare sub web_open_url(byval url as zstring ptr)

end extern

#endif
#endif
