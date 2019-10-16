!----------------------------------------------------------------------------
!
!           Verb Stub Routines v3.1.03.2 by Kent Tessman (c) 1995-2006
!                       for use with the Hugo Compiler
!
!----------------------------------------------------------------------------
!
! These routines provide default responses only; in most cases this amounts
! to a more colorful variation on "Try something else."  Any more meaningful
! response must be incorporated into the before property routines of the
! objects involved.  HUGOLIB.H includes this file if the VERBSTUBS flag
! is set.
!
!----------------------------------------------------------------------------

#ifclear _VERBSTUB_H
#set _VERBSTUB_H

#ifclear _COMPILING_HUGOLIB
#message warning "Verb stub routines not #included; #set VERBSTUBS instead"
#else

routine DoYes
{
        "That was a rhetorical question.  (Although you sound rather positive.)"
}

routine DoNo
{
        "That was a rhetorical question.  (Although you sound rather negative.)"
}

routine DoSorry
{
        "Just don't let it happen again."
}

routine DoUse
{
        "Be a little more specific about what you'd like ";
        if player_person ~= 2
                print player.pronoun #2; " ";
        "to do with ";
        print The(object, true); "."
}

routine DoSmell
{
        print capital player.pronoun #1;
        MatchPlural(player, "doesn't", "don't")
        " smell anything unusual."
        return true
}

routine DoJump
{
        print capital player.pronoun #1;
        " jump";
        MatchSubject(player)
        " up and down, accomplishing little."
        return true
}

routine DoWaveHands
{
        print capital player.pronoun #1;
        " wave";
        MatchSubject(player)
        ".  Nothing happens."
        return true
}

routine DoWave
{
        print capital player.pronoun #1;
        " wave";
        MatchSubject(player)
        print " "; The(object, true);
        ".  Nothing happens."
        return true
}

routine DoThrowAt
{
        if not xobject
        {
                "You'll have to be a little more specific about what
                you'd like ";
                if player_person ~= 2
                        print player.pronoun #2; " ";
                "to throw ";
                print The(object, true); " at."
        }
        elseif xobject is not living
        {
                "What exactly ";
                if player_person = 2
                        "are you";
                else
                        print "should "; player.pronoun #1; " be";
                " hoping for?"
        }
        else
        {
                print capital player.pronoun #1;
                " ultimately decide";
                MatchSubject(player)
                " that throwing ";
                print The(object, true); " at "; The(xobject, true);
                " isn't such a great idea."
        }
        return true
}

routine DoClimb
{
        if object = u_obj
        {
                Perform(&DoGo, u_obj)
        }
        elseif object
        {
                print capital player.pronoun #1;
                " can't climb ";
                print The(object, true); "."
        }
        else:  "Not here; not now."
        return true
}

routine DoClimbOut
{
        if player not in location
                return Perform(&DoExit)
        else
        {
                CThe(player)
                MatchPlural(player, "isn't", "aren't")
                " in anything at the moment."
        }
}

routine DoSleep
{
        "Not now--there's work to be done."
}

routine DoPush
{
        if not CheckReach(object):  return false

        "Pushing ";
        The(object, true)
        " doesn't get ";
        print player.pronoun #2;
        " anywhere."
        return true
}

routine DoPull
{
        if not CheckReach(object):  return false

        "Pulling ";
        The(object, true)
        " doesn't get ";
        print player.pronoun #2;
        " anywhere."
        return true
}

routine DoKiss
{
        if not CheckReach(object):  return false

        if object is not living
                "You need to get out more."
        else
        {
                CThe(object)
                MatchPlural(object, "doesn't", "don't")
                " seem to share ";
                print player.pronoun #3;
                " affection."
                return true
        }
}

routine DoSwim
{
        "Not here, you won't."
}

routine DoWake
{
        print capital player.pronoun #1;
        if player_person = 1
                " am";
        elseif player is plural
                " are";
        else
                " is";
        " already awake, like it or not."
}

routine DoWakeCharacter
{
        "That's probably not necessary."
}

routine DoTouch
{
        if not CheckReach(object):  return false

        "Fiddling with ";
        The(object, true)
        " like that probably isn't the best use of ";
        print player.pronoun #3;
        " time at the moment."
}

routine DoTie
{
        if not CheckReach(object):  return false

        "Trying to tie ";
        The(object, true)
        " doesn't get ";
        print player.pronoun #2;
        " anywhere."
}

routine DoTieR
{
        return Perform(&DoTie, xobject, object)
}

routine DoUntie
{
        if not CheckReach(object):  return false

        "Trying to untie ";
        The(object, true)
        " doesn't get ";
        print player.pronoun #2;
        " anywhere."
}

routine DoBurn
{
        "You didn't learn anything as a child, did you?  Don't play
        with fire.  And stay away from my house."
}

routine DoCut
{
        "A destructive and somewhat less than completely useful
        course of action."
}

routine DoDig
{
        "That won't accomplish anything."
}

routine DoYell
{
        print capital player.pronoun #1;
        " let";
        MatchPlural(player)
        "out a hearty bellow.  Nothing else happens."
        return true
}

routine DoSearch
{
        if object = player
        {
                "Search ";
                The(player)
                " indeed."
        }
        elseif object is container and child(object)
                Perform(&DoLookIn, object)
        elseif object is living
        {
                print CThe(object); MatchPlural(object, "doesn't", "don't");
                " let ";
                The(player)
                if object.pronouns #2
                {
                        " search ";
                        if object.pronouns #2
                                print object.pronouns #2;
                        else
                                print object.pronoun;
                }
                print "."
        }
        else
        {
                CThe(player)
                MatchPlural(player, "doesn't", "don't")
                " find anything new."
        }
        return true
}

routine DoFollow
{
        if object = player
        {
                if speaking
                        SpeakTo(speaking)
                else
                {
                        if player_person = 2
                                "Who are you talking to?"
                        else
                        {
                                "It's not obvious who you want ";
                                print player.pronoun #2;
                                " to talk to."
                        }
                }
        }
        elseif object in location
                print CThe(object); IsorAre(object, true); " right here."
        else
                print "Which way did "; object.pronoun; " go?"
}

routine DoHelp
{
        "Unfortunately, 'twould seem ";
        if player_person = 1
                "we're on our own."
        else
                "you're on your own."
}

routine DoHelpChar
{
        if object = player
        {
                Perform(&DoHelp)
                return
        }
        elseif object is living
        {
                "How exactly would you like ";
                if player_person ~= 2
                        print player.pronoun #2;
                " to try to help ";
                print The(object, true); "?"
        }
        else
        {
                print CThe(object); IsorAre(object, true);
                " not in need of ";
                print player.pronoun #3;
                " help at the moment."
        }
}

#endif  ! _COMPILING_HUGOLIB

#endif  ! _VERBSTUB_H
