!----------------------------------------------------------------------------
!
!         Hugo Object Library v3.1.03.2 by Kent Tessman (c) 1995-2006
!                      for use with the Hugo Compiler
!
!----------------------------------------------------------------------------
!
!       The Hugo Object Library contains the following useful class
!       definitions:
!
!               room
!               character, female_character
!               player_class
!               door
!               direction (including direction objects)
!               scenery
!               component
!
!               vehicle
!               plural_class, identical_class
!               attachable
!
!       (NOTE:  In order to use the vehicle class, plural_class or
!       identical_class, or the attachable class, the flags USE_VEHICLES,
!       USE_PLURAL_OBJECTS, or USE_ATTACHABLES, respectively, must be set.)
!
!----------------------------------------------------------------------------

#ifclear _OBJLIB_H
#set _OBJLIB_H

#ifclear _COMPILING_HUGOLIB
#message warning "OBJLIB.H not included separately from HUGOLIB.H"
#else

!----------------------------------------------------------------------------
! BASIC ROOM:
!----------------------------------------------------------------------------

class room
{
        type room
        is static, light, open
}


!----------------------------------------------------------------------------
! STANDARD CHARACTERS:
!----------------------------------------------------------------------------

class character
{
        type character
        pronouns "he", "him", "his", "himself"
        capacity 50
        holding 0
        is living, transparent, static
}

class female_character
{
        type female_character
        pronouns "she", "her", "her", "herself"
        capacity 50
        holding 0
        is living, transparent, static, female
}

character player_character
{
        ! 'nouns' and 'pronouns' as defined below are the defaults
        ! for second-person, i.e., when player_person is 2--'name'
        ! should be "you"
        !
        nouns "me", "myself"
        pronouns "you", "you", "your", "yourself"

        long_desc
        {
                print "Looking good."
        }

        capacity 100
        is hidden
        is plural       ! because player_person defaults to 2
}


!----------------------------------------------------------------------------
! REFLEXIVE NOUNS:
!----------------------------------------------------------------------------

property self_resolve alias capacity
property self_object alias holding

class self_class
{
        type self_class
        before
        {
                object
                {
                        Perform(verbroutine, self.self_object, xobject)
                }
                xobject
                {
                        if object > themselves
                        {
                                ! Rule out numbers used as xobjects:
                                local i
                                for (i=words; i>2; i--)
                                {
                                        if word[i] <= 0 or StringToNumber(word[i]) = xobject
                                        {
                                                return false
                                        }
                                }

                                if not self.self_resolve
                                {
                                        OMessage(self_class, 1)
                                }
                                else
                                {
                                        Perform(verbroutine, object, self.self_object)
                                }
                        }
                        else
                                return false
                }
        }
        is known
}

self_class himself "himself"
{
        noun "himself"
        self_object:    return him_obj
        self_resolve
        {
                if object is plural or object is female or object is not living
                        return false
        }
}

self_class herself "herself"
{
        noun "herself"
        self_object:    return her_obj
        self_resolve
        {
                if object is plural or object is not female or object is not living
                        return false
        }
}

self_class itself "itself"
{
        noun "itself"
        self_object:    return it_obj
        self_resolve
        {
                if object is plural or object is living
                        return false
        }
}

self_class themselves "themselves"
{
        noun "themselves"
        self_object:    return them_obj
        self_resolve
        {
                if object is not plural
                        return false
        }
}


!----------------------------------------------------------------------------
! COMPASS DIRECTIONS:
!----------------------------------------------------------------------------

property dir_to alias capacity          ! for directions only
property dir_from alias size            !

class direction
{
        type direction
        article "the"
        found_in
        {
                if self in direction
                        return location
        }
        long_desc
        {
                OMessage(direction, 1)
        }
        is static, known
}

direction n_obj "north"
{
        in direction
        nouns "n", "north"
        dir_to n_to
        dir_from s_obj
}

direction ne_obj "northeast"
{
        in direction
        nouns "ne", "northeast"
        dir_to ne_to
        dir_from sw_obj
}

direction e_obj "east"
{
        in direction
        nouns "e", "east"
        dir_to e_to
        dir_from w_obj
}

direction se_obj "southeast"
{
        in direction
        nouns "se", "southeast"
        dir_to se_to
        dir_from nw_obj
}

direction s_obj "south"
{
        in direction
        nouns "s", "south"
        dir_to s_to
        dir_from n_obj
}

direction sw_obj "southwest"
{
        in direction
        nouns "sw", "southwest"
        dir_to sw_to
        dir_from ne_obj
}

direction w_obj "west"
{
        in direction
        nouns "w", "west"
        dir_to w_to
        dir_from e_obj
}

direction nw_obj "northwest"
{
        in direction
        nouns "nw", "northwest"
        dir_to nw_to
        dir_from se_obj
}

direction u_obj "above"
{
        in direction
        nouns "u", "up"
        article ""
        dir_to u_to
        dir_from d_obj
}

direction d_obj "below"
{
        in direction
        nouns "d", "down"
        article ""
        dir_to d_to
        dir_from u_obj
}

direction in_obj "in"
{
        in direction
        noun "in", "inside"
        article ""
        dir_to in_to
        dir_from out_obj
        long_desc
        {
                OMessage(direction, 2)
        }
}

direction out_obj "out"
{
        in direction
        noun "out", "outside"
        article ""
        dir_to out_to
        dir_from in_obj
        long_desc
        {
                OMessage(direction, 2)
        }
}


!----------------------------------------------------------------------------
! DOORS:
!
! This door object assumes that the object will ultimately have
! exactly two components to the between property:  one on either
! side of the door.
!----------------------------------------------------------------------------

property between alias found_in

class door "door"
{
        type door
        door_to                            ! depends on actor location
        {
                local currentroom
                if actor = player
                        currentroom = location
                else
                        currentroom = parent(actor)

                ! sitting on or in something
#ifclear USE_VEHICLES
                if actor = player and player not in location
#else
                if actor = player and player not in location and
                        parent(player).type ~= vehicle:
#endif
                {
                        ! "You'll have to get out first..."
                        OMessage(door, 1)
                        return true
                }

                if self is not open and actor = player
                {
#ifclear NO_VERBS
                        if verbroutine = &DoGo
                        {
                                if self is not locked and self is openable:
                                {
                                        OMessage(door, 2)       ! "(opening it first)"
                                        Perform(&DoOpen, self)
                                        Main    ! counts as a turn
                                        if self is not open
                                                return true
                                }
                                elseif self is lockable
                                {
                                        OMessage(door, 3)      ! "It is locked."
                                        return true
                                }
                                else
                                {
#endif
                                        OMessage(door, 4)       ! "It is closed."
                                        return true
#ifclear NO_VERBS
                                }
                        }
#endif
                }

                if self is not open and (actor in location or
                        location = self.between #((parent(actor) = \
                                self.between #1) + 1))
                {
                        ! Notify the player that the player or a character
                        ! has gone through the door:
                        !
                        OMessage(door, 5)
                        general = 2
                }

                if currentroom = self.between #1
                        return self.between #2
                else
                        return self.between #1
        }
        long_desc
        {
                if self is open
                        OMessage(door, 6)       ! when open
                else
                        OMessage(door, 7)       ! when closed
        }
        is openable, static

#ifset VERBSTUBS
        before
        {
                object DoPush
                {
                        return Perform(&DoOpen, self)
                }
                object DoPull
                {
                        return Perform(&DoClose, self)
                }
        }
#endif
}


!----------------------------------------------------------------------------
! SCENERY:
!
! Scenery objects are accessible to actions, but are not individually listed
! as part of the room contents (although their children are).  They are
! intended to be mentioned in the room description, and still require
! explicit names, articles, nouns, and adjectives.  The type property
! of a scenery object should never be changed.
!----------------------------------------------------------------------------

class scenery
{
        type scenery
        exclude_from_all
        {
#ifclear NO_VERBS
                if verbroutine = &DoGet
                        return true
                else
#endif
                        return false
        }
        is static, hidden
}


!----------------------------------------------------------------------------
! COMPONENT:
!
! Components are essentially movable scenery objects, which are considered
! to be attached to another object.  The part_of property is initialized
! with the object of which the new object is a component.  Note that
! children of components are not listed in the room description; their
! children are, however, listed when directly examining the object.
!----------------------------------------------------------------------------

property part_of alias found_in

class component
{
        type component
        part_of nothing

#ifclear NO_VERBS
        before
        {
                object DoGet, DoDrop, DoPutIn
                {
                        OMessage(component)     ! "You can't separate that..."
                }
        }
        after
        {
                xobject DoAsk, DoTell
                {
                        return Perform(verbroutine, object, self.part_of)
                }
                object DoShow, DoGive
                {
                        return Perform(verbroutine, self.part_of, xobject)
                }
        }
        exclude_from_all
        {
                if verbroutine = &DoGet
                        return true
                else
                        return false
        }
#else
        exclude_from_all
                return false
#endif
        is static, hidden
}


!----------------------------------------------------------------------------
! VEHICLE:
!
! This vehicle object expects the object's vehicle_move property routine
! to return true if the vehicle is presently capable of movement; if it
! isn't, vehicle_move is responsible for printing an appropriate message
! before returning false.
!
! The vehicle_verbs property contains a list of the valid verbs for vehicle
! movement, while the vehicle_path property contains a list of all the
! vehicles which may travel in a particular room object.
!
! The DoMoveinVehicle routine may conceivably be used by all basic vehicles
! in the game.  However, new grammar incorporating the vehicle movement verbs
! for each object (corresponding exactly to the list under vehicle_verbs)
! must be specified, as in:
!
!       verb "<verb1>"[, "<verb2>",...]
!               *                       DoMoveinVehicle
!               * object                DoMoveinVehicle
!               ...
!
! For different vehicle objects, the verbs (and syntax) may change, but
! the call to DoMoveinVehicle remains the same.
!
! Because of the relative complexity of the vehicle object, it must be
! explicitly included by setting the USE_VEHICLES flag.
!----------------------------------------------------------------------------

#ifset USE_VEHICLES

property vehicle_move                   ! for vehicles only
property vehicle_verbs                  !  "     "      "
property vehicle_verb alias vehicle_verbs

property vehicle_path                   ! for rooms only

class vehicle
{
        type vehicle
        vehicle_verb "drive"            ! default verb
        prep "in", "out"                !   "     prepositions
        vehicle_move true               ! by default, always ready to move

#ifclear NO_VERBS
        before
        {
                parent(player) DoGo
                {
                        ! "To walk, you'll have to get out..."
                        OMessage(vehicle, 1, self)
                }
        }
#endif
        is enterable, static
}

routine DoMoveinVehicle
{
        local v, moveto

        if player in location
        {
                OMessage(vehicle, 2)    ! "You aren't in anything."
                return false
        }

        v = parent(player)              ! the vehicle
        if v.before:  return true

        ! Match the verb
        if not InList(v, vehicle_verbs, VerbWord)
        {
                OMessage(vehicle, 3, v)        ! wrong verb for this vehicle
                return false
        }

        if not object or (object = v and player in object)
        {
                OMessage(vehicle, 4)   ! "Specify a direction as well..."
                return false
        }

        if not v.vehicle_move:  return true

        if obstacle
        {
                OMessage(vehicle, 5, v) ! "X stops you from moving."
                return true
        }

        if object.type ~= direction
        {
                moveto = object.door_to
                if not moveto
                        ! "You can't (drive) in there..."
                        OMessage(vehicle, 6, v)

                if moveto <= 1
                        return
        }
        else
                moveto = location.(object.dir_to)

        if not InList(moveto, vehicle_path, v) and moveto ~= 1
        {
                OMessage(vehicle, 7, v) ! "You can't (drive) that way."
                return false
        }
        elseif moveto = 0
        {
                if not location.cant_go
                        OMessage(vehicle, 7, v)
                return false
        }
        elseif moveto = 1               ! already printed message
                return false            !   (moveto is never 1)

#ifset USE_ATTACHABLES
        if ObjectisAttached(v, location, moveto)
                return false
#endif

        ! Finally, the vehicle can move
        move v to moveto
        v is moved
        old_location = location
        location = moveto

#ifset USE_ATTACHABLES
        MoveAllAttachables(v, old_location, location)
#endif

        if not FindLight(location)
                DarkWarning
        else
        {
                DescribePlace(location)
                location is visited
        }

        run parent(player).after

        return true
}

#endif  ! ifset USE_VEHICLES


!----------------------------------------------------------------------------
! PLURAL OBJECTS:
!
! (Based on an implementation proposed by Jim Newland.)
!
! Plural objects are unique objects that can be referred to by a common
! group name, such as "books" for a dictionary and a cookbook.  Identical
! objects are similar, except that the case is one of, for example, "coins"
! for two identical coins.
!
! The plural_of property contains a list of all objects belonging to the
! given plural_class (or identical_class).  The property single_noun holds
! the singular form(s) of the object name, i.e. "book" or "coin".
!
! Member objects are defined simply as objects, without using the class
! name in the declaration.  For identical objects, the identical_to
! property must be defined, giving the name of the identical_class to
! which the object belongs.  As well, the object in any action performed
! on an identical object is generally assumed to be the identical_class.
! That is, properties such as long_desc should be defined for the
! identical_class.
!
! Words in the adjectives property of the member objects should also be
! adjectives for the given plural or identical class.
!
! (NOTE:  Members of a plural_class may have plural_is (essentially
! identical_to for plural objects), but it is optional and used only for
! object listing.)
!
! The plural_verbs property should return true if the current verbroutine
! is in the list of valid actions for the class in question.
!
! The Init routine must contain the line:  InitPluralObjects
!----------------------------------------------------------------------------

#ifset USE_PLURAL_OBJECTS

global plural_count             ! number of plural/identical classes used
constant MAX_PLURALS 32         ! maximum number of classes
array plurals[MAX_PLURALS]

! These three properties must be specified for each plural/identical root
! object:
!
property plural_of              ! contains a list of all individual objects
property single_noun            ! i.e. "thing" vs. "things"
property identical_to           ! for member objs., set to the
                                !   identical_class to which they belong
property plural_is alias identical_to
property plural_parse_rank

property plural_verbs           ! returns true for valid verbroutines


! These are used by the library classes:
property pluralobj_number alias nw_to
global pluralobj_heldmode       ! 1, -1, 0 for held, notheld, or neither

class plural_class "(plural_class)"
{
        type plural_class
        in_scope 0                      ! may be an actor at some point
        size 0                          ! must be weightless
        pluralobj_number 0
        plural_verbs
        {
#ifclear NO_VERBS
                if verbroutine = &DoLook, &DoGet, &DoDrop, &DoPutin
                        return true
#ifset USE_CHECKHELD
                elseif verbroutine = &DoDrop_CheckHeld, &DoPutin_CheckHeld
                        return true
#endif
                else
#endif
                        return false
        }

        found_in
        {
                if self = plural_class, identical_class
                        return parent(player)
                else
                        return false
        }

        ! Set the parse_rank of this object to the (highest) parse_rank of
        ! its member objects
        plural_parse_rank -100
        parse_rank
        {
                if self.plural_parse_rank = -100
                {
                        local i, p, pr = 1
                        for (i=1; i<=self.#plural_of; i++)
                        {
                                p = (self.plural_of #i).parse_rank
                                if p > pr
                                        pr = p
                        }
                        self.plural_parse_rank = pr
                }
                return self.plural_parse_rank
        }

        before
        {
                object                  ! intercepts all plural_object verbs
                {
                        local i, j, n, obj, count, flag

! Count the number of workflagged member objects, to see if there are
! less than specified (or any at all, for that matter).

                        for (i=1; i<=self.#plural_of; i++)
                        {
                                if self.plural_of #i and self.plural_of #i is workflag
                                {
                                        obj = self.plural_of #i
                                        AssignPronoun(obj)
                                        n++
                                }
                        }

                        if n > 1:  AssignPronoun(self)

                        if n = 0
                        {
                                ! "There are none..."
!                               OMessage(plural_class, 1)

                                if pluralobj_heldmode = 1
                                        ! "You're not holding..."
                                        ParseError(15, self)
                                else
                                        ! "You don't see that..."
                                        ParseError(11, self)

                                return true
                        }

                        if (n < self.pluralobj_number) or
                                (n = 1 and self.pluralobj_number = 0):
                        {
                                ! "There are only x things..."
                                OMessage(plural_class, 2, n)
                                return true
                        }

! If it's not an allowable plural-objects action, disqualify it:

                        if self.pluralobj_number ~=1 and not self.plural_verbs
                        {
                                ! "You'll have to do that one at a time."
                                OMessage(plural_class, 3)
                                return true
                        }

! Handle examining objects as a group specially:
#ifclear NO_VERBS
                        if verbroutine=&DoLook
                        {
                                if self.pluralobj_number ~= 1
                                {
                                        if self.type = identical_class
                                                return false

                                        if self.long_desc
                                                return true
                                }

                                flag = 0
                                for (i=1; i<=self.#plural_of; i++)
                                {
                                        j = self.plural_of #i
                                        if j and &j.long_desc
                                                flag = true
                                }
                                if flag = 0:  return false
                        }
#endif  ! ifclear NO_VERBS

! Finally, process the action on workflagged individual objects:

                        for (i=1; i<=self.#plural_of; i++)
                        {
                                obj = self.plural_of #i
                                if obj and obj is workflag
                                {
                                        if n~=1 and self.pluralobj_number~=1
                                        {
                                                if self.type = identical_class and
                                                        obj.article
                                                {
                                                        print CountWord(count+1);
                                                        print " ";
                                                }
                                                print obj.name; ":  ";
                                        }

                                        Perform(verbroutine, obj, xobject)
                                        count++

                                        AssignPronoun(obj)
                                }

                                ! number of objects specified (if specified)
                                if count=self.pluralobj_number and count>0
                                        break
                        }
                }
        }
        is plural, known
}

plural_class identical_class "(identical_class)"
{
        type identical_class
        plural_parse_rank -100
        parse_rank
        {
                if self.plural_parse_rank = -100
                        self.plural_parse_rank = (self.plural_of).parse_rank + 1
                return self.plural_parse_rank
        }
}


! InitPluralObjects must be called in the Init routine in order for plural
! or identical objects to be used.

routine InitPluralObjects
{
        local obj, count

        for (obj=1; obj<=objects; obj++)
        {
                if obj.type = plural_class, identical_class
                {
                        if obj ~= plural_class, identical_class
                        {
                                plurals[count] = obj
                                if ++count >= MAX_PLURALS
                                {
                                        "[WARNING:  Maximum number of plural/identical
                                        classes exceeded.  Increase MAX_PLURALS in
                                        the Hugo Object Library.]"
                                        return
                                }
                        }
                }
        }
        plural_count = count
}


! ParsePluralObjects is called by the Parse routine.

routine ParsePluralObjects
{
        local i, j, k, n, w, wn
        local any, plist, pobj, plural_type, loc, xobj, pobj_number
        local 2ndpass

! Before doing anything, move all plural/identical object classes back to
! their respective holding classes:

        for (i=0; i<plural_count; i++)
                move plurals[i] to plurals[i].type

        if actor = 0:  actor = player

        pluralobj_heldmode = 0  ! i.e. not explicitly held
        wn = 2                  ! starting word number

:ScanforPluralWords

        pobj_number = 0         ! default explicit count (pluralobj_number)

! The first step is to scan the phrase for a potential plural object word:

        for (w=wn; w<=words; w++)
        {
                if word[w] = 0 or w>words
                        break           ! end of phrase

                if word[w] = "my"
                {
                        pluralobj_heldmode = 1
                }

                for plist in plural_class
                {
                        if InList(plist, single_noun, word[w])
                        {
                                pobj = plist
                                plural_type = single_noun
                                break
                        }

                        if InList(plist, noun, word[w])
                        {
                                pobj = plist
                                plural_type = noun
                                break
                        }
                }
                if pobj:  break

                for plist in identical_class
                {
                        pobj = 0

                        if InList(plist, single_noun, word[w])
                        {
                                pobj = plist
                                pobj_number = 1
                                plural_type = single_noun
                                break
                        }

                        if InList(plist, noun, word[w])
                        {
                                pobj = plist
                                plural_type = noun
                                break
                        }
                }
                if pobj:  break
        }
        if not pobj:  return false      ! didn't find anything

! At this point, <w> points to the word and <pobj> is the object number of
! the plural class.  <plural_type> is either noun or single_noun.  What
! follows is the actual parsing of the plural structure:

! Determine (roughly) if object is explicitly held or notheld; the
! SetVerbHeldmode routine checks to see if the verb necessarily implies a
! specific heldmode by checking the first 6 words--i.e., even if the first
! five represent an object name in an actor-directed command.

        if not 2ndpass
        {
                for (i=1; i<=6 and i<=w; i++)
                {
                        if pluralobj_heldmode:  break

                        if word[i] = "":  break         ! end of phrase

                        SetVerbHeldmode(word[i])
                }

! If it is a single_noun (i.e. "any thing" or "either thing"), the object
! global will have to be appropriately determined.  <pobj> will hold the
! selected object (if any).  In any case, it will be necessary to know if
! at least one object (in the case of multiple plural object specification)
! is available, in order to move the plural_class into scope.

! To boot, it could also get ugly if a container or platform xobject is
! specified as the location for the object(s) being acted upon.  There is
! no polite way of figuring out what the xobject might be at this point.

                for (i=w; i<words; i++)
                {
                        k = word[i]             ! for shorthand purposes only
                        if k = 0:  break        ! end of phrase

                        if (k = "from", "outof", "offof", "off") or
                                (pluralobj_heldmode ~= 1 and
                                (k = "in" or k = "inside" or k = "on"))
                        {
                                xobj = 0        ! nothing selected yet
                                do
                                {
                                        i=i+1   ! get next word

                                        for (j=1; j<=pobj.#plural_of; j++)
                                        {
                                                n = parent(pobj.plural_of #j)
                                                if ObjWord(word[i], n)
                                                        xobj = n
                                        }
                                }
                                while i<=words and word[i]~=0

                                break
                        }
                }

                n = 0
                if xobj
                        loc = xobj
                elseif actor = player
                        loc = location
                else
                        loc = parent(actor)
        }

! Remove the old plural_class adjective(s), if any

        while w > 1 and (ObjWord(word[w-1], pobj) = adjective or word[w-1] = "my")
        {
                if word[w-1] = "my"
                {
                        ! Try to differentiate between, for instance, "my door"
                        ! out of a set of door, and "my key", meaning the one
                        ! the player is holding:
                        for (i=1; i<=pobj.#identical_to; i++)
                        {
                                if Contains(player, pobj.identical_to #i)
                                        break

                        }
                        if i > pobj.#identical_to
                                break
                }

                DeleteWord(w-1)
                w--
        }

! Then, find out if there is a preceding number

        if w > 1
        {
                k = WordisNumber(word[w-1])
                if k >= 1                       ! e.g., "one (or more) things"
                {
                        DeleteWord(w-1)
                        w--
                        pobj_number = k
                }

                if WordisNumber(word[w-1])>=1   ! "two of the three"
                {
                        pobj_number = WordisNumber(word[w-1])
                        DeleteWord(w-1)
                        w--
                }
        }

! Remove a preceding "all" or "any"; i.e. "all things" is the same as "things"

        if w>1 and (word[w-1] = "~all" or word[w-1] = "~any")
        {
                if word[w-1] = "~any":  any = true
                DeleteWord(w-1)
                w--
        }

! Now see if the object (or a member of the plural_class) is available given
! the restrictions of pluralobj_heldmode:

        n = 0
        j = 0

        if pluralobj_heldmode = -1              ! explicitly notheld object
        {
                for (i=1; i<=pobj.#plural_of; i++)
                {
                        k = pobj.plural_of #i
                        k is not workflag

                        if k in loc
                        {
                                n = k
                                k is workflag
                                j++
                        }
                        if n and plural_type = single_noun:  break
                }
        }

        if not n or pobj_number = 0 or j < pobj_number
        {
                for (i=1; i<=pobj.#plural_of; i++)
                {
                        k = pobj.plural_of #i

                        ! explicitly notheld object
                        if pluralobj_heldmode = -1 and k is not workflag
                        {
                                if FindObject(k, loc) and k not in actor
                                {
                                        n = k
                                        k is workflag
                                        j++
                                }
                        }
                        elseif pluralobj_heldmode ~= -1
                                k is not workflag

                        ! explicitly held object
                        if pluralobj_heldmode = 1
                        {
                                if k in actor
                                {
                                        n = k
                                        k is workflag
                                        j++
                                }
                        }

                        ! or neither
                        elseif pluralobj_heldmode = 0
                        {
                                if FindObject(k, loc)
                                {
                                        n = k
                                        k is workflag
                                        j++
                                }
                        }

                        if n and plural_type = single_noun:  break
                        if j and j = pobj_number:  break
                }
        }

! Even if no object is found, it will be necessary to insert one in the
! input line to generate a "You don't see that"-esque parser message in
! the case of a single_noun specification.

        if plural_type = single_noun and not n
                n = pobj.plural_of #1

! If it's not a plural specification--i.e. it is an "any" or "either"
! phrasing instead--change the plural object word to represent <n>.

        i = words

        if plural_type = single_noun
        {
                if pobj.type ~= identical_class and any
                {
                        SetObjWord(w, n)
                        if pobj.type = plural_class
                                print "("; The(n); ")"
                }
                elseif pobj.type = identical_class
                {
                        SetObjWord(w, pobj)

                        if pluralobj_heldmode = 1       ! in inventory
                                move pobj to actor
                        elseif xobj                     ! in or on something
                                move pobj to xobj
                        else                            ! anything else
                                PutinScope(pobj, actor)
                }
        }

! If it is a plural specification--plural_class.noun instead of single_noun--
! move the plural class into an accessible position, but only if one of
! the component members is available.

        elseif n and plural_type = noun
        {
                ! Insert the name of the identical_class, if necessary
                if pobj.type = identical_class
                        SetObjWord(w, pobj)

                if pluralobj_heldmode = 1       ! in inventory
                        move pobj to actor
                elseif xobj                     ! in or on something
                        move pobj to xobj
                else                            ! anything else
                        PutinScope(pobj, actor)
        }

        pobj.pluralobj_number = pobj_number     ! # of plural objects

        wn = w + pobj.#adjectives + (pobj.noun > 0)
        if wn < i
        {
                pobj = 0
                2ndpass = true
                jump ScanforPluralWords
        }

        parser_data[PARSER_STATUS] |= PRONOUNS_SET  ! since we rebuilt object(s)

        return true
}


! SetVerbHeldmode sets the pluralobj_heldmode global to the appropriate value
! 1, -1, or 0, based on the verb being used; returns true if the verb
! indicates a particular heldmode.  This routine can be replaced in order
! to change the selection of verbs.

routine SetVerbHeldMode(a)
{
        local vhm
        vhm = VerbHeldMode(a)
        if vhm = -1
                pluralobj_heldmode = -1
        elseif vhm = 1
                pluralobj_heldmode = 1

        return (pluralobj_heldmode~=0)
}


! WordisNumber returns the number given by the specified word (where the
! argument is, for example, word[x]).  If the word is not a number, or
! is out of range, -1 is returned.

routine WordisNumber(w)
{
        select w
        case "zero", "0":       return 0
        case "one", "1":        return 1
        case "two", "2":        return 2
        case "three", "3":      return 3
        case "four", "4":       return 4
        case "five", "5":       return 5
        case "six", "6":        return 6
        case "seven", "7":      return 7
        case "eight", "8":      return 8
        case "nine", "9":       return 9
        case "ten", "10":       return 10
        case else:              return -1
}


! CountWord prints an appropriate article as specified by the value <n>.

routine CountWord(n)
{
        select n
        case 1:         return "first"
        case 2:         return "second"
        case 3:         return "third"
        case 4:         return "fourth"
        case 5:         return "fifth"
        case 6:         return "sixth"
        case 7:         return "seventh"
        case 8:         return "eighth"
        case 9:         return "ninth"
        case 10:        return "tenth"
        case else:      return "next"
}


! RemoveIdentical removes the object <obj> from its current identical_class;
! used for instances where, for example, an indistinguishable object is
! marked or otherwise made unique.

routine RemoveIdentical(obj)
{
        local j, o

        for (o=0; o<plurals; o++)
        {
                j = InList(plurals[o], plural_of, obj)
                if j                                    ! if it matches
                {
                        plurals[o].plural_of #j = 0     ! remove it
                        return true
                }
        }
        return false
}


! AddIdentical adds (i.e., returns) object <obj> to the identical_class <i>.

routine AddIdentical(obj, i)
{
        local j

        for (j=1; j<i.#plural_of; j++)
        {
                if i.plural_of #j = 0           ! a blank slot
                {
                        i.plural_of #j = obj    ! add it
                        return j
                }
        }
        return false
}

#endif  ! ifset USE_PLURAL_OBJECTS


!----------------------------------------------------------------------------
! ATTACHABLE:
!
! Because ropes, chains, and other such household items are notoriously
! hard to code.
!
! The attachable_to property contains a list of all items to which the
! object may be attached.  The attached_to property will ultimately contain
! the objects to which the object is attached; it must be defined with an
! appropriate number of elements.  For example, a rope that can be tied
! between two things must have the following in its object definition:
!
! attachable rope "rope"                ! or something similar
! {
!       ...
!       attached_to 0, 0
!       ...
! }
!
! so that attached_to can hold a total of two object numbers.
!
! A harness that is already attached to a wagon, but which can also be
! attached to six horses (objects) at the same time, might be initialized
! as follows:
!
!       attached_to wagon, 0, 0, 0, 0, 0, 0
!
! with room for seven elements.
!
! The attach_take and attach_drop properties are normally not set.  If
! attach_take is true, an attempt to take the attachable is made before
! attaching (or detaching) it.  If attach_drop is true, the object is
! automatically dropped after it is attached.
!
! The attach_verbs and detach_verbs properties contain lists of all valid
! verbs to attach or detach the object.  Similarly to the vehicle class's
! DoMoveinVehicle routine, the DoAttachObject and DoDetachObject can be
! used by all basic attachables, with new grammar specified for the object
! (corresponding exactly to the lists under attach_verbs and detach_verbs),
! as in:
!
!       verb "<verb1>"[, "<verb2>",...]
!               *                       DoVague
!               * object                DoAttachObject
!               * object "to" xobject   DoAttachObject
!               ...
!
! Note that DoAttachObject expects a second object (the xobject) to be
! given.
!
! To check if a particular object is kept immovable by an attachable,
! call ObjectisAttached(<object>); it returns the number of the attachable,
! or false if there is none.
!
! AttachObject and DetachObject may be used to directly attach or detach
! an attachable object.
!
! Also, any routine that moves the player or player's parent--such as
! MovePlayer or DoMoveinVehicle--should call MoveAllAttachables to
! reconcile the location of attached objects (since they are not necessarily
! connected via the object tree).  Objects with the mobile attribute set
! may be dragged.
!
! Non-attachables may have the attach_immobile property, which governs
! whether or not they may be pulled, dragged, etc. by returning 0 when
! freely moveable or 1 (true) if something is keeping it from moving.  In
! the second case, attach_immobile is responsible for printing any
! explanatory message.
!----------------------------------------------------------------------------

#ifset USE_ATTACHABLES

property attachable_to
property attached_to

property attach_prep
property detach_prep
property attached_desc

property attach_shortdesc       ! Describe attachables using these
property attach_longdesc        ! in place of short_desc and long_desc

property attach_verb
property detach_verb
property attach_verbs alias attach_verb
property detach_verbs alias detach_verb

property attach_take            ! Normally not set, these are true if an
property attach_drop            ! implicit take or drop is required

property attach_immobile        ! for non-attachables; returns 0 if moveable,
                                ! 1 if not (after printing explanation)

class attachable
{
        type attachable
        attach_verbs "tie", "attach"
        detach_verbs "untie", "detach"
        attach_prep "to"
        detach_prep "from"
        attached_desc "tied to"
        attached_to 0           ! 1 default attachment point
        desc_detail
        {
                local i

                for (i=1; i<=self.#attached_to; i++)
                {
                        if self.attached_to #i
                        {
                                print " ("; self.attached_desc; " ";
                                ListAttachments(self)
                                print ")";
                                break
                        }
                }
        }
        short_desc
        {
                local a, i

                a = 0

                ! See first if it's attached to anything
                for (i=1; i<=self.#attached_to; i++)
                {
                        if self.attached_to #i
                                a = self.attached_to #i

                }

                if not a                ! not attached to anything
                {
                        a = self.attach_shortdesc
                        if not a
                                Message(&DescribePlace, 2, self)
                        return true
                }
                else
                {
                        if self.attach_shortdesc
                        {
                                print "\_ ";
                                OMessage(attachable, 3)  ! "Attached to..."
                        }
                        else:  OMessage(attachable, 1)
                }
        }
        long_desc
        {
                local a, i, desc

                a = 0

                ! See first if it's attached to anything
                for (i=1; i<=self.#attached_to; i++)
                {
                        if self.attached_to #i
                                a = 1
                }

                if not self.attach_longdesc
                        OMessage(attachable, 2) ! "You see nothing special..."
                if a            ! if it's attached to something(s)
                {
                        print ""
                        OMessage(attachable, 3) ! "Attached to..."
                }
        }

#ifclear NO_VERBS
        before
        {
                object DoGet
                {
                        local i, w

                        self is not workflag
                        if self.attach_take or self.attach_drop
                        {
                                for (i=1; i<=self.#attached_to; i++)
                                {
                                        if self.attached_to #i and
                                                self.#attached_to > 1:
                                        {
                                                ! "You'll have to (detach) it..."
                                                OMessage(attachable, 4)
                                                return true
                                        }
                                        elseif self.attached_to #i
                                        {
                                                if self.detach_verb~=VerbWord
                                                        ! "(having to (detach) it first)"
                                                        OMessage(attachable, 5, i)
                                                w = word[1]
                                                word[1] = self.detach_verb
                                                xobject = self.attached_to #i
                                                if not DoDetachObject
                                                        return true
                                                Main    ! counts as a turn
                                                word[1] = w
                                                self is workflag
                                                return false
                                        }
                                }
                        }
                        return false
                }

#ifset VERBSTUBS
                object DoTie
                        {Perform(&DoAttachObject, object, xobject)}
                object DoUntie
                        {Perform(&DoDetachObject, object, xobject)}
#endif
        }
        after
        {
                object DoGet
                {
                        if self is not workflag
                                return false
                }
        }

#endif  ! ifclear NO_VERBS
}

routine DoAttachObject
{
        if object.type ~= attachable
        {
                OMessage(attachable, 6)        ! "You can't (attach) that."
                return false
        }

        ! Match the verb
        if not InList(object, attach_verbs, VerbWord)
        {
                OMessage(attachable, 7, object.attach_verb)    ! wrong verb
                return false
        }

        if not xobject
        {
                OMessage(attachable, 8)        ! "Be more specific..."
                return false
        }

        if not CheckReach(xobject)
                return false

#ifclear NO_VERBS
        if object.attach_take and object not in player
        {
                OMessage(attachable, 9)         ! "(taking it first)"
                Perform(&DoGet, object)
                if object not in player
                        return false
                Main                    ! counts as a turn
        }
#endif

        ! See if the attach-to object is appropriate
        if object.attachable_to and not InList(object, attachable_to, xobject)
        {
                ! "You can't (attach) it to that."
                OMessage(attachable, 10)
                return false
        }

        ! Make sure it's not already attached
        if InList(object, attached_to, xobject)
        {
                ! "Except that it is already (attached) to that."
                OMessage(attachable, 11)
                return false
        }

        if not ObjectAttach(object, xobject)
        {
                ! "Except that it is already (attached) to (list)..."
                OMessage(attachable, 12)
                return false
        }

        if not xobject.after
        {
                if not object.after
                        ! "You (attach) it to..."
                        OMessage(attachable, 13)
        }

        return true
}

routine DoDetachObject
{
        local i, j

        if object.type ~= attachable
        {
                OMessage(attachable, 6)        ! "You can't (detach) that."
                return false
        }

        ! Match the verb
        if not InList(object, detach_verbs, VerbWord)
        {
                OMessage(attachable, 7, object.detach_verb)    ! wrong verb
                return false
        }

        if not CheckReach(xobject)
                return false

        ! If second object is given, make sure it is attached
        if xobject and not InList(object, attached_to, xobject)
        {
                ! "Except that it is not (attached) to that."
                OMessage(attachable, 14)
                return false
        }
        elseif not xobject
        {
                j = 0
                for (i=1; i<=object.#attached_to; i++)
                {
                        if object.attached_to #i
                        {
                                j++
                                xobject = object.attached_to #i
                        }
                }
                if j = 0
                {
                        ! "Except that it is not (attached) to anything."
                        OMessage(attachable, 15)
                        return false
                }
                elseif j > 1
                {
                        ! "Be a little more specific..."
                        OMessage(attachable, 16)
                        return false
                }
        }

#ifclear NO_VERBS
        if object.attach_take and object not in player and verbroutine~=&DoGet
        {
                OMessage(attachable, 9)         ! "(taking it first)"
                Perform(&DoGet, object)
                if object not in player
                        return false
                Main                    ! counts as a turn
                return true
        }
#endif

        ObjectDetach(object, xobject)

        j = verbroutine
        verbroutine = &DoDetachObject
        if not xobject.after
        {
                if not object.after
                        ! "You detach it from..."
                        OMessage(attachable, 17)
        }
        verbroutine = j

        return true
}


! Use the routines ObjectAttach and ObjectDetach to attach or detach the
! attachable given by <att> to or from the object specified by <obj>; they
! will take care of setting and/or clearing the appropriate properties,
! returning true if successful.

routine ObjectAttach(att, obj)
{
        local i, point

        if obj.attachable_to and not InList(att, attachable_to, obj):  return false

        ! See first if there is an open point
        for (i=1; i<=att.#attached_to; i++)
        {
                if att.attached_to #i = 0
                {
                        point = i
                        break
                }
        }
        if not point:  return false     ! no open point

        att.attached_to #point = obj

        if object.attach_drop
        {
                move object to location
#ifset USE_CHECKHELD
                object is not checkheld_flag
#endif
        }

        return point
}

routine ObjectDetach(att, obj)
{
        local n

        n = InList(att, attached_to, obj)
        if n
                att.attached_to #n = 0

        return true
}


! ObjectisAttached returns true if something in <oldloc> is keeping
! <obj> from moving to <newloc>.

routine ObjectisAttached(obj, oldloc, newloc)
{
        local check

        if oldloc = newloc
                return false

! Check the children of the object in question:

        check = Attachable_CheckChildren(obj, newloc)
        if check:  jump SomethingisAttached

! Failing finding any attachments there, use the roundabout method of
! checking the location:

        check = Attachable_CheckAttachedTo(obj, oldloc, newloc)
        if check:  jump SomethingisAttached

        return false                    ! nothing is stopping it

:SomethingisAttached

        if check = -1                   ! something already printed
                return true

#ifclear NO_VERBS

#ifset USE_VEHICLES
        if verbroutine = &DoGo or verbroutine = &DoMoveinVehicle
#else
        if verbroutine = &DoGo
#endif
        {
                ! "Not while it is still (attached) to..."
                OMessage(attachable, 18, check)
        }
        else
        {
#endif
                ! "You can't move whatever is (attached) to it..."
                OMessage(attachable, 19, check)
#ifclear NO_VERBS
        }
#endif

        return true
}


! Attachable_CheckAttachments checks all the attachments of <att> for mobility, where
! <att> is an attachable.  Returns the number of the immobile object, or
! false if none.

routine Attachable_CheckAttachments(att, newloc)
{
        local att_to, c, i

        for (i=1; i<=att.#attached_to; i++)
        {
                att_to = att.attached_to #i

#ifset USE_VEHICLES
                if att_to.type = vehicle and not InList(newloc, vehicle_path, att_to)
                {
                        ! "You can't pull..."
                        OMessage(attachable, 20, att_to)
                        return -1
                }
#endif
                if att_to and (att_to is not mobile and not Contains(att_to, att))
                {
                        if att_to.attach_immobile
                                return -1
                        return att
                }

                if att_to.#attachable_to
                {
                        c = Attachable_CheckAttachments(att_to, newloc)
                        if c:  return c
                }

                if att_to and child(att_to) and not Contains(parent(att_to), att)  ! att_to ~= parent(att)
                {
                        c = Attachable_CheckChildren(att_to, newloc)
                        if c:  return c
                }

                if att_to.attach_immobile
                        return -1
        }
}


! Attachable_CheckChildren checks to see what (if anything) in <obj> is
! an attachable.

routine Attachable_CheckChildren(obj, newloc)
{
        local b, i

        for i in obj
        {
                if i.#attached_to
                {
                        b = Attachable_CheckAttachments(i, newloc)
                        if b:  return b
                }
                elseif child(i)
                {
                        b = Attachable_CheckChildren(i, newloc)
                        if b:  return b
                }
        }
}


! Attachable_CheckAttachedTo checks attachables in <obj>'s location.  Returns the
! number of the first attachable keeping <obj> from moving; otherwise
! returns false.

routine Attachable_CheckAttachedTo(obj, oldloc, newloc)
{
        ! att = object in oldloc we're checking
        ! att_to = object attached_to i
        local att, att_to, i

        ! indirect represents a potential obstacle indirectly
        ! attached to obj:
        local indirect = nothing

        for att in oldloc
        {
                local possible_indirect = false

                for (i=1; i<=att.#attached_to; i++)
                {
                        att_to = att.attached_to #i

                        if att_to = obj or Contains(obj, att_to):
                        {
                                possible_indirect = true

                                if att is static        ! static attachable
                                        return att
#ifset USE_VEHICLES
                                if att_to.type = vehicle and
                                        not InList(newloc, vehicle_path, att_to):
                                {
                                        ! "You can't pull..."
                                        OMessage(attachable, 20, att_to)
                                        return -1
                                }
#endif
                                if att_to is not mobile and
                                        att_to~=obj and not Contains(obj, att_to)
                                {
                                        if att_to.attach_immobile
                                                return -1
                                        return att
                                }
                        }
                }

                if possible_indirect
                {
                        for (i=1; i<=att.#attached_to; i++)
                        {
                                att_to = att.attached_to #i

                                if att_to
                                {
                                        if att is static  ! static attachable
                                        {
                                                if not indirect
                                                        indirect = att
                                        }
#ifset USE_VEHICLES
                                        if att_to.type = vehicle and
                                                not InList(newloc, vehicle_path, att_to):
                                        {
                                                if not indirect
                                                        indirect = att_to
                                        }
#endif
                                        if att_to is not mobile and
                                                att_to~=obj and not Contains(obj, att_to)
                                        {
                                                if not indirect
                                                        indirect = att
                                        }
                                }
                        }
                }

                if child(att)
                {
                        att_to = Attachable_CheckAttachedTo(obj, child(att))
                        if att_to
                                return att_to
                }
        }

        if indirect
        {
#ifset USE_VEHICLES
                if indirect.type = vehicle
                {
                        OMessage(attachable, 20, indirect)
                        return -1
                }
#endif
                if indirect.attach_immobile
                        return -1
                return indirect
        }
}


! MoveAllAttachables moves anything connected (no matter how tenuously)
! to <obj> from <oldloc> to <newloc>.  Returns the number of the first
! object (even if there is more than one) being carried along.  The <q>
! argument is optional; if true, the objects are moved quietly, i.e.
! without a message being printed.

routine MoveAllAttachables(obj, oldloc, newloc, q)
{
        local first, second

        obj is not workflag

! First, move all attachables in the specified object:

        first = Attachable_MoveChildren(obj, oldloc, newloc)

! Then, move anything else in the old location that might somehow be
! attached to it:

        second = Attachable_MoveAttachedTo(obj, oldloc, newloc)

        if not first:  first = second

        if first and not q
                ! "With it (attached) to..."
                OMessage(attachable, 21, first)
        return first
}


! Attachable_MoveChildren moves all objects attached by an attachable item in <obj>.

routine Attachable_MoveChildren(obj, oldloc, newloc)
{
        local i, k, first

        for i in obj
        {
                k = Attachable_MoveAttached(i, obj, oldloc, newloc)
                if not first:  first = k
        }
        return first
}


! Attachable_MoveAttached is called to move everything attached to the
! attachable <att> in <oldloc> to <newloc>.

routine Attachable_MoveAttached(att, obj, oldloc, newloc)
{
        local i, j, first

        if att.#attached_to
        {
                for (i=1; i<=att.#attached_to; i++)
                {
                        j = att.attached_to #i

                        if j
                        {
                                if Contains(oldloc, j) and j not in obj
                                {
                                        if obj is not workflag
                                        {
                                                first = att
                                                obj is workflag
                                        }
                                        move j to newloc
                                        j is moved
                                        Attachable_MoveChildren(j, oldloc, newloc)
                                        Attachable_MoveAttachedTo(j, oldloc, newloc)
                                }
                        }
                }
        }
        return first
}


! Attachable_MoveAttachedTo scans <oldloc> for anything attached to <obj>, moving
! any valid matches to <newloc>.  Not particularly graceful, since it must
! recurse inward for each object, since attachments are explicit only from
! attachable to object, and not vice-versa.

routine Attachable_MoveAttachedTo(obj, oldloc, newloc)
{
        local i, j, k, first

        for i in oldloc
        {
                for (j=1; j<=i.#attached_to; j++)
                {
                        if i.attached_to #j = obj or
                                Contains(obj, i.attached_to #j):
                        {
                                move i to newloc
                                i is moved
                                if not first:  first = i
                                k = Attachable_MoveAttached(i, obj, oldloc, newloc)
                                if not first:  first = k
                                k = Attachable_MoveChildren(i, obj, oldloc, newloc)
                                if not first:  first = k
                        }
                }

                if child(i)
                {
                        k = Attachable_MoveAttachedTo(obj, child(i), newloc)
                        if not first:  first = k
                }
        }
        return first
}


! ListAttachments prints everything attached to the attachable <obj>.
! Basically an adaptation of PropertyList from HUGOLIB.H.

routine ListAttachments(obj)
{
        local a, b, n, total

        for (a=1; a<=obj.#attached_to; a++)
        {
                if obj.attached_to #a:  total++
        }

        for (a=1; a<=obj.#attached_to; a++)
        {
                b = obj.attached_to #a
                if b
                {
                        The(b)
                        if Contains(player, b)
                                OMessage(attachable, 22)  ! " you're holding"
                        if a < total and total > 2
                                print ", ";
                        elseif a = total - 1:  print " ";

                        if a = total - 1
                                print "and ";
                        n++
                }
        }
        return n
}

#endif  ! ifset USE_ATTACHABLES


!----------------------------------------------------------------------------
! OBJECT LIBRARY MESSAGES
!
! OMessage and NewOMessages mirror HUGOLIB.H's Message and NewMessages,
! respectively, although not exactly.  The difference is that the first
! argument passed to OMessage (or NewOMessages) is an object or class,
! not a routine address.
!
! OMessage(object, number)
! prints message <number> for object/class <object>
!
! OMessage (object, number, var1, var2)
! where one or two variables--which may be objects or any other value--
! are used by message <number> for object/class <object>

routine OMessage(obj, num, a, b)
{
        ! Check first to see if the NewOMessages routine provides a
        ! replacement message:
        !
        if NewOMessages(obj, num, a, b):  return

        select obj

        case self_class
        {
                select num
                case 1
                {
                        print "\""; capital self.name;
                        print "\" isn't quite appropriate for ";
                        print The(object); "."
                }
        }
        case direction
        {
                select num
                case 1
                {
                        print CThe(player); " see"; MatchPlural(player);
                        "nothing special ";
                        if self.article
                                "to ";
                        print The(self);
                        if not self.article
                                print " "; player.pronoun #2;
                        print "."
                }
                case 2
                {
                        "You'll have to be a little more specific about
                        what you'd like";
                        if player_person ~= 2
                                print " "; The(player);
                        " to look ";
                        print self.name; "."
                }
        }
        case door
        {
                select num
                case 1
                {
                        print CThe(player); " will have to get ";
                        if parent(player).prep #2
                                print parent(player).prep #2; " ";
                        else
                                print "out ";
                        print "of "; The(parent(player)); " first."
                }
                case 2:  print "(opening "; The(self); " first)"
                case 3:  print CThe(self); IsorAre(self, true); " locked."
                case 4:  print CThe(self); IsorAre(self, true); " closed."
                case 5
                {
                        CThe(actor)
                        print " open"; MatchSubject(actor); " "; \
                        The(self); " and ";

                        if actor in location
                                print "leave"; MatchSubject(actor);
                        elseif location = self.between #((parent(actor) = \
                                self.between #1) + 1)
                                print "come"; MatchSubject(actor); " in";
                        print ", closing it behind ";
                        if actor is not plural or actor = player
                                print actor.pronoun #4; "."
                        else
                                print "themselves."
                }
                case 6:  print CThe(self); IsorAre(self, true); " open."
                case 7:  print CThe(self); IsorAre(self, true); " closed."
        }

        case component:  print CThe(player); " can't separate "; \
                                The(self); " from "; The(self.part_of); "."

#ifset USE_VEHICLES
        case vehicle
        {
                select num
                case 1
                {
                        print "To walk, "; The(player); " will have to
                                get "; self.prep #2; " of "; The(self); \
                                ".  Otherwise, try \""; self.vehicle_verbs; \
                                "\" ";
                        if self.#vehicle_verbs > 1
                                print "or \""; self.vehicle_verbs #2; "\" ";
                        print "and a direction."
                }
                case 2: print CThe(player); IsorAre(player); \
                                " not in anything at the moment."
                case 3: print CThe(player); IsorAre(player); " "; \
                        a.prep; " "; The(a);"--try \""; a.vehicle_verbs; "\"."
                case 4: print "You'll have to specify a direction as well."
                case 5: print CArt(obstacle); " stop"; \
                                MatchSubject(obstacle); " "; \
                                The(player, true); " from going anywhere."
                case 6
                {
                        print CThe(player); " can't "; a.vehicle_verbs; " ";
                        if object.prep
                                print object.prep;
                        else
                                print "in";
                        print " "; The(object); "."
                }
                case 7:  print CThe(player); " can't "; a.vehicle_verbs; \
                        " that way."
        }
#endif  ! ifset USE_VEHICLES

#ifset USE_PLURAL_OBJECTS
        case plural_class
        {
                select num
                case 1
                {
                        if pluralobj_heldmode = 1
                                print CThe(player); \
                                MatchPlural(player, "doesn't", "don't"); \
                                " have any ";
                        else
                                print "There are no ";
                        print self.name; " to "; VerbWord; "."
                }
                case 2
                {
                        if pluralobj_heldmode = 1
                                print CThe(player); " only"; \
                                        MatchPlural(player, "has", "have"); " ";
                        else
                        {
                                print "There ";
                                if a = 1:  print "is";
                                else:  print "are";
                                print " only ";
                        }
                        print NumberWord(a); " ";
                        if a > 1
                                print self.noun;
                        else
                                print self.single_noun;
                        print " to "; VerbWord; "."
                }
                case 3:  print CThe(player); " will have to do that one "; \
                                self.single_noun; " at a time."
        }
#endif  ! ifset USE_PLURAL_OBJECTS

#ifset USE_ATTACHABLES
        case attachable
        {
                select num
                case 1
                {
                        print CArt(self); IsorAre(self, true); " "; \
                                self.attached_desc; " ";
                        ListAttachments(self)
                        print "."
                }
                case 2:  print CThe(player); " see"; MatchSubject(player); \
                        " nothing special about "; The(self); "."
                case 3
                {
                        print CThe(self); IsorAre(self, true); " "; \
                                self.attached_desc; " ";
                        ListAttachments(self)
                        print "."
                }
                case 4:  print CThe(player); " will have to "; \
                        self.detach_verb; " before you can take it."
                case 5:  print "(having to "; self.detach_verb; " "; \
                        The(self); " "; self.detach_prep; " "; \
                        The(self.attached_to #a); " first)"
                case 6:  print CThe(player); " can't "; VerbWord; " "; \
                        The(object); "."
                case 7:  print "You might want to try \""; a; "\" with "; \
                        The(object); " instead of "; "\""; VerbWord; "\"."
                case 8:
                {
                        print "You'll have to be a little more specific
                                about what you'd like ";
                        if player_person ~= 2:  print The(player, true); " ";
                        print "to "; object.attach_verb; " "; \
                        The(object); " "; object.attach_prep; "."
                }
                case 9:  print "(taking "; The(object); " first)"
                case 10:  print CThe(player); " can't "; VerbWord; " "; \
                        The(object); " "; object.attach_prep; " "; \
                        The(xobject); "."
                case 11:  print "Except that "; The(object); \
                        IsorAre(object, true); " already "; \
                        object.attached_desc; " "; The(xobject); "."
                case 12
                {
                        print "Except that "; The(object); " is already "; \
                        object.attached_desc; " ";
                        ListAttachments(object)
                        print "."
                }
                case 13:  print CThe(player); " "; object.attach_verb; \
                        MatchSubject(player); " "; The(object); " "; \
                        object.attach_prep; " "; The(xobject); "."
                case 14:  print "Except that "; The(object); \
                        IsorAre(object, true); " not "; \
                        object.attached_desc; " "; The(xobject); "."
                case 15:  print "Except that "; The(object); \
                        IsorAre(object, true); " not "; \
                        object.attached_desc; " anything."
                case 16:
                {
                        print "You'll have to be a little more specific
                                about what you'd like ";
                        if player_person ~= 2:  print The(player, true); " ";
                        print "to "; VerbWord; " "; The(object); " "; \
                                object.detach_prep; "."
                }
                case 17:  print CThe(player); " "; object.detach_verb; \
                        MatchSubject(player); " "; The(object); " "; \
                        object.detach_prep; " "; The(xobject); "."
                case 18
                {
                        print CThe(player); " can't go anywhere while "; \
                                The(a); IsorAre(a, true); " still "; \
                                a.attached_desc; " ";
                        ListAttachments(a)
                        print "."
                }
                case 19
                {
                        print CThe(player); " can't move whatever is "; \
                        a.attached_desc; " "; The(a); ", namely ";
                        ListAttachments(a)
                        print "."
                }
                case 20:  print CThe(player); " can't pull "; The(a); \
                        " that way."
                case 21
                {
                        print "(with "; The(a); " "; a.attached_desc; " ";
                        ListAttachments(a)
                        print ")"
                }
                case 22:  print " "; The(player); IsorAre(player); " holding";
        }
#endif  ! ifset USE_ATTACHABLES
}

routine NewOMessages(obj, num, a, b)    ! The NewOMessages routine may be
{                                       ! replaced, and should return true
        return false                    ! if a replacement message <num>
}                                       ! exists for object/class <obj>

#endif  ! _COMPILING_HUGOLIB

#endif  ! _OBJLIB_H
