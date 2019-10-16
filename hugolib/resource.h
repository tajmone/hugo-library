!----------------------------------------------------------------------------
!
!       Hugo Resource Library v3.1.03.2 by Kent Tessman (c) 1995-2006
!                     for use with the Hugo Compiler
!
!       Including:
!               LoadPicture
!               PictureinText
!               PlayMusic
!               PlaySound
!
!----------------------------------------------------------------------------

#ifclear _RESOURCE_H
#set _RESOURCE_H

! For system_status:
#if undefined STAT_NOERROR
constant STAT_NOERROR             0
constant STAT_UNAVAILABLE        -1
constant STAT_NOFILE            101
constant STAT_NORESOURCE        102
constant STAT_LOADERROR         103
#endif

global last_resourcefile

!----------------------------------------------------------------------------
! LoadPicture(resourcefile, picturename)
! LoadPicture(picturename)
!
! Loads "picturename" from the specified resourcefile and displays it in
! the currently defined text window.  ("resourcefile" is recommended but
! optional--if not given, the library assumes the last-used resourcefile
! name.)  The aspect ratio of the picture is maintained, but it may be
! either centered or shrunk to fit the window.

routine LoadPicture(resfile, pic)
{
        if not display.hasgraphics:  return false

        if pic = ""
        {
                pic = resfile
                resfile = last_resourcefile
        }

        picture resfile, pic

        last_resourcefile = resfile

        if system_status:  return false

        return true
}


!----------------------------------------------------------------------------
! PictureinText(resourcefile, picturename, width, height[, preserve_lines])
!
! Loads "picturename" from the specified resourcefile and displays it as part
! of the regular text flow in the main bottom window of the Hugo screen.
! <preserve_lines>, if given, represents the number of lines to keep (i.e.,
! not scroll) at the top of the screen.
!
! It's generally a good idea to pass the width and height as calculations
! based on display.screenwidth, display.screenheight, etc., since this will
! ensure greater compatibility across different systems and screen modes/
! sizes.
!
! Note:  PictureInText is designed to work where the text is scrolling
! (i.e., being printed at the bottom of the window).  For other purposes,
! such as simply displaying a picture in mid-screen, simply use a window
! and LoadPicture.

routine PictureinText(resfile, pic, width, height, preserve)
{
        local i

        if not display.hasgraphics:  return false

        Font(PROP_OFF)

        ! Print blank lines the height of the picture, but only if images
        ! aren't being routed to an external destination
        !
        if (display.hasgraphics ~= 2)
        {
                for (i=1; i<=height; i++)
                        print ""
        }

        if width = 0:  width = 1
        if height = 0: height = 1

        ! Create a properly sized window at the bottom-left corner of the
        ! screen to load the picture in
        !
        window 1, (display.screenheight-height), width,
                (display.screenheight)
        {
                LoadPicture(resfile, pic)
        }

        ! If any number of lines was given to preserve, temporarily define
        ! a window of that height in order to make it the lowest-defined
        ! window--i.e., the top of the main scrolling region
        !
        if preserve
        {
                window 0
                window preserve
                {}
        }

        print ""
        Font(DEFAULT_FONT)

        if system_status:  return false

        return true
}


!----------------------------------------------------------------------------
! The audio object, similar to the display object in that it can be used to
! get information about the current song/sound playing:

object audio
{
        current_music           ""
        current_music_resfile   ""
        current_music_looping   false
        current_music_volume    0

        current_sound           ""
        current_sound_resfile   ""
        current_sound_looping   false
        current_sound_volume    0
}


!----------------------------------------------------------------------------
! PlayMusic(resourcefile, songname, volume[, loop[, force]])
! PlayMusic(MUSIC_STOP)
!
! Loads music module "songname" from the specified resourcefile and begins
! playing it.  If <force> is used, the song is restarted even if it is
! already playing.
!
! Note:  PlayMusic(MUSIC_STOP) cannot be expected not to stop all audio
! resources currently playing.
!
! IMPORTANT:  Passing 0 for the volume is the same as no volume parameter
! at all (i.e., regular/full volume).  For silent playback, pass volume < 0.

CONSTANT MUSIC_STOP 0
#ifclear _HUGOLIB_H
property current_music
property current_music_resfile
property current_music_looping
property current_music_volume
#else
property current_music alias n_to
property current_music_resfile alias ne_to
property current_music_looping alias e_to
property current_music_volume alias se_to
#endif

routine PlayMusic(resfile, song, volume, loop, force)
{
        ! If the given song is already playing:
        if audio.current_music_resfile = resfile and
                audio.current_music = song
        {
                if not force
                        return
                else
                        music 0
        }

        local vol
        vol = volume
        if vol = 0
                vol = 100
        elseif vol < 0
                vol = 0

        if resfile = MUSIC_STOP and not song
        {
                music 0
                audio.current_music = 0
        }
        else
        {
                if not loop
                        music resfile, song, vol
                else
                        music repeat resfile, song, vol
                audio.current_music = song
                audio.current_music_looping = loop
        }

        audio.current_music_resfile = resfile
        last_resourcefile = resfile
        audio.current_music_volume = volume

        if system_status:  return false

        return true
}


!----------------------------------------------------------------------------
! PlaySound(resourcefile, soundname, volume[, loop [, force]])
! PlaySound(SOUND_STOP)
!
! Loads sample "soundname" from the specified resourcefile and begins
! playing it.  If <force> is true, the sound is restarted even if it is
! already playing.
!
! IMPORTANT:  Passing 0 for the volume is the same as no volume parameter
! at all (i.e., regular/full volume).  For silent playback, pass volume < 0.

constant SOUND_STOP 0
#ifclear _HUGOLIB_H
property current_sound
property current_sound_resfile
property current_sound_looping
property current_sound_volume
#else
property current_sound alias s_to
property current_sound_resfile alias sw_to
property current_sound_looping alias w_to
property current_sound_volume alias nw_to
#endif

routine PlaySound(resfile, s, volume, loop, force)
{
        ! Due to the nature of samples, always restart the sound
        force = true

        ! If the given sound is already playing:
        if audio.current_sound_resfile = resfile and
                audio.current_sound = s
        {
                if not force
                        return
                else
                        sound 0
        }

        local vol
        vol = volume
        if vol = 0
                vol = 100
        elseif vol < 0
                vol = 0

        if resfile = SOUND_STOP and not s
        {
                sound 0
                audio.current_sound = 0
        }
        else
        {
                if not loop
                        sound resfile, s, vol
                else
                        sound repeat resfile, s, vol
                audio.current_sound = s
                audio.current_sound_looping = loop
        }

        audio.current_sound_resfile = resfile
        last_resourcefile = resfile
        audio.current_sound_volume = volume

        if system_status:  return false

        return true
}

#endif  ! _RESOURCE_H
