'OHRRPGCE - Script command names
'(C) Copyright 1997-2020 James Paige, Ralph Versteegen, and Hamster Republic Productions
'Please read LICENSE.txt for GPL License details and disclaimer of liability

'Don't bother to update this file. It is only for scripts compiled with versions
'of HSpeak before 3H

STATIC cmd_default_names(437) as string*24 => {"noop","wait","waitforall","waitforhero",_
        "waitfornpc","suspendnpcs","suspendplayer","resumenpcs","resumeplayer",_
        "waitforkey","walkhero","showtextbox","checktag","settag","","usedoor",_
        "fightformation","getitem","deleteitem","leader","getmoney","losemoney",_
        "paymoney","unequip","forceequip","setheroframe","setnpcframe","suspendoverlay",_
        "playsong","stopsong","keyval","rankincaterpillar","showbackdrop","showmap",_
        "dismountvehicle","usenpc","","useshop","camerafollowshero","camerafollowsnpc",_
        "pancamera","focuscamera","waitforcamera","herox","heroy","npcx","npcy",_
        "suspendobstruction","resumeobstruction","suspendherowalls","suspendnpcwalls",_
        "resumeherowalls","walknpc","setherodirection","setnpcdirection",_
        "getdefaultweapon","setdefaultweapon","suspendcatapillar","resumecatapillar",_
        "waitfortextbox","equipwhere","teleporttomap","suspendrandomenemys",_
        "resumerandomenemys","getherostat","resumeoverlay","addhero","deletehero",_
        "swapouthero","swapinhero","roominactiveparty","lockhero","unlockhero",_
        "gameover","setdeathscript","fadescreenout","fadescreenin","showvalue",_
        "alternpc","shownovalue","currentmap","setherospeed","inventory","setherostat",_
        "suspendboxadvance","resumeboxadvance","advancetextbox","setheroposition",_
        "setnpcposition","swapbyposition","findhero","checkequipment","","","",_
        "resumenpcwalls","setheroz","readmapblock","writemapblock","readpassblock",_
        "writepassblock","npcdirection","herodirection","resetpalette","tweakpalette",_
        "readcolor","writecolor","updatepalette","seedrandom","greyscalepalette",_
        "setheropicture","setheropalette","getheropicture","getheropalette",_
        "readglobal","writeglobal","heroiswalking","","suspendcaterpillar",_
        "resumecaterpillar","npcreference","npcatspot","getnpcid","npccopycount",_
        "changenpcid","createnpc","destroynpc","teachspell","forgetspell","readspell",_
        "writespell","knowsspell","canlearnspell","herobyslot","herobyrank","puthero",_
        "putnpc","putcamera","heropixelx","heropixely","npcpixelx","npcpixely",_
        "camerapixelx","camerapixely","loadtileset","pickhero","renameherobyslot",_
        "readgeneral","writegeneral","","statusscreen","showminimap","spellsmenu",_
        "itemsmenu","equipmenu","","","ordermenu","teammenu","initmouse","mousepixelx",_
        "mousepixely","mousebutton","putmouse","mouseregion","npcatpixel","saveinslot",_
        "lastsaveslot","suspendrandomenemies","resumerandomenemies","savemenu",_
        "saveslotused","importglobals","exportglobals","loadfromslot","deletesave",_
        "runscriptbyid","npciswalking","readgmap","writegmap","mapwidth","mapheight",_
        "readnpc","setherolevel","giveexperience","herolevelled","spellslearnt",_
        "getmusicvolume","setmusicvolume","","setformationsong","heroframe","npcframe",_
        "npcextra","setnpcextra","","","playsound","pausesound","stopsound",_
        "systemhour","systemminute","systemsecond","currentsong","getheroname",_
        "setheroname","getitemname","getmapname","getattackname","getglobalstring",_
        "showstring","clearstring","appendascii","appendnumber","copystring",_
        "concatenatestrings","stringlength","deletechar","replacechar",_
        "asciifromstring","positionstring","setstringbit","getstringbit","stringcolor",_
        "stringx","stringy","systemday","systemmonth","systemyear","stringcompare",_
        "readenemydata","writeenemydata","trace","getsongname","loadmenu",_
        "keyispressed","soundisplaying","","searchstring","trimstring",_
        "stringfromtextbox","expandstring","joystickbutton","joystickaxis",_
        "waitforscancode","savemapstate","loadmapstate","resetmapstate",_
        "deletemapstate","partymoney","setmoney","setstringfromtable",_
        "appendstringfromtable","settileanimationoffset","gettileanimationoffset",_
        "animationstarttile","suspendmapmusic","resumemapmusic","checkherowall",_
        "checknpcwall","settimer","stoptimer","readtimer","getcolor","setcolor","rgb",_
        "extractcolor","mainmenu","loadpalette","totalexperience","experiencetolevel",_
        "experiencetonextlevel","setexperience","milliseconds","openmenu","readmenuint",_
        "writemenuint","readmenuitemint","writemenuitemint","createmenu","closemenu",_
        "topmenu","bringmenuforward","addmenuitem","deletemenuitem",_
        "getmenuitemcaption","setmenuitemcaption","getlevelmp","setlevelmp",_
        "bottommenu","previousmenu","nextmenu","menuitembyslot","previousmenuitem",_
        "nextmenuitem","selectedmenuitem","selectmenuitem","parentmenu","getmenuid",_
        "swapmenuitems","findmenuitemcaption","findmenuid","menuisopen","menuitemslot",_
        "outsidebattlecure","changetileset","layertileset","getformationsong",_
        "addenemytoformation","findenemyinformation","deleteenemyfromformation",_
        "formationslotenemy","formationslotx","formationsloty","setformationbackground",_
        "getformationbackground","lastformation","randomformation",_
        "formationsetfrequency","formationprobability","currenttextbox","getherospeed",_
        "loadherosprite","freesprite","placesprite","setspritevisible",_
        "setspritepalette","replaceherosprite","setspriteframe","loadwalkaboutsprite",_
        "replacewalkaboutsprite","loadweaponsprite","replaceweaponsprite",_
        "loadsmallenemysprite","replacesmallenemysprite","loadmediumenemysprite",_
        "replacemediumenemysprite","loadlargeenemysprite","replacelargeenemysprite",_
        "loadattacksprite","replaceattacksprite","loadbordersprite",_
        "replacebordersprite","loadportraitsprite","replaceportraitsprite",_
        "clonesprite","getspriteframe","spriteframecount","slicex","slicey","setslicex",_
        "setslicey","slicewidth","sliceheight","sethorizalign","setvertalign",_
        "sethorizanchor","setvertanchor","numberfromstring","sliceissprite",_
        "spritelayer","freeslice","firstchild","nextsibling","createcontainer",_
        "setparent","checkparentage","slicescreenx","slicescreeny","sliceiscontainer",_
        "createrect","sliceisrect","setslicewidth","setsliceheight","getrectstyle",_
        "setrectstyle","getrectfgcol","setrectfgcol","getrectbgcol","setrectbgcol",_
        "getrectborder","setrectborder","getrecttrans","setrecttrans",_
        "slicecollidepoint","slicecollide","slicecontains","clampslice",_
        "horizflipsprite","vertflipsprite","spriteishorizflipped","spriteisvertflipped",_
        "settoppadding","gettoppadding","setleftpadding","getleftpadding",_
        "setbottompadding","getbottompadding","setrightpadding","getrightpadding",_
        "fillparent","isfillingparent","slicetofront","slicetoback","lastchild",_
        "ysortchildren","setsortorder","sortchildren","previoussibling","getsortorder",_
        "getsliceextra","setsliceextra","getspritetype","getspritesetnumber",_
        "getspritepalette","suspendtimers","resumetimers","setslicevisible",_
        "getslicevisible","sliceedgex","sliceedgey","createtext","setslicetext",_
        "gettextcolor","settextcolor","getwrap","setwrap","sliceistext","gettextbg",_
        "settextbg","getoutline","setoutline","usemenuitem","sliceatpixel",_
        "findcollidingslice","parentslice","childcount","lookupslice"}
