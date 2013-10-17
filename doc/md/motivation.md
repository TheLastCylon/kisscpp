# Why do this? What's the motivation?

Well, there are actually several reasons, but the most important ones are
listed here.

## Bad code

I used to work in an environment where I had to maintain source code containing
C functions, spanning 2000 lines. One or two such functions would not really
have bothered me, as I would simply conclude that the code was done under
pressure, and a re-factor was in order. The problem here though, was the 37 near
duplicate functions each spanning said 2000 lines of code.

At one point, I was convinced that this kind of code, was a result of
incompitant developers in that environment. I was partially correct, in that
there were developers that were incompitent. However, this was a reflection
of management and the prevailing employment strategies, as well as a
non-existant internal communication structure.

It was the kind of environment, where non-technical people were placed in senior
technical positions, resulting in decisions around tooling being made by people
who never use the tools.

Sadly, I'm not exagirating. If I were, I'd probably still be working in that
environment.

In short though, it was the typical Waterfall development, perpetuated by a
poor implimentation of the Rational Unified Process, where hamstrung
project-managers had no choice but to pester hamstrung developers into hacking
a 15 year old, poorly designed system, so as to provide the appearance of
valuable solutions to the business.

KISSCPP is the culmination of frustration and experiance, into a library that
will make it possible for developers at lartge corporates, to deliver
re-factored components, at a fraction of the cost of maintaining old ones.

Yes, the system will have to be re-designed to make use of KISSCPP, but the cost
would be rediculously low compared to maintaining a system like the one I
had to deal with.

## Development speed

Over the years I've had to listen to the "My language is better than yours"
debate, one too many times. The more I litened to these debates, the more I
realized that there are several types of Software Developers.

1. **Those that don't know.**
> These are the ones, that are just starting out. They are totaly new to the
> industry and can be forgiven almost anything.
2. **Those that think they know.**
> These are the ones that will blindly stick to a single programming language/
> tool/methedology/paradigm etc. They will happily argue that a business will
> only need the tool they happen to be proficient in. They are also the kind
> of developer that will argue, that Delphi is a programming language and PHP
> is fast enough to be used as a tool for creating a database cache service.
3. **Those that know.**
> These are the ones that actually get qualified in multiple programming
> languages, using diverse paradigms. They are the ones that will convince a
> business of the need for a multitude of tools and skills to manage it's own
> software. They will prescribe PHP for the web requirements, Java for
> business components, C++ for the componets that feel the need for speed, C
> for those that feel the need for warp speed, assembler for trans-warp speeds
> and machine code for instantanious teleportation.

There is a particular kind of the "Think they know" developer, that I aim to
address, using the KISSCPP library. They are the ones who argue, that getting
something done in C++ takes too much development effort. With the release of
KISSCPP and the various applications that use it, I aim to demonstrate just how
fast one can develop in C++, when you are willing to standardise your development
and/or design.

The only advantage a programming language like Java has over C++, is that JAVA
developers have been forced to standardise their solutions.

KISSCPP is a step in that direction, for C++ developers.

