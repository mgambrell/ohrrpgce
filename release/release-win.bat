CD ..

REM grab a copy of the pdb tools from the wip/support folder
support\cp ../../wip/support/mspdbsrv.exe ../../wip/support/ms*.dll support

REM Build the release
CALL distrib-win.bat

REM Upload the release
REM This only works if you have Putty's pscp.exe in your path.
pscp distrib/ohrrpgce-*.zip james_paige@motherhamster.org:HamsterRepublic.com/ohrrpgce/archive/
pscp distrib/ohrrpgce-*.exe james_paige@motherhamster.org:HamsterRepublic.com/ohrrpgce/archive/
pscp distrib/ohrrpgce-symbols-*.7z james_paige@motherhamster.org:HamsterRepublic.com/ohrrpgce/symbols-archive/
