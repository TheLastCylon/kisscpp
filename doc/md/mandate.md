# Contributor's Mandate

As a contributor, you'll need to keep a few things in mind.

The most important thing you have to keep in mind with regard to KISSCPP is the
drive towards simplicity. Within the context of KISSCPP, simplicity trumps
performance in most cases.

i.e. Optimization for the sake of speed, will happen if and when necessary.

Clarity of code and usability of the library, trumps clever solutions every
time.

Also, in order to keep this library sane, portable, future proof and maintainable
a few decisions had to be made:

-# Other than the standard C++ library, only the Boost library may be used. i.e.
   KISSCPP as a library, may not make use of anything that falls outside
   standard C++ and the C++ boost libraries.
-# In order to satisfy the requirement for a standardised internal
   representation of inter process communications, the boost property tree
   and supporting framework was investigated and approved over custom
   implementations.
-# It was decided that the Inter Process Protocol for KISSCPP derived
   applications, will be **JSON**. Other protocols were considered, but only
   **JSON** served all the requirements of an IPC Protocol within this context.
   Other protocols may be supported in the future, but only if they are provided
   as part of the boost property tree library. For more detail please refer to:
   [Inter Process Communication](md_inter_process_communication.html)
-# For networking boost::asio is used.
-# For the time being, and until we have certainty that most compilers will deal
   consistently with the new C++ 11 standard, we'll be avoiding use of the
   C++ 11 standard. This is not because I don't want to use C++ 11 standard.
   It's rather a matter of knowing the environments this library is intended
   for. Those environments are rife with paper pushing idiots, who consume
   developers souls for pleasure. We are trying to make things as easy as
   possible for our tortured friends in these environments.

