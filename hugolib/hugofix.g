!----------------------------------------------------------------------------
!
!       HugoFix Library Grammar v3.1.03.2 by Kent Tessman (c) 1995-2006
!                      for use with the Hugo Compiler
!
!----------------------------------------------------------------------------
!
! This grammar file is automatically included by GRAMMAR.G if the DEBUG
! flag is set.
!
! NOTE:  The following verb definitions follow the somewhat non-standard
! practice of each calling the same verb routine (DoHugoFix), primarily
! to cut down on the sheer number of verbs.  DoHugoFix is responsible for
! differentiating which debugging command is intended based on word[1].
!
!----------------------------------------------------------------------------

#ifclear _HUGOFIX_G
#set _HUGOFIX_G

xverb "$", "$ca", "$fd", "$on", "$pm", "$pr", "$sc"
        *                                       DoHugoFix

xverb "$ac"
        * anything                              DoHugoFix
        * anything number                       DoHugoFix
        * number                                DoHugoFix
        * number number                         DoHugoFix

verb "$at"
        * anything "is" number                  DoHugoFix
        * number "is" number                    DoHugoFix
        * anything "is" "not" number            DoHugoFix
        * number "is" "not" number              DoHugoFix

xverb "$au"
        *                                       HugoFixAudit

xverb "$de"
        * anything                              DoHugoFix
        * number                                DoHugoFix

#ifclear NO_OBJLIB
xverb "$di"
        *                                       DoHugoFix
        * number                                DoHugoFix
        * anything                              DoHugoFix
#endif

#ifclear NO_FUSES
xverb "$fd"
        *                                       DoHugoFix
#endif

xverb "$fi"
        *                                       DoHugoFix

xverb "$fo"
        *                                       DoHugoFix
        * anything                              DoHugoFix
        * number                                DoHugoFix

xverb "$kn"
        *                                       DoHugoFix
        * number                                DoHugoFix

xverb "$mo"
        * number "to" "$loc"                    DoHugoFix
        * anything "to" "$loc"                  DoHugoFix
        * number "to" number                    DoHugoFix
        * anything "to" number                  DoHugoFix
        * number "to" anything                  DoHugoFix
        * anything "to" anything                DoHugoFix

xverb "$mp"
        * anything                              DoHugoFix
        * number                                DoHugoFix

xverb "$na"
        * number                                DoHugoFix

xverb "$nr"
        *                                       DoHugoFix

xverb "$nu"
        * anything                              DoHugoFix

xverb "$on"
        *                                       DoHugoFix

xverb "$ot"
        *                                       DoHugoFix
        * number                                DoHugoFix
        * anything                              DoHugoFix
        * "$loc"                                DoHugoFix

xverb "$pc"
        *                                       DoHugoFix
        * "$all"                                DoHugoFix
        * "$all" anything                       DoHugoFix
        * anything                              DoHugoFix
        * "$all" number                         DoHugoFix
        * number                                DoHugoFix

xverb "$pm"
        *                                       DoHugoFix

xverb "$rr"
        *                                       DoHugoFix

#ifclear NO_SCRIPTS
xverb "$sc"
        *                                       DoHugoFix
#endif

xverb "$uk"
        * number                                DoHugoFix
        * anything                              DoHugoFix

xverb "$wn"
        * word                                  DoHugoFix

xverb "$wo"
        * number                                DoHugoFix

#endif  ! _HUGOFIX_G
