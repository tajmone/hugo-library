!----------------------------------------------------------------------------
!
!     Hugo Window Class Library v3.1.03.2 by Kent Tessman (c) 1995-2006
!                     for use with the Hugo Compiler
!
!----------------------------------------------------------------------------

#ifclear _WINDOW_H
#set _WINDOW_H

!\
The window_class is relatively easy to use.  It does require that HUGOLIB.H
be loaded first (or linked as HUGOLIB.HLB).  Then, define a window as:

        window_class <window name> "title"
        {
                win_left        <leftmost column>
                win_top         <topmost row>
                win_right       <rightmost column>
                win_bottom      <bottommost row>

                win_textcolor   <text color>
                win_backcolor   <background color>
                win_titlecolor  <title text>
                win_titleback   <title background>
        }

Then, call the window using something along the lines of:

        run <window name>.win_init              ! Initialize
        window                                  ! Re-open the last-defined
        {                                       !   window (i.e., this one)
                ...print some text...

                pause                           ! Wait for a keypress
                run <window name>.win_clear     ! Clear the window

                ...print some more text...

        }
        run <window name>.win_end               ! Clean up

Note that the top of the main scrolling window (i.e., when not in any
"window" block) is always reset to the bottom of the lowest-positioned
window.

Note also that if a window should be located a particular position on
the screen regardless of the screen dimensions/size, it is necessary
to make the win_left, win_top, etc. properties property routines that
return a calculation based on display.screenwidth and/or
display.screenheight, in the event that screen metrics are dynamically
altered by the engine at run time.
\!

property win_title      alias name
property win_init       alias n_to
property win_end        alias ne_to
property win_clear      alias e_to
property win_left       alias se_to
property win_top        alias s_to
property win_right      alias sw_to
property win_bottom     alias w_to
property win_textcolor  alias nw_to
property win_backcolor  alias u_to
property win_titlecolor alias d_to
property win_titleback  alias in_to
property win_active     alias out_to

array _win_title_string[65]             ! maximum string length is 64 char.

class window_class
{
        win_title ""

        win_left        0               ! default coordinates
        win_top         0
        win_right       0
        win_bottom      0

        win_textcolor
                return TEXTCOLOR
        win_backcolor
                return BGCOLOR
        win_titlecolor
                return SL_TEXTCOLOR
        win_titleback
                return SL_BGCOLOR

        win_active false

        win_init
        {
                local x, y, titlelen, width, height

                titlelen = string(_win_title_string, self.win_title, 64)

                height = self.win_bottom - self.win_top
                if height++ = 0:  height = 10
                if self.win_title:  height++

                width =  self.win_right - self.win_left
                if width++ = 0:  width = 20
                if titlelen > width
                        width = titlelen+2

                ! The default position is centered in the middle of
                ! the screen
                !
                x = self.win_left
                if x = 0:  x = display.screenwidth/2 - width/2 + 1
                y = self.win_top
                if y = 0:  y = display.screenheight/2 - height/2 + 1

                ! Check to make sure the window will actually fit on the
                ! screen--and if it doesn't, make it
                !
                if x + width > display.screenwidth + 1
                {
                        if width > display.screenwidth
                                width = display.screenwidth
                        x = display.screenwidth - width + 1
                }
                if y + height > display.screenheight + 1
                {
                        if height > display.screenheight
                                height = display.screenheight
                        y = display.screenheight - height + 1
                }

                ! Draw the window, in the supplied color(s), with or
                ! without a static title:
                !
                window x, y, (x+width-1), (y+height-1)
                {
                        color self.win_textcolor, self.win_backcolor
                        cls

                        if self.win_title ~= ""
                        {
                                locate 1, 1
                                color self.win_titlecolor,
                                        self.win_titleback

                                print to ((width-titlelen)/2); \
                                        self.win_title; to width
                        }

                }

                ! If necessary, redefine the window below the static
                ! title:
                !
                if self.win_title ~= ""
                {
                        window x, (y+1), (x+width-1), (y+height-1)
                        {}                      ! empty window body
                }

                color self.win_textcolor, self.win_backcolor
                self.win_active = true
        }

        win_clear
        {
                if self.win_active
                {
                        color self.win_textcolor, self.win_backcolor
                        cls
                }
        }

        win_end
        {
                color TEXTCOLOR, BGCOLOR
                self.win_active = false
        }
}

#endif  ! _WINDOW_H
