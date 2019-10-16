!----------------------------------------------------------------------------
!
!        Hugo System Library v3.1.03.2 by Kent Tessman (c) 1995-2006
!                        for use with the Hugo Compiler
!
!----------------------------------------------------------------------------

#ifclear _SYSTEM_H
#set _SYSTEM_H

! Constant definitions for use with the system() function, as in:
!
!       system(<function>)

! For system_status:
#if undefined STAT_NOERROR
constant STAT_NOERROR             0
constant STAT_UNAVAILABLE        -1
constant STAT_NOFILE            101
constant STAT_NORESOURCE        102
constant STAT_LOADERROR         103
#endif

constant READ_KEY               11
constant NORMALIZE_RANDOM       21
constant INIT_RANDOM            22
constant PAUSE_SECOND           31
constant PAUSE_100TH_SECOND     32
constant GAME_RESET             41
constant SYSTEM_TIME            51
constant MINIMAL_INTERFACE      61


!----------------------------------------------------------------------------
! GetSystemTimeValue(TIME_CONSTANT)
!
! returns a numerical value for the specified aspect of the current system
! time, or -1 for failure.
!----------------------------------------------------------------------------
array _time_string[20]

enumerate
{
        TIME_YEAR = 1,
        TIME_MONTH,
        TIME_DAY,
        TIME_HOURS,
        TIME_MINUTES,
        TIME_SECONDS
}

routine GetSystemTimeValue(which)
{
        local pos, factor = 10, result

        system(SYSTEM_TIME)
        if system_status = STAT_UNAVAILABLE
                return -1

        string(_time_string, parse$, 20)

        select which
        case TIME_YEAR
        {
                pos = 0
                factor = 1000
        }
        case TIME_MONTH
                pos = 5
        case TIME_DAY
                pos = 8
        case TIME_HOURS
                pos = 11
        case TIME_MINUTES
                pos = 14
        case TIME_SECONDS
                pos = 17
        case else
                return -1

        while factor
        {
                result += ((_time_string[pos]-'0')*factor)
                pos++
                if factor = 1
                        break
                factor /= 10
        }

        return result
}


#endif  ! _SYSTEM_H
