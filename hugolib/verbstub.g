!----------------------------------------------------------------------------
!
!  Grammar for Verb Stub Routines v3.1.03.2 by Kent Tessman (c) 1995-2006
!                    for use with the Hugo Compiler
!
!----------------------------------------------------------------------------
!
! This grammar file must be included along with the standard grammar file
! at the start of the game file (before any executable code); GRAMMAR.G
! does this if the VERBSTUBS flag is set.
!
!----------------------------------------------------------------------------

#ifclear _VERBSTUB_G
#set _VERBSTUB_G

xverb "yes"
        *                                       DoYes

xverb "no"
        *                                       DoNo

xverb "sorry"
        *                                       DoSorry

verb "use"
        *                                       DoVague
        * object "with"/"on"/"in" xobject       DoUse
        * object                                DoUse

verb "smell", "sniff", "inhale", "breathe"
        *                                       DoSmell
        * object                                DoSmell

verb "jump", "leap", "hop"
        *                                       DoJump

verb "wave"
        *                                       DoWaveHands
        * "hands"                               DoWaveHands
        * "to" object                           DoWaveHands
        * held                                  DoWave

verb "throw", "hurl", "toss", "pitch"
        *                                       DoVague
        * held                                  DoThrowAt
        * held "at"/"to" xobject                DoThrowAt

verb "climb"
        *                                       DoClimb
        * "in"/"into"/"inside" object           DoEnter
        * "in"/"inside"                         DoEnter
        * "on"/"onto" object                    DoEnter
        * "off"/"offof"/"outof" object          DoExit
        * "down"/"out"/"outside"                DoClimbOut
        * "up"/"down" object                    DoClimb
        * object                                DoClimb

verb "sleep", "rest", "nap", "snooze"
        *                                       DoSleep

verb "push", "shove", "press"
        *                                       DoVague
        * "on" object                           DoPush
        * object                                DoPush

verb "pull", "yank", "tug"
        *                                       DoVague
        * "on" object                           DoPull
        * object                                DoPull

verb "kiss", "hug"
        *                                       DoVague
        * object                                DoKiss

verb "swim", "dive"
        *                                       DoSwim

verb "wake", "awake", "awaken"
        *                                       DoWake
        * "up"                                  DoWake
        * "up" living                           DoWakeCharacter
        * living "up"                           DoWakeCharacter
        * living                                DoWakeCharacter

verb "touch", "feel"
        *                                       DoVague
        * object                                DoTouch

verb "tie", "attach", "fasten", "lash"
        *                                       DoVague
        * object "to"/"on" xobject              DoTie
        * object "with"/"using" held            DoTieR
        * object                                DoTie

verb "untie", "detach", "unfasten", "unlash"
        *                                       DoVague
        * object "from" xobject                 DoUntie
        * object                                DoUntie

verb "burn", "light", "incinerate"
        *                                       DoVague
        * object "with" held                    DoBurn
        * object "from" held                    DoBurn
        * object                                DoBurn

verb "set"
        * "fire" "to" object                    DoBurn
        * object "on" "fire"                    DoBurn
        * object "ablaze"                       DoBurn

verb "cut", "slice", "chop", "sever"
        *                                       DoVague
        * object "with" held                    DoCut
        * object                                DoCut

verb "dig", "burrow"
        *                                       DoVague
        * "in"/"into" object                    DoDig
        * "in"/"into" object "with" held        DoDig
        * object "with" held                    DoDig
        * object                                DoDig

verb "yell", "shout", "scream", "bellow", "cry", "call", "holler"
        *                                       DoYell

verb "search"
        *                                       DoVague
        * "for" object                          DoVague
        * "under"/"underneath"/"beneath"/"below" object DoLookUnder
        * "in" object                           DoSearch
        * object                                DoSearch

verb "follow", "tail"
        *                                       DoVague
        * anything                              DoFollow

xverb "help"
        *                                       DoHelp
        * object                                DoHelpChar

#endif  ! _VERBSTUB_G
