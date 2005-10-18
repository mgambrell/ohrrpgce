'OHRRPGCE GAME - Compatibility functions, FreeBasic version
'(C) Copyright 1997-2005 James Paige and Hamster Republic Productions
'Please read LICENSE.txt for GPL License details and disclaimer of liability
'See README.txt for code docs and apologies for crappyness of this code ;)
'
'$DYNAMIC
DEFINT A-Z
'$include: 'compat.bi'
'$include: 'allmodex.bi'
'$include: 'gglobals.bi'

DECLARE SUB fatalerror (e$)
DECLARE FUNCTION small% (n1%, n2%)
declare sub debug (m$)

SUB getdefaultfont(font() as integer)
	IF isfile(progdir$ + "ohrrpgce.fnt" + CHR$(0)) THEN
	 	DEF SEG = VARSEG(font(0)): BLOAD progdir$ + "ohrrpgce.fnt", VARPTR(font(0))
	ELSE
	 	'--load the ROM font
	 	regs.ax = &H1130
	 	regs.bx = &H300
	 	CALL interruptx(&H10, regs, regs)
	 	'off9 = regs.bx: seg9 = regs.es
	 	DEF SEG = regs.es
	 	FOR i = 1 TO 255
	  		FOR j = 0 TO 7
	   			b = PEEK(regs.bp + (8 * i) + j)
	   			FOR k = 0 TO 7
	    			setbit font(), i * 4, (7 - k) * 8 + j, (b AND 2 ^ k)
	   			NEXT k
	  		NEXT j
	 	NEXT i
	END IF
END SUB

SUB xbload (f$, array(), e$)
	IF isfile(f$ + CHR$(0)) THEN
	 	DEF SEG = VARSEG(array(0)): BLOAD f$, VARPTR(array(0))
	ELSE
	 	fatalerror e$
	END IF
END SUB

SUB crashexplain
	PRINT "Please report this exact error message to ohrrpgce@HamsterRepublic.com"
	PRINT "Be sure to describe in detail what you were doing when it happened"
	PRINT
	PRINT version$
	PRINT "Memory Info:"; SETMEM(0); FRE(-1); FRE(-2); FRE(0)
	PRINT "Executable: "; progdir$ + exename$ + ".EXE"
	PRINT "RPG file: "; sourcerpg$
END SUB

SUB fbdim (v%)
'dummy sub for compatibility
END SUB