!----------------------------------------------------------------------------
!
!         Hugo Verb Library v3.1.03.2 by Kent Tessman (c) 1995-2006
!                     for use with the Hugo Compiler
!
!----------------------------------------------------------------------------

#ifclear _VERBLIB_H
#set _VERBLIB_H

!----------------------------------------------------------------------------
routine DoVague
{
        VMessage(&DoVague)       ! "Be a little more specific..."
        return false
}

!----------------------------------------------------------------------------
! FOR LOOKING AND EXAMINING:
!----------------------------------------------------------------------------
routine DoLookAround
{
        DescribePlace(location, true)
        return true
}

!----------------------------------------------------------------------------
routine DoLook
{
        local i

        if not light_source
                VMessage(&DoLook, 1)     ! "It's too dark to see anything."
        else
        {
                if not object.long_desc
                        ! "Looks just like you'd expect..."
                        VMessage(&DoLook, 2)

!               if object is living, transparent, not quiet
                if ((object is living, transparent) or
                        object is platform or
                        (object is container and (object is open or object is not openable))) and
                        object is not quiet and object is not already_listed
                {
                        for i in object
                        {
                                if i is not hidden
                                        break
                        }
                        if i and object ~= player
                        {
                                local tempformat
                                tempformat = FORMAT
                                FORMAT = FORMAT | NOINDENT_F
                                list_nest = 0
                                print ""
                                WhatsIn(object)
                                FORMAT = tempformat
                        }
                }

                run object.after
                return true
        }
}

!----------------------------------------------------------------------------
routine DoLookIn
{
        local tempformat

        if not CheckReach(object):  return false

        if not light_source
                VMessage(&DoLook, 1)     ! "It's too dark to see anything."
        else
        {
                if object is container and (object is openable and
                        object is not open and object is not transparent):
                        VMessage(&DoLookIn, 1)           ! "It's closed."
                else
                {
                        if not object.after
                        {
                                object is not quiet

                                tempformat = FORMAT
                                FORMAT = FORMAT | NOINDENT_F
                                list_nest = 0

                                if WhatsIn(object) = 0
                                        VMessage(&DoLookIn, 2)   ! "It's empty."

                                FORMAT = tempformat
                        }
                }
                return true
        }
}

!----------------------------------------------------------------------------
routine DoLookThrough
{
        if object is not static
                if not CheckReach(object)
                        return false

        if object is transparent
        {
                if object is container or children(object)
                        return Perform(&DoLookIn, object)
                else
                        return Perform(&DoLook, object)
        }

#ifclear NO_OBJLIB
        elseif object.type = door
        {
                if object is not open
                        VMessage(&DoLookIn, 1)   ! "It's closed."
                else
                {
                        ! "Through it you can see..."
                        VMessage(&DoLookThrough, 1)

                        if location = object.found_in #1
                                print The(object.found_in #2); "."
                        else
                                print The(object.found_in #1); "."
                }
                return true
        }
#endif
        else
                VMessage(&DoLookThrough, 2)  ! "You can't see through that."
}

!----------------------------------------------------------------------------
routine DoLookUnder
{
        if not CheckReach(object):  return false

        VMessage(&DoLookUnder, 1)        ! "You don't find anything."
        return true
}

!----------------------------------------------------------------------------
! FOR WAITING AROUND:
!----------------------------------------------------------------------------

global WAIT_TURN_COUNT = 3

routine DoWait(count)                   ! count argument is from DoWaitUntil
{
        if object = 0
                count = WAIT_TURN_COUNT
        elseif count = 0
                count = object

        VMessage(&DoWait)                ! "Time passes..."
        event_flag = 0
        while --count
        {
                Main
                system(32) !(PAUSE_100TH_SECOND)  ! potential updating
                if event_flag
                        if not KeepWaiting
                                return
        }
        event_flag = 0
        return true
}

!----------------------------------------------------------------------------
routine DoWaitforChar
{
        local count

        if object is not living
        {
                ParseError(6)
                return false
        }

        if object in location
        {
                VMessage(&DoWaitforChar, 1)      ! "They're right here..."
                return true
        }

        VMessage(&DoWait, 1)                     ! "Time passes..."
        event_flag = 0
        do
        {
                Main
                if object in location
                {
                        ! character has arrived
                        VMessage(&DoWaitforChar, 2)
                        event_flag = 0
                        return
                }
                if event_flag
                {
                        if not KeepWaiting
                                return
                }
                count++
        }
        while (STATUSTYPE=2 and count<60) or (STATUSTYPE~=2 and count<20)

        VMessage(&DoWaitforChar, 3)     ! char. hasn't arrived yet

        event_flag = 0
}

!----------------------------------------------------------------------------
routine DoWaitUntil
{
        if object = counter
        {
                VMessage(&DoWaitUntil, 1)       ! "Wow.  Time flies."
                return
        }
        elseif object < counter and (STATUSTYPE ~= 2 or object > 720)
        {
                VMessage(&DoWaitUntil, 2)       ! "You're ahead of your time."
                return
        }
        else
        {

                if object < counter
                {
                        if object < 60
                                ! Assume "2" is "2:00"
                                object = object * 60
                        else
                                ! Convert "1:00" to "1:00 p.m.", if it's
                                ! past 1:00 a.m.
                                object = object + 720
                }

                DoWait(object - counter + 1)

                ! "It is now (whatever time/turn)..."
                VMessage(&DoWaitUntil, 3)
        }
        event_flag = 0
}

!----------------------------------------------------------------------------
routine KeepWaiting                     ! used by DoWait... routines
{
        if event_flag = 2               ! i.e., force an interruption
        {
                event_flag = 0
                return false
        }

        event_flag = 0
        VMessage(&KeepWaiting)           ! "Keep waiting?"
        GetInput
        return YesorNo
}

!----------------------------------------------------------------------------
! FOR TALKING TO CHARACTERS:
!----------------------------------------------------------------------------
routine DoHello
{
        local obj

        if object
        {
                obj = object
                object = nothing
                SpeakTo(obj)
        }
        else
        {
                VMessage(&DoHello)
                return false
        }
}

routine DoTalk
{
        if word[2] = "to" and not xobject
        {
                if object is unfriendly
                {
                        if not object.ignore_response
                                Message(&Speakto, 4)    ! "X ignores you."
                        speaking = 0
                }
                elseif object = player
                {
                        Message(&Speakto, 1)    ! "Stop talking to yourself."
                        return false
                }
                else
                {
                        speaking = object
                        Message(&Speakto, 2)            ! "X is listening."
#ifclear NO_SCRIPTS
                        SkipScript(object)
#endif
                }
                return true
        }

        VMessage(&DoTalk, 1)     ! "Tell or ask about something specific..."
        return false
}

!----------------------------------------------------------------------------
routine DoAsk
{
        if xobject = 0
        {
                VMessage(&DoAsk, 1)      ! "Ask about something specific..."
                return false
        }

        speaking = object

        if object is unfriendly
        {
                if not object.ignore_response
                        Message(&Speakto, 4)    ! "X ignores you."
                speaking = 0
        }
        elseif object = player
        {
                VMessage(&DoAsk, 2)             ! "Talking to yourself..."
                return false
        }
        elseif xobject = player
        {
                if not object.after
                        VMessage(&DoAsk, 3)     ! asking about yourself
        }
        elseif xobject = object
        {
                if not object.after
                        VMessage(&DoAsk, 4)     ! asking about him/herself
        }
        else
        {
!\ "Suspicious character" behavior commented out:
                if xobject in location and xobject is living

                        ! "Not while the other person is in the room..."
                        VMessage(&DoAsk, 5)

                elseif not object.after
\!
                if not object.after
                {
                        if not xobject.after

                                ! "Doesn't seem to know anything..."
                                VMessage(&DoAsk, 6)
                }

#ifclear NO_SCRIPTS
                SkipScript(object)
#endif
        }
        return true
}

!----------------------------------------------------------------------------
routine DoAskQuestion
{
        if speaking = 0
        {
                VMessage(&DoAskQuestion)        ! "Not talking to anybody..."
                return false
        }
        elseif xobject
        {
                ParseError(6)
                return false
        }

        return Perform(&DoAsk, speaking, object)
}

!----------------------------------------------------------------------------
routine DoTell
{
        if object = player
                VMessage(&DoAsk, 2)      ! "Talking to yourself..."

        if xobject = 0
        {
                VMessage(&DoTell, 1)    ! "Tell about something specific..."
                return false
        }

        speaking = object


        if object is unfriendly
        {
                if not object.ignore_response
                        Message(&Speakto, 4)    ! "X ignores you."
                speaking = 0
        }
        else
        {
                if not object.after
                {
                        if not xobject.after
                                VMessage(&DoTell, 2)    ! "Not interested..."
                }
#ifclear NO_SCRIPTS
                SkipScript(object)
#endif
        }
        return true
}

!----------------------------------------------------------------------------
routine DoListen
{
        if not object
        {
                VMessage(&DoListen, 1)  ! "Be a little more specific..."
                return false
        }
        elseif not object.after
                VMessage(&DoListen, 2)   ! "Not making any sound..."
        return true
}

!----------------------------------------------------------------------------
! FOR MOVING AROUND
!----------------------------------------------------------------------------

! (Credit for the implementation of generic directions goes to Julian Arnold)

routine DoGo
{
        local moveto
#ifset NO_OBJLIB
        local wordnum, m
#endif

        if player not in location               ! sitting on or in an obj.
        {
#ifclear NO_OBJLIB
                if object ~= u_obj and object ~= out_obj
                {
                        VMessage(&DoGo, 3)      ! "You'll have to get up..."
                        return false
                }
#endif
                if parent(player).before        ! i.e., a vehicle, etc.
                        return true
        }
        elseif obstacle
        {
#ifclear NO_OBJLIB
                VMessage(&DoGo, 1)              ! "X stops you from going..."
                return true
#endif
        }

#ifclear NO_OBJLIB
        ! See if the object is one of the current direction set

        if object.type = direction and object in direction
        {
                moveto = object
                jump FoundDirection
        }
#endif

#ifclear NO_OBJLIB
        if not moveto
        {
#else
        wordnum = 1                             ! various phrasings
        if words > 1 and word[2] ~= ""
                wordnum = 2
        if word[2] = "to", "through"
                wordnum = 3
        elseif (word[2] = "in" or word[2] = "inside") and words > 2
                wordnum = 3

        select word[wordnum]
                case "north", "n":      m = n_to
                case "south", "s":      m = s_to
                case "east", "e":       m = e_to
                case "west", "w":       m = w_to
                case "northwest", "nw": m = nw_to
                case "northeast", "ne": m = ne_to
                case "southeast", "se": m = se_to
                case "southwest", "sw": m = sw_to
                case "up", "u":         m = u_to
                case "down", "d":       m = d_to
                case "in", "inside":    m = in_to
                case "out", "outside":  m = out_to

        if not m
        {
#endif
                if not object
                {
                        ! ParseError(6)  ! doesn't make any sense
                        VMessage(&DoGo, 4)      ! "Which way...?"
                        return
                }

                if not &object.door_to
                {
                        if not object.door_to and object is enterable
                                return Perform(&DoEnter, object)
                }

                moveto = object.door_to
                if not moveto
                {
                        VMessage(&DoEnter, 2)  ! "You can't enter..."
                        return
                }
                if moveto = 1
                        return true
        }

:FoundDirection

#ifclear NO_OBJLIB
        if moveto.type = direction and moveto in direction
        {
                if player not in location and object = out_obj
                {
                        if parent(player) is platform
                        {
                                VMessage(&DoGo, 3)
                                return false
                        }
                        else
                                return Perform(&DoExit)
                }
                elseif player not in location and object = u_obj
                {
                        if parent(player) is container
                        {
                                VMessage(&DoGo, 3)
                                return false
                        }
                        else
                                return Perform(&DoExit, parent(player))

                }

                if not object
                {
                        object = moveto
                        if object.before
                                return true
                }
!               else:  object = moveto
                moveto = location.(moveto.dir_to)

                if &moveto.door_to or moveto.type = door
                {
                        moveto = moveto.door_to
                }
        }
#else
        if m
        {
                if player not in location and m = out_to
                {
                        if parent(player) is platform
                        {
                                VMessage(&DoGo, 3)
                                return false
                        }
                        else
                                return Perform(&DoExit)
                }
                elseif player not in location and m = u_to
                {
                        if parent(player) is container
                        {
                                VMessage(&DoGo, 3)
                                return false
                        }
                        else
                                return Perform(&DoExit, parent(player))
                }

                moveto = location.m
                if moveto.door_to
                        moveto = moveto.door_to
        }
#endif

        if moveto = false
        {
                if not location.cant_go
                        VMessage(&DoGo, 2)      ! "You can't go that way."
                return false
        }

        elseif moveto = true                    ! already printed message
                return true                     ! (moveto is never 1)

        elseif player not in location           ! sitting on or in an obj.
        {
                VMessage(&DoGo, 3)              ! "You'll have to get up..."
        }
        else
        {
!\ Recursive call issues
                m = verbroutine                 ! check room exiting
                verbroutine = &DoExit
                if location.before
                        return true
                verbroutine = m
\!
                MovePlayer(moveto)

                return true
        }
}

!----------------------------------------------------------------------------
! Note that DoEnter and DoExit expect the preposition (alias prep) property
! to hold the equivalent of "in" and "out" for the enterable object in
! question; if there is no preposition property, "in" and "out" are the
! defaults.

routine DoEnter
{
#ifclear NO_OBJLIB
        if not object or object in direction
#else
        if not object
#endif
        {
                local i, obj, count
                for i in location
                {
                        if i is enterable
                        {
                                obj = i
                                count++
                        }
                }
                if count = 1
                {
                        object = obj
                }
                else
                {
                        VMessage(&DoEnter, 1)    ! "Be a little more specific..."
                        return false
                }

                return Perform(&DoEnter, object)
        }

        ! To prevent endless loops if the player_character class
        ! automatically resets the object to in_obj if word[1] = "in"
        word[1] = ""

        if &object.door_to
                return Perform(&DoGo, object)   ! routine
        elseif object.door_to
                return Perform(&DoGo, object)   ! object

        if object is not enterable or Contains(player, object)
                VMessage(&DoEnter, 2)    ! "You can't enter that."
        elseif player in object
                VMessage(&DoEnter, 3)    ! already in it
        elseif player not in location
                VMessage(&DoGo, 3)       ! "You'll have to get up..."
        elseif object is openable, not open
                VMessage(&DoLookIn, 1)   ! "X is closed."
        else
        {
                move player to object
                if not object.after
                        VMessage(&DoEnter, 4)    ! "You get in..."
                object is not quiet
                DescribePlace(location)
                return true
        }
        return
}

routine DoSit
{
#ifclear NO_OBJLIB
        if not object or object is enterable or (object = d_obj, u_obj)
#else
        if not object or object is enterable
#endif
                return Perform(&DoEnter, object, xobject)
        else
                ParseError(12, object)  ! "Can't do that with..."
}

!----------------------------------------------------------------------------
routine DoExit
{
        local p

#ifclear NO_OBJLIB
        ! >GO OUT winds up calling DoExit with object = out_obj, thanks to
        ! the direction-parsing code in Perform().  English ambiguities being
        ! what they are, we correct that interpretation of "out" here, and
        ! treat the command as a generic instruction to exit whatever
        ! container context we may be in.
        if object = out_obj
                object = nothing

        if object = nothing or object = location
        {
                if player in location and out_obj in direction
                {
                        word[1] = out_obj.noun
                        word[2] = ""
                        return Perform(&DoGo)
                }
        }
        elseif object = d_obj and player in location
        {
                return Perform(&DoGo, object)
        }
#endif

        p = parent(player)

#ifclear NO_OBJLIB
        if object and player not in object
#else
        if object and player not in object
#endif
                VMessage(&DoExit, 1)             ! "You aren't in that."
        elseif p is openable, not open
                VMessage(&DoLookIn, 1)           ! "X is closed."
        else
        {
                if object = nothing
                        object = p
                move player to location
                if not object.after
                        VMessage(&DoExit, 2)     ! "You get out.."
        }
        return true
}

routine DoStand
{
        if parent(parent(player)) = nothing     ! i.e., a room
                VMessage(&DoEnter, 3)           ! "A bit redundant..."
        else
                return Perform(&DoExit(parent(player)))
}

!----------------------------------------------------------------------------
! FOR MANIPULATING OBJECTS:
!----------------------------------------------------------------------------
routine DoInventory
{
        local tempformat

        if word[2] = "tall"
                INVENTORY_MASK = INVENTORY_MASK | LIST_F
        elseif word[2] = "wide"
                INVENTORY_MASK = INVENTORY_MASK & ~LIST_F

        tempformat = FORMAT
        FORMAT = FORMAT | INVENTORY_F | GROUPPLURALS_F | NOINDENT_F
        FORMAT = FORMAT | INVENTORY_MASK
        list_nest = 0
        if not WhatsIn(player)
                VMessage(&DoInventory)   ! "You aren't carrying anything."
        FORMAT = tempformat

        return true
}

!----------------------------------------------------------------------------
routine DoClose
{
        if not CheckReach(object):  return false

        if object is not openable
        {
                VMessage(&DoClose, 1)            ! "You can't close that."
                return
        }
        elseif object is not open
                VMessage(&DoClose, 2)            ! "It's already closed."
        else
        {
                object is not open
                object is moved
                if not object.after
                        VMessage(&DoClose, 3)    ! "Closed."

                if not FindLight(location)      ! in case the light source
                                                ! has been concealed

                        VMessage(&DoClose, 4)    ! "Everything goes dark."
        }
        return true
}

!----------------------------------------------------------------------------
routine DoOpen
{
        local tempformat

        if not CheckReach(object):  return false

        if object is not openable
        {
                VMessage(&DoOpen, 1)             ! "You can't open that."
                return
        }
        elseif object is open
                VMessage(&DoOpen, 2)             ! "It's already open."
        elseif object is locked
                VMessage(&DoOpen, 3)             ! "It's locked."
        else
        {
                object is open
                object is moved
                if not object.after
                {
                        VMessage(&DoOpen, 4)     ! "Opened."

                        FindLight(location)     ! in case the light source
                                                ! has been revealed
                        if children(object) and object is not quiet
                        {
                                print ""
                                tempformat = FORMAT
                                FORMAT = FORMAT | NOINDENT_F
                                list_nest = 0
                                WhatsIn(object)
                                FORMAT = tempformat
                        }
                }
        }
        return true
}

!----------------------------------------------------------------------------
routine DoMove
{
        if not CheckReach(object):  return false

        VMessage(&DoMove)                ! "You can't move that."
        return true
}

!----------------------------------------------------------------------------
routine DoGet
{
        local b, p

        if object in player
                VMessage(&DoGet, 1)      ! "You already have that."
        elseif object = player
        {
                VMessage(&DoGet, 2)     ! player trying to get player
                return false
        }
        elseif object is living and object is static
        {
                VMessage(&DoGet, 3)     ! player trying to get character
                return false
        }
        elseif parent(object) is living and parent(object) is unfriendly
                VMessage(&DoGet, 4)      ! "X doesn't want to give it to you."
        elseif (parent(object) is openable, not open) and
                parent(object) is container:
        {
                VMessage(&DoGet, 5)     ! "X is closed."
                return false
        }
        elseif Contains(object, player)
        {
                if object is static
                        VMessage(&DoGet, 7)     ! "You can't take that."
                else
                        VMessage(&DoGet, 6)     ! "Not while you're in/on it..."
                return false
        }
        else
        {
                if not CheckReach(object)
                        return false
                elseif object is static
                {
                        VMessage(&DoGet, 7)      ! "You can't take that."
                        return true
                }

                ! Because the engine calls location.before
                if (parent(object)~=location)
                        b = parent(object).before

                if not b
                {
                        xobject = parent(object)

                        if object not in location
                        {
                                CalculateHolding(xobject)
                                p = xobject
                        }

                        if Acquire(player, object)
                        {
                                object is not hidden

                                if not object.after
                                {
                                        ! Again, to avoid duplication or
                                        ! echoing:
                                        !
                                        b = 0
                                        if xobject ~= location
                                                b = xobject.after

                                        if b = false
                                                ! "Taken."
                                                VMessage(&DoGet, 8)
                                }
                        }
                        else
                                ! "You're carrying too much to take that."
                                VMessage(&DoGet, 9)
                }
        }
        return true
}

routine ImplicitTake(obj, no_newline)
{
        ! It's generally okay to call ImplicitTake with no arguments
        ! from a verbroutine

        local r

        if obj ~= 0
                object = obj

        if not FindObject(obj, location)
        {
                ParseError(11, obj)     ! "You don't see that."
                return false
        }

        VMessage(&ImplicitTake)         ! "(taking it first)"
        r = Perform(&DoGet, object)
        if object not in player
        {
                if r
                {
                        Main            ! counts as a turn
                }
                return false
        }
        Main                            ! counts as a turn
        if not no_newline
                print ""
        return true
}


#ifset USE_CHECKHELD

!----------------------------------------------------------------------------
! Note on ImplicitTakeForDrop:
!
! If USE_CHECKHELD is set, the Do*_CheckHeld routines are called by the
! library grammar to enable calling DoDrop, etc. without a 'held' token in
! the grammar so that something like this will work:
!
!      >PUT BOOK ON SHELF
!      (taking the book first)
!      Taken.
!
!      You put the book on the shelf.
!
! Before/after routines should check DoDrop, etc. as opposed
! to DoDrop_CheckHeld.  For compatibility and convenience, grammar compiled
! using 'held' tokens and standard DoDrop, etc. will work as usual.
!
! ParseCheckHeld temporarily moves held objects into nonheld scope (for "all"
! handling) and flags them with checkheld_flag.
!
! NOTE:  There are still some functionality issues with USE_CHECKHELD;
! enabling it is not recommended.

routine ImplicitTakeForDrop(obj, loc)
{
        if obj is checkheld_flag        ! already moved and marked
                return true

        if FindObject(obj, loc) and obj ~= loc and (obj is not static, living)
        {
                if object in xobject
                {
                        VMessage(&DoPutIn, 8)   ! "It's already in..."
                        return false
                }

                local r
                r = ImplicitTake(obj)

                ! ImplicitTake may reset this (via Perform)
                checkheld is workflag

                return r
        }
        ParseError(15, obj)             ! "You're not carrying that."
}

constant CHECKHELD_LIMIT 64     ! # objects that can be managed at a time

object checkheld        ! 'active' when active; 'plural' for "~all", etc.
{
        misc #CHECKHELD_LIMIT
        size 0          ! # of managed objects being managed
        is hidden
}
attribute checkheld_flag

routine ParseCheckHeld
{
        local i, obj, nextobj

        ResetCheckHeld

        ! checkheld is workflag when an ImplicitTakeForDrop has
        ! been executed during a series of Perform calls.  Don't
        ! include this in ResetCheckHeld since Perform calls
        ! ResetCheckHeld.
        checkheld is not workflag

        ! If the verb requires an explicitly held object
        if VerbHeldMode(word[1]) = 1
        {
                for (i=2; i<=words; i++)
                {
                        if word[i] = "~all"
                        {
                                checkheld.size = 0
                                obj = child(player)
                                while obj
                                {
                                        nextobj = sibling(obj)
                                        if obj is not static, hidden
                                        {
                                                move obj to location
                                                obj is checkheld_flag
                                                checkheld.size++
                                                checkheld.misc #(checkheld.size) = obj
                                        }
                                        obj = nextobj
                                }
                                move checkheld to location
                                checkheld is plural
                                break
                        }
                        elseif word[1] = ""
                                break
                }

                if checkheld is not plural
                {
                        for obj in player
                        {
                                obj is checkheld_flag
                                checkheld.size++
                                checkheld.misc #(checkheld.size) = obj
                        }
                }

                checkheld is active
        }
}

routine CallVerbCheckHeld(v, loc)
{
        local r, impt

        if object not in player and object is not checkheld_flag
        {
                if not ImplicitTakeForDrop(object, loc)
                        return false
                impt = true
        }

        RemoveCheckHeldObj(object)

        r = Perform(v, object, xobject)

        ! Perform will reset this:
        if impt:  checkheld is workflag

        return r
}

routine ResetCheckHeld
{
        local i, obj
        for (i=1; i<=checkheld.size; i++)
        {
                obj = checkheld.misc #i
                if obj is checkheld_flag and obj in parent(checkheld)
                        move obj to player
                obj is not checkheld_flag
        }
        checkheld.size = 0
        checkheld is not plural
        checkheld is not active
}

routine RemoveCheckHeldObj(obj)
{
        if obj is checkheld_flag
        {
                obj is not checkheld_flag
                move obj to player
        }
}

#endif  ! USE_CHECKHELD


!----------------------------------------------------------------------------
routine DoDrop
{
        if object is clothing and object is worn
                VMessage(&DoDrop, 1)     ! "You'll have to take it off first."

        ! For the following, xobject is true only
        ! when called by DoPutonGround:
        elseif player not in location and (parent(player) is container or
                parent(player) is platform) and not xobject
        {
                Perform(&DoPutIn, object, parent(player))
        }
        else
        {
                move object to location
                object is moved
                player.holding = player.holding - object.size
                if not object.after
                        VMessage(&DoDrop, 2)     ! "Dropped."
        }
        xobject = 0
        return true
}

#ifset USE_CHECKHELD
routine DoDrop_CheckHeld                ! See above re: ImplicitTakeForDrop
{
        return CallVerbCheckHeld(&DoDrop, player)
}
#endif

routine DoPutonGround                   ! to override vehicles, etc.
{
        if xobject and player not in xobject
        {
                VMessage(&DoPutonGround, 1)      ! "You aren't in X."
                return false
        }
        xobject = -1
        return Perform(&DoDrop, object, xobject)
}

#ifset USE_CHECKHELD
routine DoPutonGround_CheckHeld         ! See above re: ImplicitTakeForDrop
{
        return CallVerbCheckHeld(&DoPutonGround, location)
}
#endif

!----------------------------------------------------------------------------
! >LEAVE OBJECT goes to DoDropOrExit, which dispatches to DoDrop or DoExit

routine DoDropOrExit
{
        if object not in player
                return Perform(&DoExit, object)
        else
                return Perform(&DoDrop, object)
}

!----------------------------------------------------------------------------
routine DoPutIn
{
        if not xobject
                VMessage(&DoPutIn, 1)    ! "Put it in what?"
        elseif object is clothing and object is worn
                VMessage(&DoDrop, 1)     ! "Have to take it off first..."
        elseif xobject is container, openable, not open
                VMessage(&DoPutIn, 2)    ! "It's closed."
        elseif object = xobject
                VMessage(&DoPutIn, 3)    ! putting something in itself
        elseif xobject is not container and xobject is not platform
                VMessage(&DoPutIn, 7)    ! "Can't do that with..."
        elseif Contains(object, xobject)
                VMessage(&DoPutIn, 4)    ! putting a parent in its child
        elseif object in xobject
                VMessage(&DoPutIn, 8)   ! "It's already in..."
        elseif CheckReach(object)
        {
                if CheckReach(xobject)
                {
                        if Acquire(xobject,object)
                        {
                                if not object.after
                                {
                                        if not xobject.after
                                                ! "You put X in/on Y."
                                                VMessage(&DoPutIn, 5)
                                }
                        }
                        else
                                ! "There's no room..."
                                VMessage(&DoPutIn, 6)
                }
        }
        return true
}

#ifset USE_CHECKHELD
routine DoPutIn_CheckHeld               ! See above re: ImplicitTakeForDrop
{
        return CallVerbCheckHeld(&DoPutIn, location)
}
#endif

!----------------------------------------------------------------------------
routine DoEmpty
{
        local a, b, obj, xobj
        local thisobj, nextobj

        CalculateHolding(object)

        if object is container, openable, not open
        {
                VMessage(&DoEmpty, 1)           ! "It's closed."
                return true
        }
        if not children(object)
        {
                VMessage(&DoEmpty, 2)           ! "It's already empty."
                return true
        }

        if object is not container, platform
        {
                ParseError(12, object)
                return false
        }

        thisobj = child(object)
        while thisobj
        {
                nextobj = sibling(thisobj)

                print thisobj.name; ":  ";

                if thisobj is static
                        VMessage(&DoEmpty, 3)    ! "You can't move that."
                else
                {
                        a = player.holding
                        b = thisobj
                        obj = object
                        xobj = xobject

                        if player not in location and
                                (parent(player) is platform or
                                        parent(player) is container) and
                                not xobject:

                                Perform(&DoPutIn, b, parent(player))
                        else
                                Perform(&DoDrop, b)

                        object = obj
                        xobject = xobj
                        player.holding = a
                        if b not in object
                                object.holding = object.holding - b.size
                }

                thisobj = nextobj
        }

        run object.after
        return true
}

routine DoEmptyGround                   ! to override vehicles, etc.
{
        xobject = 1
        return Perform(&DoEmpty, object, xobject)
}

!----------------------------------------------------------------------------
! >UNLOAD OBJECT goes to DoEmptyOrGet, which dispatches to DoEmpty or DoGet

routine DoEmptyOrGet
{
        if object is container or object is platform or
                object in player or player in object
        {
                return Perform(&DoEmpty, object)
        }
        else
                return Perform(&DoGet, object)
}

!----------------------------------------------------------------------------
routine DoGive
{
        if not xobject
        {
                VMessage(&DoGive, 1)    ! "Be more specific..."
                return false
        }

        if object not in player
        {
                if not FindObject(object, location)
                {
                        ParseError(11, object)
                        return false
                }
                else
                {
                        if not ImplicitTake(object)
                                return false
                }
        }

        if object is clothing, worn
        {
                VMessage(&DoDrop, 1)    ! "Have to take it off first..."
                return false
        }

        if xobject = player
        {
                VMessage(&DoGive, 2)    ! player giving something to player
                return false
        }

        if not xobject.after
        {
                if not object.after
                {
                        if xobject is not living
                                ! "Your kind gesture goes unnoticed."
                                VMessage(&DoGive, 3)
                        else
                                ! "X refuses."
                                VMessage(&DoGive, 4)
                }
        }
        return true
}

!----------------------------------------------------------------------------
routine DoShow
{
        if not xobject
        {
                VMessage(&DoShow, 1)    ! "Be more specific..."
                return false

        }

        if not xobject.after
        {
                if not object.after
                {
                        if xobject is not living
                                ! showing it to something inanimate
                                VMessage(&DoShow, 2)
                        else
                                ! character ignores it
                                VMessage(&DoShow, 3)
                }
        }
        return true
}

!----------------------------------------------------------------------------
routine DoWear
{
        if object is not clothing
        {
                VMessage(&DoWear, 1)    ! "You can't wear that."
                return false
        }

        if object not in player
        {
                if not FindObject(object, location)
                        ParseError(11, object)
                if not ImplicitTake(object)
                        return false
        }

        if object is worn
                VMessage(&DoWear, 2)     ! "Already wearing that..."
        else
        {
                object is worn
                if not object.after
                        VMessage(&DoWear, 3)     ! "You put it on."
        }
        return true
}

!----------------------------------------------------------------------------
routine DoTakeOff
{
        if not Contains(player, object)
        {
                ! So >REMOVE LOCK will >TAKE LOCK, etc.
                return Perform(&DoGet, object)
        }

        if object is not clothing
        {
                VMessage(&DoTakeOff, 1)         ! "Can't do that..."
                return false
        }

        if object is not worn
                VMessage(&DoTakeOff, 2)          ! "You're not wearing that."
        else
        {
                object is not worn
                if not object.after
                        VMessage(&DoTakeOff, 3)  ! "You take it off."
        }
        return true
}

!----------------------------------------------------------------------------
routine DoSwitchOnOff
{
        if object is switchedon
                return Perform(&DoSwitchOff, object)
        else
                return Perform(&DoSwitchOn, object)
}

!----------------------------------------------------------------------------
routine DoSwitchOn
{
        if not CheckReach(object):  return false

        if object is switchedon
                VMessage(&DoSwitchOn, 1)         ! "It's already on."
        else
        {
                object is switchedon
                if not object.after
                        VMessage(&DoSwitchOn, 2)         ! "Switched on."
        }
        return true
}

!----------------------------------------------------------------------------
routine DoSwitchOff
{
        if not CheckReach(object):  return false

        if object is not switchedon
                VMessage(&DoSwitchOff, 1)        ! "It's already off."
        else
        {
                object is not switchedon
                if not object.after
                        VMessage(&DoSwitchOff, 2)        ! "Switched off."
        }
        return true
}

!----------------------------------------------------------------------------
routine DoUnlock
{
        if not CheckReach(object):  return false

        if xobject ~= 0
        {
                if object.key_object ~= xobject
                {
                        VMessage(&DoUnlock, 1)  ! "Doesn't seem to work..."
                        return true
                }
        }
        elseif object.key_object and object.key_object not in player
        {
                VMessage(&DoUnlock, 2)           ! no key that fits
                return true
        }

        if object is not locked
                VMessage(&DoUnlock, 3)           ! already unlocked
        else
        {
                object is not locked
                if not object.after
                {
                        if not xobject.after
                                VMessage(&DoUnlock, 4)   ! "Unlocked."
                }
        }
        return true
}

!----------------------------------------------------------------------------
routine DoLock
{
        if not CheckReach(object):  return false

        if xobject ~= 0
        {
                if object.key_object ~= xobject
                {
                        VMessage(&DoUnlock, 1)  ! "Doesn't seem to work..."
                        return true
                }
        }
        elseif object.key_object and object.key_object not in player
        {
                VMessage(&DoUnlock, 2)           ! no key that fits
                return true
        }

        if object is locked
                VMessage(&DoLock, 1)             ! already locked
        elseif object is open
                VMessage(&DoLock, 2)             ! "Have to close it first..."
        else
        {
                object is locked
                if not object.after
                {
                        if not xobject.after
                                VMessage(&DoLock, 3)     ! "Locked."
                }
        }
        return true
}

!----------------------------------------------------------------------------
routine DoEat
{
        if not CheckReach(object):  return false

        if not object.after
                VMessage(&DoEat)         ! "You can't eat that."
        else
                return true
}

!----------------------------------------------------------------------------
routine DoDrink
{
        if not CheckReach(object):  return false

        if not object.after
                VMessage(&DoDrink)       ! "You can't drink that."
        else
                return true
}

!----------------------------------------------------------------------------
routine DoHit
{
        if not CheckReach(object):  return false

        if not object.after
                VMessage(&DoHit)         ! "Doesn't accomplish much..."
        else
                return true
}

#endif  ! ifclear NO_VERBS

!----------------------------------------------------------------------------
! NON-ACTION VERBS:
!----------------------------------------------------------------------------

! Note that AnyVerb() in hugolib.h depends on certain ordering of xverb
! definitions in verblib.h

#ifclear NO_XVERBS

routine DoBrief
{
        VMessage(&DoBrief)
        verbosity = 0
}

routine DoSuperbrief
{
        VMessage(&DoSuperBrief)
        verbosity = 1
}

routine DoVerbose
{
        VMessage(&DoVerbose)
        verbosity = 2
}

routine DoDisplay
{
        if VerbWord = "tall" or word[2] = "tall"
        {
                VMessage(&DoDisplay, 1)          ! "Tall listings."
                FORMAT = FORMAT | LIST_F
        }
        elseif VerbWord = "wide" or word[2] = "wide"
        {
                VMessage(&DoDisplay, 2)          ! "Wide listings."
                FORMAT = FORMAT & ~LIST_F
        }
        else
                VMessage(&DoDisplay, 3)          ! instructions
}

!----------------------------------------------------------------------------
routine DoSave
{
        if save
        {
                VMessage(&DoSave, 1)             ! "Saved."
                return true
        }
        else:  VMessage(&DoSave, 2)              ! "Unable to save."
}

!----------------------------------------------------------------------------
routine DoRestore
{
        if restore
        {
                VMessage(&DoRestore, 1)         ! "Restored."
                PrintStatusline
                DescribePlace(location, true)
                return true
        }
        else:  VMessage(&DoRestore, 2)           ! "Unable to restore."
}

!----------------------------------------------------------------------------
routine DoQuit
{
        PrintScore
        VMessage(&DoQuit)                        ! "Are you sure?"
        GetInput
        if YesorNo = true
                quit
}

!----------------------------------------------------------------------------
routine DoRestart
{
        VMessage(&DoRestart, 1)                  ! "Are you sure?"
        GetInput
        if YesorNo = true
        {
                if not restart
                        VMessage(&DoRestart, 2)  ! "Unable to restart."
        }
}

!----------------------------------------------------------------------------
routine DoScriptOnOff
{
        if word[2] = "on" or words = 1
        {
                if not scripton
                        VMessage(&DoScriptOnOff, 1)      ! "Unable to begin..."
                else
                        VMessage(&DoScriptOnOff, 2)      ! "Transcription on."
        }
        elseif word[2] = "off"
        {
                if not scriptoff
                        VMessage(&DoScriptOnOff, 3)      ! "Unable to end..."
                else
                        VMessage(&DoScriptOnOff, 4)      ! "Transcription off."
        }
}

!----------------------------------------------------------------------------
#ifclear NO_RECORDING
routine DoRecordOnOff
{
        if VerbWord = "playback"
        {
                if not playback
                        VMessage(&DoRecordOnOff, 1)  ! "Unable to begin..."
                else
                        VMessage(&DoRecordOnOff, 2)  ! "Playback beginning..."
        }
        elseif word[2] = "on" or words = 1
        {
                if not recordon
                        VMessage(&DoRecordOnOff, 3)  ! "Unable to begin..."
                else
                        VMessage(&DoRecordOnOff, 4)  ! "Recording on."
        }
        elseif word[2] = "off"
        {
                if not recordoff
                        VMessage(&DoRecordOnOff, 5)  ! "Unable to end..."
                else
                        VMessage(&DoRecordOnOff, 6)  ! "Recording off."
        }
}
#endif

!----------------------------------------------------------------------------
routine DoScore
{
        if STATUSTYPE ~= 1
                VMessage(&DoScore)               ! no scorekeeping
        else
                PrintScore
}

!----------------------------------------------------------------------------
routine DoUndo
{
        if not UNDO_OFF
        {
                if undo
                {
                        PrintStatusline
                        DescribePlace(location)
                }
                else
                        VMessage(&DoUndo)
        }
        else
                VMessage(&DoUndo)
}

#endif  ! ifclear NO_XVERBS


!----------------------------------------------------------------------------
! VERB LIBRARY MESSAGES
!
! VMessage and NewVMessages mirror HUGOLIB.H's Message and NewMessages,
! respectively.
!
! VMessage(&routine, number)
! prints message <number> for <routine>
!
! VMessage (&routine, number, var1, var2)
! where one or two variables--which may be objects or any other value--
! are used by message <number> for <routine>

routine VMessage(r, num, a, b)
{
        ! Check first to see if the NewVMessages routine provides a
        ! replacement message:
        if NewVMessages(r, num, a, b):  return

        select r

        case &DoVague:
        {
                print "Be a little more specific about what you'd like ";
                if player_person ~= 2
                        print The(player, true); " ";
                print "to "; VerbWord; "."
        }

        case &DoLook
        {
                select num
                case 1:  print "It's too dark to see anything."
                case 2
                {
                        print CThe(object);
#ifclear NO_OBJLIB
                        if object = u_obj, d_obj
                                print " "; The(player);
#endif
                        print " look"; MatchSubject(object); \
                                " just like "; The(player); " would expect."
                }
        }

        case &DoLookIn
        {
                select num
                case 1:  print CThe(object); IsorAre(object, true); " closed."
                case 2:  print CThe(object); IsorAre(object, true); " empty."
        }

        case &DoLookThrough
        {
                select num
                case 1:  print "Through "; Art(object); " "; The(player); \
                        " can see ";
                case 2:  print CThe(player); " can't see through that."
        }

        case &DoLookUnder:  print CThe(player); \
                                MatchPlural(player, "doesn't", "don't"); \
                                " find anything "; word[2]; " "; \
                                The(object); "."

        case &DoWait:  "Time passes..."

        case &DoWaitforChar
        {
                select num
                case 1:
                {
                        print CThe(object); IsorAre(object, true); \
                                " right here.";
                        if player_person = 2:  print "  Good detective work."
                        print newline
                }
                case 2
                {
                        print "\n("; CThe(object); \
                                MatchPlural(object, "has", "have"); \
                                " arrived.";
                        if STATUSTYPE = 2
                                print "  It is now "; HoursMinutes(counter);
                        print ")"
                }
                case 3
                {
                        print "\n("; CThe(player); \
                                MatchPlural(player, "has", "have"); \
                                " been waiting ";
                        select STATUSTYPE
                                case 2
                                        print "an hour";
                                case else
                                        print "for twenty turns";
                        print ", and "; The(object); " still"; \
                                MatchPlural(object, "hasn't", "haven't");
                                " arrived.)"
                }
        }

        case &DoWaitUntil
        {
                select num
                case 1:  print "Wow.  Time flies."
                case 2:
                {
                        if player_person = 2
                                print CThe(player); IsorAre(player); \
                                        " ahead of your time."
                        else
                                print "You're getting ahead of "; \
                                        The(player, true); "."
                }
                case 3
                {
                        print ""
                        select STATUSTYPE
                        case 2: print "It is now "; HoursMinutes(counter)
                        case else: print "It is now turn "; number object; "."
                }
        }

        case &KeepWaiting:
        {
                print "\nDo you want ";
                if player_person ~= 2:  print The(player, true); " ";
                print "to keep waiting (YES or NO)? ";
        }

        case &DoTalk:
        {
                print "Try ";
                if player_person ~= 2
                        print "having "; The(player, true); " ask or tell";
                else:  print "asking or telling";
                print " someone about something in particular."
        }

        case &DoAsk
        {
                select num
                case 1:
                {
                        print "Try ";
                        if player_person ~= 2
                                print "having "; The(player, true); " ask";
                        else:  print "asking";
                        print " someone about something in particular."
                }
                case 2:  print "Talking to "; player.pronoun #4; " isn't a
                        big step forward."
                case 3:  print "Hopefully "; The(player); " know"; \
                        MatchSubject(player); " as much as anyone."
                case 4
                {
                        print CThe(object); " would probably rather
                                not talk about "; object.pronoun #4; "."
                }
                case 5
                {
                        print CThe(object); " glance"; MatchSubject(object); \
                                " toward "; The(xobject); ".  "; \
                                capital object.pronoun; " would probably
                                rather not say anything with "; \
                                The(xobject); " right here."
                }
                case 6
                {
                        print CThe(object); \
                                MatchPlural(object, "doesn't", "don't"); \
                                " seem to know anything about ";
                        if xobject is living
                                The(xobject)
                        else
                                print "that";
                        print "."
                }
        }

        case &DoAskQuestion
        {
                if player_person = 1
                        print CThe(player); "'m not";
                else
                        print CThe(player); MatchPlural(player, "isn't", "aren't");
                " talking to anybody."
        }

        case &DoTell
        {
                select num
                case 1:
                {
                        print "Try ";
                        if player_person ~= 2
                                print "having "; The(player, true); " tell";
                        else:  print "telling";
                        print " someone about something in particular."
                }
                case 2
                {
                        print CThe(object); \
                                MatchPlural(object, "doesn't", "don't"); " seem
                                particularly interested in what "; The(player); \
                                MatchPlural(player, "has", "have"); " to say about ";
                        if xobject is living
                                The(xobject)
                        else
                                print "that";
                        print "."
                }
        }

        case &DoListen
        {
                select num
                case 1:
                {
                        print "Be a little more specific about exactly what
                                you'd like ";
                        if player_person ~= 2:  print The(player, true); " ";
                        print "to listen to."
                }
                case 2:  print CThe(object); IsorAre(object, true); " not
                                making a sound."
        }

        case &DoGo
        {
                select num
                case 1:  print CThe(obstacle); " stop"; \
                        MatchSubject(obstacle); " you from going anywhere."
                case 2:  print CThe(player); " can't go that way."
                case 3
                {
                        print CThe(player); " will have to get ";
                        if parent(player).prep #2
                                print parent(player).prep #2; " ";
                        elseif parent(player) is platform
                                print "off ";
                        else
                                print "out ";
                        print "of "; The(parent(player)); " first."
                }
                case 4
                {
                        print "Which way do you want ";
                        if player_person ~= 2:  print The(player, true); " ";
                        "to go?"
                }
        }

        case &DoEnter
        {
                select num
                case 1:
                {
                        print "Be a little more specific about where you'd
                                like ";
                        if player_person ~= 2:  print The(player, true); " ";
                        print "to "; VerbWord; "."
                }
                case 2:
                {
                        print CThe(player); " can't ";
                        if object is platform
                                print "get on ";
                        else
                                print "enter ";
                        print The(object); "."
                }
                case 3:  print "A bit redundant, don't you think?"
                case 4
                {
                        print CThe(player); " get"; MatchSubject(player); " ";
                        if object.prep
                                print object.prep;
                        elseif object is platform
                                print "onto";
                        else
                                print "into";
                        print " "; The(object); "."
                }
        }

        case &DoExit
        {
                select num
                case 1
                {
                        print CThe(player); IsorAre(player); " not ";
                        if object.prep
                                print object.prep;
                        elseif object is platform
                                print "on";
                        else
                                print "in";
                        print " "; The(object); "."
                }
                case 2
                {
                        print CThe(player); " get"; MatchSubject(player); " ";
                        if object.prep #2
                                print object.prep #2;
                        elseif object is platform
                                print "off";
                        else
                                print "out";
                        print " of "; The(object); "."
                }
        }

        case &DoInventory:
                print CThe(player); IsorAre(player); " not carrying anything."

        case &DoClose
        {
                select num
                case 1:  print CThe(player); " can't close "; The(object); "."
                case 2:  print CThe(object); IsorAre(object, true); " already closed."
                case 3:  print "Closed."
                case 4:  print "\nEverything goes dark."

        }

        case &DoOpen
        {
                select num
                case 1:  print CThe(player); " can't open "; The(object); "."
                case 2:  print CThe(object); IsorAre(object, true); " already open."
                case 3:  print CThe(object); IsorAre(object, true); " locked."
                case 4:  print "Opened."
        }

        case &DoMove:  print CThe(player); " can't move "; The(object); "."

        case &DoGet
        {
                select num
                case 1:  print CThe(player); " already"; \
                        MatchPlural(player, "has", "have"); " that."
                case 2:  print "This is not progress "; The(player); \
                        IsorAre(player, true); " making."
                case 3:  print CThe(object); " probably wouldn't be too big
                                on that idea."
                case 4
                {
                        print CThe(parent(object)); \
                                MatchPlural(object, "doesn't", "don't"); \
                                " want to give "; The(object); " to "; \
                                The(player, true); "."
                }
                case 5:  print CThe(parent(object)); \
                        IsOrAre(parent(object), true); " closed."
                case 6
                {
                        print CThe(player); " can't take "; The(object); \
                                " while "; player.pronoun; IsorAre(player); \
                                " ";
                        if object.prep:  print object.prep;
                        elseif object is platform:  print "on";
                        else:  print "in";
                        if object is plural:  print " them."
                        else:  print " it."
                }
                case 7
                {
                        print CThe(player); " can't take";
                        MatchPlural(object, "that", "those")
                        print "."
                }
                case 8:  print "Taken."
                case 9:  print CThe(player); IsorAre(player); " carrying
                        too much to take that."
        }

        case &ImplicitTake
        {
                print "(taking "; The(object); " first)"
        }

        case &DoDrop
        {
                select num
                case 1:  print CThe(player); " will have to take "; \
                        The(object); " off first."
                case 2:  print "Dropped."
        }

        case &DoPutonGround
        {
                print CThe(player); IsorAre(player); " not ";
                if xobject.prep
                        print xobject.prep;
                else
                        print "in";
                print " "; The(xobject); "."
        }

        case &DoPutIn
        {
                select num
                case 1:
                {
                        print "You'll have to be a little more specific
                                about exactly with what you'd like ";
                        if player_person ~= 2:  print The(player, true); " ";
                        print "to do that."
                }
                case 2:  print CThe(xobject); IsorAre(xobject, true); \
                                " closed."
                case 3:  print CThe(player); " can't do that."
                case 4:  print "Trying to bend the laws of physics, are we?"
                case 5
                {
                        print CThe(player); " put"; MatchSubject(player); \
                                " "; The(object); " ";
                        if xobject.prep
                                print xobject.prep;
                        elseif xobject is platform
                                print "on";
                        else
                                print "in";
                        print " "; The(xobject); "."
                }
                case 6
                {
                        print "There's no room ";
                        if xobject.prep
                                print xobject.prep; " ";
                        elseif xobject is platform
                                print "on ";
                        else
                                print "in ";
                        print The(xobject); "."
                }
                case 7:  print CThe(player); " can't do that with "; \
                        The(xobject); "."
                case 8
                {
                        print CThe(object); IsorAre(object, true); " already ";
                        if xobject.prep
                                print xobject.prep;
                        elseif xobject is platform
                                "on";
                        else
                                "in";
                        print " "; The(xobject); "."
                }
        }

        case &DoEmpty
        {
                select num
                case 1
                {
                        print CThe(object); IsorAre(object, true); \
                                " closed."
                }
                case 2:  print CThe(object); IsorAre(object, true); \
                                " already empty."
                case 3:  print CThe(player); " can't move that."
        }

        case &DoGive
        {
                select num
                case 1:
                {
                        print "Try ";
                        if player_person ~= 2
                                print "having "; The(player, true); " give";
                        else:  print "giving";
                        print " something to someone in particular."
                }
                case 2:
                {
                        print "Not quite sure what you hope ";
                        if player_person ~= 2:  print The(player); " will";
                        else:  print "to";
                        print " accomplish with that."
                }
                case 3:
                {
                        print "The kind gesture goes unnoticed by "; \
                                The(xobject); "."
                }
                case 4
                {
                        print CThe(xobject);
                        if xobject is not unfriendly
                                print " politely";
                        print " refuse"; MatchSubject(xobject); "."
                }
                case 5:  print "(taking "; The(object); " first)"
        }

        case &DoShow
        {
                select num
                case 1:
                {
                        print "Try ";
                        if player_person ~= 2
                                print "having "; The(player, true); " show";
                        else:  print "showing";
                        print " something to someone in particular."
                }
                case 2:  print "Showing "; Art(object); " to "; The(xobject); \
                                " doesn't accomplish a whole lot."
                case 3:  print CThe(xobject); \
                                MatchPlural(xobject, "doesn't", "don't"); \
                                " seem too interested in "; \
                                The(object); "."

        }

        case &DoWear
        {
                select num
                case 1:  print CThe(player); " can't wear "; The(object); "."
                case 2:  print CThe(player); IsorAre(player); " already
                        wearing that."
                case 3:  print CThe(player); " put"; MatchSubject(player); \
                        " on "; The(object); "."
        }

        case &DoTakeOff
        {
                select num
                case 1:  print CThe(player); " can't do that with "; \
                        The(object); "."
                case 2:  print CThe(player); IsorAre(player); " not
                        wearing that."
                case 3:  print CThe(player); " take"; MatchSubject(player); \
                        " off "; The(object); "."
        }

        case &DoSwitchOn
        {
                select num
                case 1:  print CThe(object); IsorAre(object); " already turned on."
                case 2:  print "Switched on."
        }

        case &DoSwitchOff
        {
                select num
                case 1:  print CThe(object); IsorAre(object); " already turned off."
                case 2:  print "Switched off."
        }

        case &DoUnlock
        {
                select num
                case 1:  print "That doesn't seem to do the trick."
                case 2:  print CThe(player); IsorAre(player); " not holding
                        any key that fits."
                case 3:  print CThe(object); IsorAre(object); " already unlocked."
                case 4
                {
                        if not xobject and object.key_object
                                print "(with "; The(object.key_object); ")"
                        print "Unlocked."
                }
        }

        case &DoLock
        {
                select num
                case 1:  print CThe(object); IsorAre(object); " already locked."
                case 2:  print CThe(player); " will have to close "; \
                        object.pronoun; " first."
                case 3
                {
                        if not xobject and object.key_object
                                print "(with "; The(object.key_object); ")"
                        print "Locked."
                }
        }

        case &DoEat:  print CThe(player); " can't eat "; The(object); "."

        case &DoDrink:  print CThe(player); " can't drink "; The(object); "."

        case &DoHit:  print "Venting "; player.pronoun #3; " frustrations
                        on "; The(object); " won't accomplish much."

#endif  ! ifclear NO_VERBS

#ifclear NO_XVERBS

        case &DoHello:  print "And to you."

        case &DoBrief:  print "Brief descriptions."
        case &DoSuperBrief:  print "Superbrief descriptions."
        case &DoVerbose:  print "Verbose descriptions."

        case &DoDisplay
        {
                select num
                case 1:  print "Tall listings."
                case 2:  print "Wide listings."
                case 3:  print "Type \"display tall\" or \"display wide\" to
                                alter the way objects are listed."
        }

        case &DoSave
        {
                select num
                case 1:  print "Saved."
                case 2:  print "Unable to save."
        }

        case &DoRestore
        {
                select num
                case 1:  print "Restored."
                case 2:  print "Unable to restore."
        }

        case &DoQuit:  print "Are you sure you want to quit (YES or NO)? ";

        case &DoRestart
        {
                select num
                case 1:  print "Are you sure you want to restart (YES or
                                NO)? ";
                case 2:  print "Unable to restart."
        }

        case &DoScriptOnOff
        {
                select num
                case 1:  print "Unable to begin transcription."
                case 2:  print "Transcription on."
                case 3:  print "Unable to end transcription."
                case 4:  print "Transcription off."
        }

#ifclear NO_RECORDING
        case &DoRecordOnOff
        {
                select num
                case 1:  print "Unable to begin command playback."
                case 2:  print "Command playback beginning."
                case 3:  print "Unable to begin command recording."
                case 4:  print "Command recording on."
                case 5:  print "Unable to end command recording."
                case 6:  print "Command recording off."
        }
#endif

        case &DoScore:  print "Nobody's keeping score."

        case &DoUndo:  print "Unable to undo."

#endif  ! ifclear NO_XVERBS
}

routine NewVMessages(r, num, a, b)      ! The NewVMessages routine may be
{                                       ! replaced, and should return true
        return false                    ! if a replacement message <num>
}                                       ! exists for routine <r>

#endif  ! _VERBLIB_H
