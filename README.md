[![BSD 2-Clause License][license badge]][BSD 2-Clause License]&nbsp;
![build status][travis badge]

# The Hugo Library

Hugo Library v3.1.03.2, by [Kent Tessman] © 1995-2006, [BSD 2-Clause License].

Repository created by [Tristano Ajmone], 2019/10/18.

-----

**Table of Contents**

<!-- MarkdownTOC autolink="true" bracket="round" autoanchor="false" lowercase="only_ascii" uri_encoding="true" levels="1,2,3" -->

- [About](#about)
- [Comparison with the IF Archive Version](#comparison-with-the-if-archive-version)
- [License](#license)
- [Links](#links)

<!-- /MarkdownTOC -->

-----

# About

This repository contains the Hugo Library v3.1.03.2. taken from the official [Hugo SVN repository], plus the `sample.hug` and `shell.hug` files from the [Hugo website].

- [`/hugolib/`](./hugolib/) — Hugo Library v3.1.03.2.
- [`sample.hug`](./sample.hug) — demo adventure for the library.
- [`shell.hug`](./shell.hug) — boilerplate for creating adventures with the library.
- [`LICENSE`](./LICENSE) — BSD 2-Clause License.

I've converted all tabs to spaces to improve readability across various editors and preserve the original intended alignment (1 tab = 8 spaces).

In order to enforce code style consistency via [EditorConfig], I had to add or remove a single space in a few lines, in order to make all indentations multiples of eight.
Now the whole repository passes the [EditorConfig] validation via the [ECLint] tool.

# Comparison with the IF Archive Version

This is a slightly updated library compared to the [version currently available on the IF Archive]  (v3.1.03.1).

The main difference consists in a bug fix for the `DoGet` response in `verblib.h` (LLs. 2137–2138) regarding platforms; where `"in"` and `"on"` were swapped:

* `verblib.h` v3.1.03.1:

    ```hugo
    elseif object is platform:  print "in";
    else:  print "on";
    ```

* `verblib.h` v3.1.03.2:

    ```hugo
    elseif object is platform:  print "on";
    else:  print "in";
    ```

The other two minor changes affect `hugolib.h`, where the letter casing of the `TheorThat` routine (LL. 1195) was fixed to `TheOrThat`, and an explanatory comment was added at LL. 2467.


# License

- [`LICENSE`][LICENSE]

The Hugo Library is part of the Hugo project, which is released under the terms of the BSD 2-Clause License (see [original license file on Hugo website]).

```
BSD 2-Clause License

Copyright (c) 2011 by Kent Tessman
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```


# Links

- [Hugo website]
- [Hugo SVN repository]:
    + [Hugo Library][SVN Lib]
    + [License][SVN License]
- [IF Archive » Programming » Hugo » Library]


<!-----------------------------------------------------------------------------
                              REFERENCE LINKS
------------------------------------------------------------------------------>

[original license file on Hugo website]: http://develop.generalcoffee.com/hugo/trac/browser/trunk/License.txt "See the original Hugo License file at the official Hugo Subversion repository"
[version currently available on the IF Archive]: https://www.ifarchive.org/indexes/if-archive/programming/hugo/library/ "Visit the Hugo Library section of the IF Archive"
[BSD 2-Clause License]: ./LICENSE "View full text of the BSD 2-Clause License"

<!-- badges -->

[license badge]: https://img.shields.io/badge/license-BSD%202--Clause-00b5da
[travis badge]: https://travis-ci.com/tajmone/hugo-library.svg?branch=master "Travis CI Status: EditorConfig validation"

<!-- Hugo -->

[Hugo website]: http://www.generalcoffee.com/hugo/gethugo.html "Visit the Hugo website"
[Hugo SVN repository]: http://develop.generalcoffee.com/hugo/trac/browser/trunk "Visit the official Hugo Subversion repository (frozen)"

<!-- Links -->

[IF Archive » Programming » Hugo » Library]: https://www.ifarchive.org/indexes/if-archive/programming/hugo/library/
[SVN Lib]: http://develop.generalcoffee.com/hugo/trac/browser/trunk/lib "View the Hugo Library at the official Hugo Subversion repository"
[SVN License]: http://develop.generalcoffee.com/hugo/trac/browser/trunk/License.txt "See the original Hugo License file at the official Hugo Subversion repository"

<!-- EditorConfig -->

[EditorConfig]: https://editorconfig.org/ "Visit EditorConfig website"
[ECLint]: https://www.npmjs.com/package/eclint "Visit ECLint page at NPM"
[either natively or via plug-ins]: https://editorconfig.org/#download "More info on editors support for EditorConfig"

<!-- project files -->

[LICENSE]: ./LICENSE "View full text of the BSD 2-Clause License"

<!-- people -->

[Tristano Ajmone]: https://github.com/tajmone "View Tristano Ajmone's GitHub profile"
[Kent Tessman]: https://github.com/tessman "View Kent Tessman's GitHub profile"

<!-- EOF -->
