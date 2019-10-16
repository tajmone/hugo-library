!----------------------------------------------------------------------------
!
!      HugoFix Debugging Library v3.1.03.2 by Kent Tessman (c) 1995-2006
!                      for use with the Hugo Compiler
!
!----------------------------------------------------------------------------
!
!    NOTE:  These routines and the corresponding grammar table are
!           automatically included by HUGOLIB.H and GRAMMAR.G if the
!           DEBUG compiler flag is set.
!
!----------------------------------------------------------------------------

#ifclear _HUGOFIX_H
#set _HUGOFIX_H

#ifclear _COMPILING_HUGOLIB
#message warning "HUGOFIX.H not #included; #set DEBUG instead"
#else

! BITMASKS FOR debug_flags:

global debug_flags

enumerate step *2
{
        D_FUSES = 1             ! fuse/daemon monitoring
        D_OBJNUM                ! object numbers
        D_PARSE                 ! parse monitoring
        D_SCRIPTS               ! script monitoring
        D_FINDOBJECT            ! FindObject monitoring
        D_PARSE_RANK            ! parse_rank monitoring
}

routine DoHugoFix
{
        local i, n

        Font(PROP_OFF)          ! proportional printing off

        if ((VerbWord ~= "$wo", "$wn") and object > objects and object > 0) or
                ((VerbWord ~= "$ac", "$at") and xobject > objects)
        {
                "[Object number(s) out of range]"
                Font(DEFAULT_FONT)
                return
        }

        select VerbWord
        case "$", "$?"
        {
                "HUGOFIX DEBUGGING COMMANDS:\n\
                \_    $?  - Display this help menu\n"

                "Monitoring:\n\
                \_    $on - Toggle object numbers"

#ifclear NO_SCRIPTS
                "\_    $sc - Script monitor on/off"
#endif

#ifclear NO_FUSES
                "\_    $fd - Fuse/daemon monitor on/off"
#endif

                "\_    $fi - FindObject monitoring on/off"
                "\_    $pm - Parser monitoring on/off\n"


                "Object manipulation:\n\
                \_    $at <obj.> is [not] <attr. #>\n\
                \_                         - set/clear object attribute\n\
                \_    $mo <obj.> to <obj.> - Move object to new parent\n
                \_    $mp <obj.>           - Move player object to new
                        parent\n"

                "Object testing:\n\
                \_    $fo [obj.]   - Find object (or player, if no obj.)\n\
                \_    $na <obj. #> - Print name of object number <obj.>\n\
                \_    $nu <obj.>   - Print number of object <obj.>\n"

                "Parser testing:\n\
                \_    $ca                - Check articles for all objects\n\
                \_    $pc [$all] [obj.]  - Check parser conflicts (for <obj.>)\n\
                \_                         (parse_ranked objects only or all)\n\
                \_    $pr                - parse_rank monitoring\n"

                "Other utilities:"

#ifclear NO_FUSES
                "\_    $ac <obj.> [timer] - Activate fuse (with timer) or
                        daemon\n\
                \_    $de <obj.>         - Deactivate fuse or daemon"
#endif
#ifclear NO_OBJLIB
                "\_    $di [obj.]         - Audit directions (for room <obj.>)"
#endif
                "\_    $kn                - Set all objects to 'known'\n\
                \_    $kn <obj. #>       - Make <obj.> 'known'\n\
                \_    $nr                - Normalize random numbers\n\
                \_    $ot [obj. | $loc]  - Print object tree (beginning with
                        <obj.>)\n\
                \_    $rr                - Restore \"random\" random numbers\n\
                \_    $uk <obj.>         - Make object unknown\n\
                \_    $wo <val.>         - Print dictionary word entry <val.>\n\
                \_    $wn <word>         - Value/addr. of (lowercase) <word>\n\
                \n\_    $au                - Run HugoFixAudit"
        }


#ifclear NO_FUSES
        case "$ac"
        {
                if object.type ~= fuse and object.type ~= daemon
                        "Not a fuse or daemon."
                else
                {
                        Activate(object)
                        object.timer = xobject
                        "Activated."
                }
        }
        case "$de"
        {
                if object.type ~= fuse and object.type ~= daemon
                        "Not a fuse or daemon."
                else
                {
                        Deactivate(object)
                        "Deactivated."
                }
        }
#endif

        case "$at"
        {
                local isnot

                for (i=1; i<=words; i++)
                {
                        if word[i] = "": break
                        if word[i] = "not": isnot = true
                }

                print "[Setting "; object.name; " is ";

                n = xobject

                if isnot
                {
                        object is not n
                        "not ";
                }
                else
                        object is n

                print "attribute "; number n; "]"
        }
        case "$au"
        {
                HugoFixAudit
        }
        case "$ca"
        {
                "[Checking articles]"
                for (i=last_library_object+1; i<objects; i++)
                {
                        if (i.noun or i.adjective) and not i.article
                        {
                                print "Object \""; i.name; "\" ("; number i; ") has no article"
                        }
                }
                "[...Done]"
        }

#ifclear NO_OBJLIB
        case "$di"
        {
                local place

                if object:  place = object
                else:  place = location

                print "DIRECTIONS FROM:  "; capital place.name; " "; "("; \
                        number place; ")"
                local printed
                for i in direction
                {
                        if &place.(i.dir_to)
                        {
                                print "\_ "; i.name; ":  (Property routine)"
                                printed++
                        }
                        else
                        {
                                n = place.(i.dir_to)
                                if n > 1
                                {
                                        print "\_ "; i.name; ":  "; \
                                                capital n.name
                                        printed++
                                }
                                elseif n = 0
                                {
                                        if &place.cant_go
                                        {
                                                print "\_ "; i.name; ":  ";
                                                if not place.cant_go
                                                        print newline
                                                printed++
                                        }
                                }
                        }
                }
                if not printed:  "\_ (None)"
        }
#endif

#ifclear NO_FUSES
        case "$fd"
        {
                "[Fuse/daemon monitor";
                OnorOff(D_FUSES)
        }
#endif

        case "$fi"
        {
                "[FindObject monitoring";
                OnorOff(D_FINDOBJECT)
        }
        case "$fo"
        {
                if not object:  object = player
                "[Parent of obj. "; : print number object; " ("; \
                object.name; "):  Obj. ";
                print number (parent(object)); " ("; parent(object).name; ")]"
        }
        case "$kn"
        {
                if object
                {
                        object is known
                        print "["; object.name; " now 'known']"
                }
                else
                {
#ifset NO_OBJLIB
                        i = 0
#else
                        i = out_obj + 1
#endif
                        for (; i<objects; i++)
                                i is known
                        "[All objects now 'known']"
                }
        }
        case "$mo"
        {
                if word[words] = "$loc":  xobject = location
                print "[Obj. "; number object; " ("; object.name; ") ";
                print "moved to obj. "; number xobject; " (";
                print xobject.name; ")]"
                move object to xobject
                object is not hidden
                object is known
        }
        case "$mp"
        {
                if parent(object) ~= 0 and object is not enterable
                {
                        print "[Obj. "; number object; " (";
                        print object.name; ") is not a room object]"
                }
                else
                {
                        MovePlayer(object, false, true)
                        Font(PROP_OFF)
                        "[Player moved to obj. ";
                        print number object; " ("; object.name; ")]"
                }
        }
        case "$na", "$nu"
        {
                print "[Obj. "; number object; ": \""; object.name; "\"]"
        }
        case "$nr"
        {
                system(21)
                "[Random numbers now normalized]"
        }
        case "$on"
        {
                "[Object numbers";
                OnorOff(D_OBJNUM)
        }
        case "$ot"
        {
                if object = 0 and words = 1
                {
                        "[Specify a parent object to draw the tree from, or
                        '$ot 0' to draw the entire object tree.]"
                        Font(DEFAULT_FONT)
                        return
                }

                if word[2] = "$loc":  object = location
                print "["; number object; "] "; object.name
                list_nest = (object ~= 0)
                for (i=0; i<objects; i=i+1)
                {
                        if i ~= object and parent(i) = object
                                DrawBranch(i)
                }
        }
        case "$pc"
        {
                local parse_rank_only = true

                if word[2] = "$all"
                        parse_rank_only = false

                if object
                {
                        if not ListParserConflicts(object, parse_rank_only)
                        {
                                "No parser conflicts for:  ";
                                PrintParserInfoForObject(object)
                        }
                }
                else
                {
                        "NOTE:  You have chosen to check parser conflicts for the
                        entire tree";
                        if not parse_rank_only
                                " for ALL objects";
                        ".  This may take a long time depending on the number of
                        objects/adjectives/nouns.  Do you wish to continue (YES OR NO)? ";
                        input
                        if not YesOrNo
                        {
                                Font(DEFAULT_FONT)
                                return
                        }
                        "[Checking parser conflicts...]"

                        local count
                        for (i=0; i<objects; i++)
                        {
                                if not parse_rank_only or i.parse_rank
                                {
                                        if ListParserConflicts(i, parse_rank_only)
                                                count++
                                }
                        }
                        if not count
                                "No parser conflicts found."
                }
        }
        case "$pm"
        {
                "[Parser monitoring";
                OnorOff(D_PARSE)
        }
        case "$pr"
        {
                "[parse_rank monitoring";
                OnorOff(D_PARSE_RANK)
        }
        case "$rr"
        {
                system(22)
                "[\"Random\" random numbers restored]"
        }

#ifclear NO_SCRIPTS
        case "$sc"
        {
                "[Script monitor";
                OnorOff(D_SCRIPTS)
        }
#endif

        case "$uk"
        {
                object is not known
                print "["; object.name; " now not 'known']"
        }
        case "$wn"
        {
                print "[\""; word[2]; "\": "; number word[2]; "]"
        }
        case "$wo"
        {
                print "[\""; object; "\": "; number object; "]"
        }

        Font(DEFAULT_FONT)      ! restore font (i.e., proportional printing
                                ! if it was turned off)
}


routine OnorOff(a)
{
        if not (debug_flags & a)
        {
                print " on.]"
                debug_flags = debug_flags | a
        }
        else
        {
                print " off.]"
                debug_flags = debug_flags & ~a
        }
}


!----------------------------------------------------------------------------
! Object-tree drawing
!----------------------------------------------------------------------------

routine DrawBranch(obj)
{
        local i, nextobj

        for (i=1; i<=list_nest; i++)
                print ". . ";
        print "["; number obj; "] "; obj.name

        for nextobj in obj
        {
                list_nest++
                DrawBranch(nextobj)
                list_nest--
        }
}


!----------------------------------------------------------------------------
! Parser conflict checking
!----------------------------------------------------------------------------

routine CheckParserConflicts(obj1, obj2, what)
{
        local n

        for (n=1; n<obj1.#what; n++)
        {
                if obj1.what #n and InList(obj2, what, obj1.what #n)
                {
                        if obj1 is not workflag
                        {
                                "\n*** Conflicts with: ";
                                PrintParserInfoForObject(obj1)
                        }
                        "\_ ";
                        PrintParserInfoForObject(obj2, obj1)
                        obj1 is workflag
                        return true
                }
        }
}


routine ListParserConflicts(obj, parse_rank_only)
{
        local i, matched

        obj is not workflag

        for (i=0; i<objects; i++)
        {
                local check     ! check this object

                if parse_rank_only
                {
                        if obj.parse_rank or i.parse_rank
                                check = true
                }
                else
                {
                        check = true
                }

                if i ~= obj and check
                {
                        if CheckParserConflicts(obj, i, adjectives)
                        {
                                matched = true
                        }
                        else
                        {
                                if CheckParserConflicts(obj, i, nouns)
                                        matched = true
                        }
                }
        }

        return matched
}

routine PrintParserInfoForObject(obj, compare_to)
{
        local i

        print obj.name; " ("; number obj; "):";
        if obj.#adjectives
        {
                " adjectives: ";
                for (i=1; i<obj.#adjectives; i++)
                {
                        print "\""; obj.adjective #i; "\", ";
                }
                print "\""; obj.adjective #i; "\"";

                if obj.#nouns or obj.parse_rank
                        print " /";
        }
        if obj.#nouns
        {
                " nouns: ";
                for (i=1; i<obj.#nouns; i++)
                {
                        print "\""; obj.noun #i; "\", ";
                }
                print "\""; obj.noun #i; "\"";

                print " /";
        }

        print " parse_rank = "; number obj.parse_rank;
        if not obj.#parse_rank
                " (none)";
        if compare_to
        {
                if obj.parse_rank > compare_to.parse_rank
                        " (higher)";
                elseif obj.parse_rank < compare_to.parse_rank
                        " (lower)";
        }
        print newline
}


!----------------------------------------------------------------------------
! HugoFixAudit
!
! To check for potential problem areas that might not be easily debuggable
!----------------------------------------------------------------------------

routine HugoFixAudit
{
        local obj, err, count

        Font(PROP_OFF)

        "Validating library object setup..."

        ! Make sure "nothing" is object 0
        if nothing ~= 0
        {
                "\"nothing\" object is not object 0"
                err++
        }

#ifset USE_PLURAL_OBJECTS
        ! Check for proper plural/identical objects initialization
        local i

        "Validating plural/identical objects..."
        count = 0

        for (obj=1; obj<objects; obj++)
        {
                if (obj.type = plural_class, identical_class) and
                        (obj ~= plural_class, identical_class)
                {
                        count++

                        if not obj.plural_of
                        {
                                err++
                                "Plural/identical class ";
                                print "\""; obj.name; "\" ("; number obj; ")";
                                " has no 'plural_of' property"
                        }
                        elseif obj.type = identical_class
                        {
                                for (i=1; i<=obj.#plural_of; i++)
                                {
                                        if obj.plural_of #i.identical_to ~= obj
                                        {
                                                err++
                                                "Identical object ";
                                                print "\""; obj.plural_of #i.name; "\" ";
                                                print "("; number obj.plural_of #i; ") ";
                                                "should have 'identical_class' ";
                                                print "\""; obj.name; "\" ";
                                                print "("; number obj; ") ";
                                                "in 'identical_to' property"
                                        }
                                }
                        }
                }
        }

        print number count;
        " plural/identical object(s) checked"

#endif  ! ifset USE_PLURAL_OBJECTS

        "Validating containers and platforms..."
        count = 0

        for (obj=1; obj<objects; obj++)
        {
                if obj is container or obj is platform
                {
                        count++

                        if obj.capacity and not obj.#holding and not &obj.holding
                        {
                                if obj is container
                                        "Container ";
                                else
                                        "Platform ";
                                print "\""; obj.name; "\" ("; number obj; ")";
                                " has 'capacity' but no 'holding' property"
                                err++
                        }
                        if obj.holding and not obj.#capacity and not &obj.capacity
                        {
                                if obj is container
                                        "Container ";
                                else
                                        "Platform ";
                                print "\""; obj.name; "\" ("; number obj; ")";
                                " has 'holding' but no 'capacity' property"
                                err++
                        }
                }
        }

        print number count;
        " container/platform object(s) checked"

        print "[";
        if err
                print number err;
        else
                print "No";
        " error(s)/warning(s) found by HugoFixAudit]"
        Font(DEFAULT_FONT)
}


!----------------------------------------------------------------------------
! Replacements for The, CThe, Art, and CArt:
!----------------------------------------------------------------------------

replace The(obj, a)
{
        AssignPronoun(obj)

        if obj = player and player_person = 1 and a
        {
                print player.pronoun #2;
                jump PrintTheObjNumber
        }

        if obj.article
                print "the ";
        print obj.name;

:PrintTheObjNumber
        if (debug_flags & D_OBJNUM)
                print " ["; number obj; "]";
}

replace CThe(obj)
{
        AssignPronoun(obj)

        if obj.article
                print "The "; obj.name;
        else
                print capital obj.name;
        if (debug_flags & D_OBJNUM)
                print " ["; number obj; "]";
}

replace Art(obj, a)
{
        AssignPronoun(obj)

        if obj = player and player_person = 1 and a
        {
                print player.pronoun #2;
                jump PrintArtObjNumber
        }

        if obj.article
                print obj.article; " ";
        print obj.name;

:PrintArtObjNumber
        if (debug_flags & D_OBJNUM)
                print " ["; number obj; "]";
}

replace CArt(obj)
{
        AssignPronoun(obj)

        if obj.article
                print capital obj.article; " "; obj.name;
        else
                print capital obj.name;
        if (debug_flags & D_OBJNUM)
                print " ["; number obj; "]";
}

#endif  ! _COMPILING_HUGOLIB

#endif  ! _HUGOFIX_H
