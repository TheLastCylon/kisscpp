<link href="./github.css" rel="stylesheet"></link>

# KISS-Base

KISS-Base is a library containing classes, templates and supporting functions to
facilitate rapid application development of C++ server side applications. It is
in it's part, heavily dependent upon the C++ Boost libraries.

## Why

I come from an IT industry where an incredibly vast majority of developers,
think 1995 lines of C code, in a single function, is acceptable. What's more,
is that this seems to be justification enough for 37 copies of that function,
to exist within the same application.

At one point, I was convinced that this kind of behaviour was directly related
to the incompitance of supposed senior developers in that environment.

I was partially correct, in that there were developers that were incompitent.
Unfortunatly, that turned out to be a reflection of the incompitence of
management and a lack of satisfactory communication with business.

In short, it was the typical waterfall development environment where managers
spent more time protecting their employment than seeing to the needs of their
developers. Consiquintly, developers were blamed for the problems created by
these managers and business people. While not being given the opertunity to
properly solve those problems.



After 4 years in that environment I realized that the developers are not nearly
as incompetent as I suspected, but rater accutely demoralized. It turns out
that beurocracy is actually a developer's equivalent of Kryptonite.





After living in an IT industry where deranged developers, believe that C functions
spanning 1995 lines of code, and 37 copies of that same functions, baring a few
parameters here and there

After living in an IT industry where nobody outside the open source community
seems to share my perspective on clean, reusable and reliable code, 


## Mandate

This project owes it's existance to the KISS-SMPP project. It was developed to
satisfiy the requirement for a standardised framework, used to create custom
server side applications, that run as daemons.

The utility of this library in other applications was recognised early in it's
development, as such, it was decided to break this away from the KISS-SMPP
project, to allow for it's independent growth and maintinance.

In order to keep this library sane, portable and future proof a few limitations
in terms of technologies used were settled upon.

1. Other than the standard C++ library, only the Boost library may be used. i.e. 
   No other library may be linked to.
1. In order to satisfy the requirement for a standardised internal
   representation of inter process communications, the boost property tree
   and supporting framework was investigated and approved over custom
   implementations.
1. It was decided that the Inter Process Protocol for KISS-Base derived
   applications, will be **JSON**. Other protocols were considered, but only
   **JSON** served all the requirements of an IPC within this context.
   Other protocols may be supported in the future, but only if they are
   provided as part of the boost property tree library.
1. The networking 


and use inter process comm for custom 
create a bases from which to implement applications that run as server side
daemons for custom requirements.


The mandate of this project
collection of classes that form a 

Welcome!

Before reading any further, please take note:

KISS-SMPP is *not*, I repeat, *not* a library that will assist you with creating
your own SMPP client.

The intent behind KISS-SMPP, is to completely negate your need to create a
custom SMPP client and/or server. As such, the name "KISS-SMPP" does not refer
to a single application, but rather an applications-suite and several libraries,
that endevor to do the SMPP specific stuff for you.

The entire SMPP Session Management process is tucked away, where you'll never
have to touch it if you don't want to. All that's expected from you, is to know
how to use basic networking (*in the technology of your choice*) and have some
mechanism for decoding and encoding data from and to *JSON* format.

If you have that, you are almost ready to start using the KISS-SMPP client, for
sending and recieving text messages.

Take a look at the interface specification of the KISS-SMPP Client (ksmppcd).
You'll find that you'll never have to issue a Bind request or deal with
enquire\_link PDUs. Once you've completed the configuration of the client, you
could manage the sending and recieving of messages with nothing more than a well
written shell script. *It might be foolish, but it would be possible.*

# Terms and Definitions

| Term | Definition |
|------|------------|
| KISS | **K**eep **I**t **S**imple **S**tupid |
| SMPP | **S**hort **M**essage **P**eer-to-peer **P**rotocol |
| MC   | **M**essage **C**entre (the people you connect to, using an smpp client) |
| ESMC | **E**xternal **S**hort **M**essage **C**entre (The entity connecting to a Message Centre) |

# Basics of interfacing with KISS-SMPP Client (ksmppcd)

All applications in the KISS-SMPP application suit, use JSON as an interface
protocol. There are many reasons for this, but here are some major ones:

- It's human readable.
- It's much less bandwidth intensive than XML, SOAP, etc., etc., etc.
- It's incredibly well supported across major programming/scripting languages.




TODO: Define message centre.

|   |   |   |   |
|---|:-:|---|---|
| Parameter        | Mandatory? | Valid Values                 | Clarification     |
| cmd              |   Yes      | send                         |  The send command |
| source-addr      |   Yes      | a valid address for your MC. |                   |
| destination-addr |   Yes      | a valid address for your MC. |                   |
| short-message    |   Yes      | The text message you want to send ||

socket and your applications are expected to expose a few callback interfaces.
Other than that, the aim is to be as platform and technology agnostic, as can be reasonably expected.


You will not be forced to write code in any language
you feel uncomfortable with. Nor will you need to cram new and foreign ideas into your organization.

1.  *a collection of applications and libraries* that endevour to completeley negate the need for having to create your own **SMPP** client.



You are reading this so chances are good, that you want to know what **KISS-SMPP** is.
Let's get the obvious out of the way first:
KISS is an acronym for **K**eep **I**t **S**imple **S**tupid, and SMPP is an acronym for **S**hort **M**essage **P**eer-to-peer **P**rotocol.
From those two acronyms, you'd possibly be led to think, that **KISS-SMPP** is a library aimed at simplifying your life, with regard to implementing your own **SMPP** client.
However, you'd be mistaken.

So, what is it then?

Well, first off, **KISS-SMPP** isn't a library, it's a collection of applications and libraries 


~~~~
A code block
this is a cool code block
~~~~

## This is a test

### This is a test

1. list item
    - asdf
    - asdf
    - asdf
      - qwer
      - qwer
      - qwer
    - asdf

~~~~
A code block
~~~~

asdf

1. list item
1. list item
1. list item
1. list item
1. list item
1. list item
1. list item
1. list item

