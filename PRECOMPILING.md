> **EDITOR NOTE** — This document is the markdown version of the original [`hugolib.txt`][hugolib.txt] file distributed with `hugolib.hug` at the [IF Archive].
>
> While this document was included for the sake of completeness and historical preservation, the benefits of resorting to precompiled headers are mostly irrelevant on modern computers.
> Quoting _The Hugo Book_, [Appendix E: Precompiled Headers][AppE]:
>
> > [...] precompiled headers ... becomes less and less vital as computer (and therefore compilation) speeds increase.
> > As of this writing, on a relatively fast computer, a game that takes 6 seconds to compile will compile in 4 using a precompiled version of the library.
> > A game that takes 2 seconds to compile normally will compile in 1.
> > (In other words, the savings are somewhat negligible.)
>
> The above passage was written around 2004, so probably today compilation differences are in the order of milliseconds.
> For more information on precompiled headers, refer to the above mentioned appendix.

# A Note on Precompiling the Hugo Library

Generating a precompiled version of [`hugolib.h`][hugolib.h] can greatly speed up game development time by saving the compiler having to recompile the Hugo Library each time the game source is compiled.

In order to do this, two steps are necessary:

1. Compile the short file [`hugolib.hug`][hugolib.hug] as `hugolib.hlb` via:

        hc -h hugolib.hug

    Looking at the Hugo source for [`hugolib.hug`][hugolib.hug] will reveal that it is basically a couple of `#set`tings — which notify the compiler to include the files [`hugofix.h`][hugofix.h] (the HugoFix debugging library) and [`verbstub.h`][verbstub.h] (the set of additional action verb routines) — the #inclusion of [`hugolib.h`][hugolib.h], and an empty `main` routine.
    The `-h` switch forces creation of an __.HLB__ precompiled file instead of the normal __.HEX__ Hugo executable.

    If it is desirable to have a version of `hugolib.hlb` that does not include the code for the debugging library or the verb stub routines, it is necessary to remove the `#set`ting(s) in question from [`hugolib.hug`][hugolib.hug], then recompile.

2.  Change the line

        #include "hugolib.h"

    in the Hugo game source to

        #link "hugolib.hlb"

    and the definition for the `main` routine from

        routine main
        {...

    to

        replace main
        {...

    (since `hugolib.hlb` already has a `main` routine defined in it).


That's it.
Now whenever the game source is compiled, the Hugo compiler will quickly and efficiently link the precompiled library.

> **NOTE:**  It is probably desirable to compile a release version of a game using the original library files instead of a precompiled header.
The reason for this is that a game compiled with the precompiled header may tend to be larger due to the fact that it contains unnecessary code — i.e. the code for routines that are `REPLACE`d in the game source, or routines that would otherwise have been excluded due to the setting or clearing of compiler flags such as `DEBUG` and `VERBSTUBS`, or `NO_AUX_MATH` and `NO_SCRIPTS`.

<!-----------------------------------------------------------------------------
                               REFERENCE LINKS
------------------------------------------------------------------------------>

[hugofix.h]: ./hugolib/hugofix.h "View source file"
[hugolib.h]: ./hugolib/hugolib.h "View source file"
[hugolib.hug]: ./hugolib.hug "View source file"
[verbstub.h]: ./hugolib/verbstub.h "View source file"

[AppE]: https://tajmone.github.io/hugo-book/#appendix_e "Live HTML link to Appendix E. of The Hugo Book"

<!-- IF Archive -->

[hugolib.txt]: https://www.ifarchive.org/if-archive/programming/hugo/examples/hugolib.txt "View original 'hugolib.tx' file on the IF Archive"
[IF Archive]: https://www.ifarchive.org/indexes/if-archive/programming/hugo/examples/ "Visit the Hugo examples folder on the IF Archive"

<!-- EOF -->
