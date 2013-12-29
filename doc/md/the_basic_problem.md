# The Basic Problem

In any message based system you have shared functionality across applications.

The ability to send, receive, parse and process messages, is a technical
problem that should be centralised in a simple library.

KISSCPP provides a framework and an application level communications standard,
allowing developers to create message based systems quickly and easily.
It provides the major functionality for creating client-server applications in
C++, while allowing developers to focus on getting business rules implemented.

Take for instance, the two theoretical message based system as described in below:

1. **Distributed Message based system**
![Distributed Message based system](dia/message_based_system_1.png "Figure 1")
2. **Message Broker based system**
![Message Broker based system](dia/message_based_system_2.png "Figure 2")

Both these architectures have their own problem sets when dealing with code
maintenance and application deployment. Depending on the situation you are
facing, you could decide to use either one.

KISSCPP currently leans more towards solving the distributed message based
system. This will only be the case while a message broker using the KISSCPP
library, does not exists. This is currently in planning phase and will be
released as a separate project.

## Why not use Google's protocol buffers?

I wanted to, really, how awesome would it be to say I use Google's cool stuff :)
But there is the real world of business needs, a world where:

1. JSON is much better supported across a much wider programming and scripting
   language base. i.e. More people would be able to use JSON than protocol
   buffers. Think about it. KISSCPP based applications will need to interface
   with arbitrary systems. Using JSON has a much higher chance of success than
   protocol buffers ever would.
2. The documentation for protocol buffers, suggest that it's only really for
   message sizes up to 1 Mb. Seeing as I've worked in environments where the
   1Mb message size is routinely exceeded, I decided on using something more
   forgiving.
3. Having to re-compile and re-deploy components every time an interface changes,
   causes huge issues for all but the smallest of systems. Yes, interfaces
   don't change every day, but they change often enough to make the lives of
   all involved in the development process, a royal pain in the posterior.
4. Protocol buffers destroy any possibility of having templitized messages.

## Why not use CORBA?

1. Maintaining IDL files routinely becomes a mess. In the real-world,
   interfaces change just often enough to make this a problem.
2. Again, having to re-compile and re-deploy components every time an interface
   changes, causes problems. See my comments on protocol buffers.
3. CORBA destroys any possibility of having templitized messages.

## Why not use 0mq?

0mq lacks one huge feature: Persistence!
There are other issues with it, but persistence is the big one.

## Why not use GO?

GO is too young to know wither or not it will stick and become an industry
standard. If it does, that would be awesome, but I don't see a future where big
corporates are going to re-factor towards using GO for their server side
components.

