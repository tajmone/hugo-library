!----------------------------------------------------------------------------
!
!         Hugo Verb Grammar v3.1.03.2 by Kent Tessman (c) 1995-2006
!                      for use with the Hugo Compiler
!
!----------------------------------------------------------------------------
!
! All verb grammar must be defined or included at the start of a game file,
! before any objects or executable code.
!
!----------------------------------------------------------------------------

#ifclear _VERBLIB_G
#set _VERBLIB_G

!----------------------------------------------------------------------------
! NON-ACTION VERBS:
!----------------------------------------------------------------------------

#ifclear NO_XVERBS

xverb "load"
        * "game"/"story"                                        DoRestore

xverb "restore", "resume"
        *                                                       DoRestore
        * "game"/"story"                                        DoRestore

xverb "save", "suspend"
        *                                                       DoSave
        * "game"/"story"                                        DoSave

xverb "script", "transcript", "transcription"
        *                                                       DoScriptOnOff
        * "on"/"off"                                            DoScriptOnOff

#ifclear NO_RECORDING
xverb "record"
        *                                                       DoRecordOnOff
        * "on"/"off"                                            DoRecordOnOff

xverb "playback"
        *                                                       DoRecordOnOff
#endif

xverb "quit", "q"
        *                                                       DoQuit
        * "game"                                                DoQuit

xverb "restart"
        *                                                       DoRestart
        * "game"                                                DoRestart

xverb "brief", "normal"
        *                                                       DoBrief

xverb "superbrief", "short"
        *                                                       DoSuperBrief

xverb "verbose", "long"
        *                                                       DoVerbose

xverb "display"
        *                                                       DoDisplay
        * "wide"/"tall"                                         DoDisplay

xverb "wide", "tall"
        *                                                       DoDisplay

xverb "score"
        *                                                       DoScore

#ifclear NO_UNDO
xverb "undo"
        *                                                       DoUndo
#endif

#endif  ! ifclear NO_XVERBS

!----------------------------------------------------------------------------
! ACTION VERBS:
!----------------------------------------------------------------------------

#ifclear NO_VERBS

!
! For looking and examining:
!

verb "examine", "x", "watch"
        *                                                       DoVague

verb "look", "l", "examine", "x", "watch"
        *                                                       DoLookAround
        * "around"                                              DoLookAround
        * "in"/"inside" container                               DoLookIn
        * "on" platform                                         DoLookIn
        * "at"/"to" object                                      DoLook
        * "out"/"through" object                                DoLookThrough
        * "under"/"underneath"/"beneath"/"below" object         DoLookUnder
        * "beside"/"behind"/"around" object                     DoLookUnder
        * object                                                DoLook

!
! For waiting around:
!

verb "wait", "z"
        *                                                       DoWait
        * "here"/"there"                                        DoWait
        * "for" anything                                        DoWaitforChar
        * "until" number                                        DoWaitUntil
        * "until" "turn" number                                 DoWaitUntil
        * "for" number "minutes"/"turns"/"turn"/"minute"        DoWait
        * number                                                DoWait
        * number "minutes"/"turns"/"turn"/"minute"              DoWait


!
! For moving around:
!

verb "north", "n", "south", "s", "east", "e", "west", "w", \
        "southeast", "se", "southwest", "sw", "northwest", "nw", \
        "northeast", "ne", "up", "u", "down", "d", "in", "out", \
        "inside", "outside"
        *                                                       DoGo

verb "go", "walk"
        *                                                       DoGo
        * "to"/"in"/"into"/"inside"/"through" object            DoEnter
        * "out" object                                          DoGo
        * "out"/"outside"                                       DoExit
        * object                                                DoGo

verb "enter", "board", "mount"
        *                                                       DoVague
        * object                                                DoEnter

verb "exit"
        *                                                       DoExit
        * object                                                DoExit

verb "sit", "lie"
        *                                                       DoEnter
        * "down"                                                DoSit
        * "in"/"on" object                                      DoSit
        * "down" "in"/"on" object                               DoSit

verb "stand"
        *                                                       DoStand
        * "up"                                                  DoStand


verb "dismount"
        *                                                       DoStand
        * object                                                DoExit


!
! For moving, taking, and dropping objects:
!

verb "move"
        *                                                       DoVague
        * object                                                DoMove

verb "get", "step"
        *                                                       DoVague
        * "up"/"out"/"off"/"down"                               DoStand
        * "outof"/"offof"/"off" object                          DoExit
        * "in"/"inside"/"on"                                    DoEnter
        * "in"/"into"/"inside"/"on"/"onto" object               DoEnter

verb "take"
        *                                                       DoVague
        * "inventory"                                           DoInventory
        * "off" multiheld                                       DoTakeOff
        * multiheld "off"                                       DoTakeOff

verb "get", "take", "grab", "snatch", "fetch"
        *                                                       DoVague
        * multi "from"/"off"/"on"/"in" parent                   DoGet
        * multi "offof"/"outof" parent                          DoGet
        * multi "from" "offof"/"outof"/"on"/"in" parent         DoGet
        * multi                                                 DoGet

verb "pick", "lift"
        *                                                       DoVague
        * "up" multi                                            DoGet
        * "up" multi "from"/"off" parent                        DoGet
        * "up" multi "offof"/"outof" parent                     DoGet
        * multi "up"                                            DoGet
        * multi "up" "from"/"off" parent                        DoGet
        * multi "up" "offof"/"outof" parent                     DoGet
        * multi                                                 DoGet

verb "drop"
        *                                                       DoVague
#ifset USE_CHECKHELD
        * multi "on" "ground"/"floor"                           DoPutonGround_CheckHeld
        * multi "outside" xobject                               DoPutonGround_CheckHeld
        * multi "in"/"into"/"inside" container                  DoPutIn_CheckHeld
        * multi "on" platform                                   DoPutIn_CheckHeld
        * multi                                                 DoDrop_CheckHeld
#else
        * multiheld "on" "ground"/"floor"                       DoPutonGround
        * multiheld "outside" xobject                           DoPutonGround
        * multiheld "in"/"into"/"inside" container              DoPutIn
        * multiheld "on" platform                               DoPutIn
        * multiheld                                             DoDrop
#endif

verb "leave"
        *                                                       DoExit
#ifset USE_CHECKHELD
        * multi "on"/"onto" "ground"/"floor"                    DoPutonGround_CheckHeld
        * multi "outside" xobject                               DoPutonGround_CheckHeld
        * multi "in"/"inside" container                         DoPutIn_CheckHeld
        * multi "on" platform                                   DoPutIn_CheckHeld
! Send >LEAVE OBJECT to DoDropOrExit, which dispatches to DoDrop or DoExit
!       * multi                                                 DoDrop_CheckHeld
!       * object                                                DoExit_CheckHeld
#else
        * multiheld "on"/"onto" "ground"/"floor"                DoPutonGround
        * multiheld "outside" xobject                           DoPutonGround
        * multiheld "in"/"inside" container                     DoPutIn
        * multiheld "on" platform                               DoPutIn
! Send >LEAVE OBJECT to DoDropOrExit, which dispatches to DoDrop or DoExit
!       * multiheld                                             DoDrop
!       * object                                                DoExit
#endif
        * object                                                DoDropOrExit

verb "let"
        * "go" multi                                            DoDrop
        * multi "go"                                            DoDrop

verb "put", "place", "set"
        *                                                       DoVague
#ifset USE_CHECKHELD
        * "down" multi                                          DoDrop_CheckHeld
        * multi "on"/"onto" "ground"/"floor"                    DoPutonGround_CheckHeld
        * multi "outside" xobject                               DoPutonGround_CheckHeld
        * multi "down"                                          DoDrop_CheckHeld
        * multi "in"/"into"/"inside" container                  DoPutIn_CheckHeld
        * multi "on"/"onto" platform                            DoPutIn_CheckHeld
        * multi                                                 DoDrop_CheckHeld
#else
        * "down" multiheld                                      DoDrop
        * multiheld "on"/"onto" "ground"/"floor"                DoPutonGround
        * multiheld "outside" xobject                           DoPutonGround
        * multiheld "down"                                      DoDrop
        * multiheld "in"/"into"/"inside" container              DoPutIn
        * multiheld "on"/"onto" platform                        DoPutIn
        * multiheld                                             DoDrop
#endif

verb "put"
        * "on" multi                                            DoWear
        * multi "on"                                            DoWear

verb "insert"
        *                                                       DoVague
#ifset USE_CHECKHELD
        * object "in"/"into" object                             DoPutIn_CheckHeld
        * object                                                DoPutIn_CheckHeld
#else
        * held "in"/"into" object                               DoPutIn
        * held                                                  DoPutIn
#endif

verb "empty", "unload"
        *                                                       DoVague
        * object "on"/"onto" "ground"/"floor"                   DoEmptyGround
        * multi "from"/"off"/"on"/"in" parent                   DoGet
        * multi "offof"/"outof" parent                          DoGet
        * multi "from" "offof"/"outof"/"on"/"in" parent         DoGet
! Send >UNLOAD OBJECT to DoEmptyoOrGet, which dispatches to DoEmpty or DoGet
!       * object                                                DoEmpty
        * object                                                DoEmptyOrGet


!
! Other object-handling verbs:
!

verb "give", "hand", "offer", "pass"
        *                                                       DoVague
        * "me" object                                           DoGive
        * object "to" xobject                                   DoGive
        * object                                                DoGive

verb "show"
        *                                                       DoVague
        * "me" object                                           DoShow
        * object "to" xobject                                   DoShow
        * object                                                DoShow

verb "inventory", "inv", "i"
        *                                                       DoInventory
        * "wide"/"tall"                                         DoInventory

verb "open"
        *                                                       DoVague
        * openable                                              DoOpen

verb "lock"
        *                                                       DoVague
        * lockable "with"/"using" held                          DoLock
        * lockable                                              DoLock

verb "unlock"
        *                                                       DoVague
        * lockable "with"/"using" held                          DoUnlock
        * lockable                                              DoUnlock

verb "close", "shut"
        *                                                       DoVague
        * openable                                              DoClose

verb "read", "peruse"
        *                                                       DoVague
        * readable                                              DoLook

verb "switch", "turn", "flip"
        *                                                       DoVague
        * "on" switchable                                       DoSwitchOn
        * "off" switchable                                      DoSwitchOff
        * switchable "on"                                       DoSwitchOn
        * switchable "off"                                      DoSwitchOff
        * switchable                                            DoSwitchOnOff

verb "activate", "start"
        *                                                       DoVague
        * switchable                                            DoSwitchOn

verb "deactivate", "stop"
        *                                                       DoVague
        * switchable                                            DoSwitchOff

verb "wear"
        *                                                       DoVague
        * multi                                                 DoWear

verb "remove"
        *                                                       DoVague
        * multi "from"/"outof"/"offof" parent                   DoGet
        * multi                                                 DoTakeOff

verb "listen"
        *                                                       DoListen
        * "to" object                                           DoListen

verb "hear"
        *                                                       DoListen
        * object                                                DoListen

verb "eat", "taste", "bite", "chew"
        *                                                       DoVague
        * object                                                DoEat

verb "drink", "sip", "swallow"
        *                                                       DoVague
        * "from" object                                         DoDrink
        * object                                                DoDrink

verb "hit", "strike", "break", "attack", "whack", "beat", \
        "punch", "kick", "clobber"
        *                                                       DoVague
        * object "with"/"using" held                            DoHit
        * object                                                DoHit

verb "kill", "murder", "fight"
        *                                                       DoVague
        * living                                                DoHit


!
! For talking to characters:
!

verb "hello", "hi", "howdy"
        *                                                       DoHello
        ! because a comma becomes "~and":
        * "~and" object                                         DoHello
        * object                                                DoHello

verb "ask", "question", "consult"
        *                                                       DoAsk
        * "about" anything                                      DoAskQuestion
        * living "about" anything                               DoAsk
        * living                                                DoAsk

verb "talk", "speak"
        *                                                       DoTalk
        * "to" living                                           DoTalk
        * "to" living "about" anything                          DoTalk
        * "about" anything                                      DoAskQuestion

verb "tell"
        * "me" "about" anything                                 DoAskQuestion
        * living "about" anything                               DoTell

verb "explain"
        * "to" "me" "about" anything                            DoAskQuestion
        * "to" living "about" anything                          DoTell

verb "what"
        * "is"/"about" anything                                 DoAskQuestion

verb "who"
        * "is" anything                                         DoAskQuestion

#endif  ! ifclear NO_VERBS

!----------------------------------------------------------------------------
! Include verb stub grammar if specified

#ifset VERBSTUBS
#include "verbstub.g"
#endif

!----------------------------------------------------------------------------
! Include debugging grammar if specified

#ifset DEBUG
#include "hugofix.g"
#endif

#ifset DEBUG_SMALL
#include "hugofix.g"
#endif

#endif  ! _VERBLIB_G
