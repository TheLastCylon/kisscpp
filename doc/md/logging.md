# Logging

All but the most trivial of applications, require logging at some level.
KISSCPP has been designed with logging in mind, and presents the developer
with a mechanism that closely emulates C++ streams, through the use of the
LogStream class.

Each KISSCPP application is restricted to having one log file, but you can
log to that file, from anywhere in the application.

## Introduciton to log levels

There are nine log levels, divided into three log types and each log type
has three severity levels.

The three types of logs are _error_, _info_ and _debug_.
Refer to the table below for more detail on each.

| **Log Type**            | **Purpose**                                                                                                                      |
| :---------------------: | :------------------------------------------------------------------------------------------------------------------------------- |
| error                   | Used to log actual processing errors, logs of this type are always at a higher log level than other log types.                   |
| info                    | For logging things that are valuable information on a live system. Typically things like business logic events.                  |
| debug                   | For use during development and stabilization of your applications. You should not be using this log type on a production system. |

Each of the above log types have three levels of severity associated with them. 
Those severities are _low_, _normal_ and _high_.

The combination of a log type and severity, gives you a logging level.
The table bellow shows the various log levels associated with a type and severity
combination.

| Log Level               | Type                 | Severity     |
| :---------------------: | :------------------: | :----------: |
| 9                       | error                | high         |
| 8                       | error                | normal       |
| 7                       | error                | low          |
| 6                       | info                 | high         |
| __5__                   | __info__             | __normal__   |
| 4                       | info                 | low          |
| 3                       | debug                | high         |
| 2                       | debug                | normal       |
| 1                       | debug                | low          |

KISSCPP defaults to a logging level of 5.

i.e. logs of type **info** with severity **normal**, or combinations that result
in a higher log level, will be written.

Yes, that is honestly how simple it is. KISSCPP **will not support** the ridiculous
number of logging levels supplied by other libraries. If a developer can not work
with only 9 log levels, 15 levels won't be enough either. There will be no support
for so called "log filtering" either. Filtering huge amounts of text is well covered
by tools like **grep**, **sed** and **awk**. Even the most stringent logging
practices, are easily managed with those tools.

## Using logging in your programs.

The header file you'll be needing is:
~~~
#include <kisscpp/logstream.hpp>
~~~

And here is an example of some code:
~~~
void myFunction()
{
  kisscpp::LogStream my_log(__PRETTY_FUNCTION__);                    // Instantiate a LogStream class.
                                                                     // Technically you can set the log source to be any arbitrary string here.
                                                                     // Setting it to the value of __PRETTY_FUNCTION__ is good practice though,
                                                                     // and you should be doing it.
                                                                    
  my_log << kisscpp::manip::info_normal                              // Set the level of this log line
                                                                     // Untill you change it, this will be the log level of all subsiquent log lines, logged
                                                                     // using this instance of LogStream.
         << "This is an informational log, at normal severity"       // Log your message.
         << kisscpp::manip::endl;                                    // Mark the end of your log line.

  my_log << "This is still an informational log, at normal severity" // Another log message
         << kisscpp::manip::endl;                                    // Mark the end of your log line.
                                                             
  my_log << kisscpp::manip::debug_high                               // Set the log level again.
         << "This is now a debug log, at high severity"              // Another message.
         << kisscpp::manip::endl;                                    // Mark the end of your log line.

  my_log << "This is still a debug log, at high severity"            // Another message.
         << kisscpp::manip::flush;                                   // Force a flush of the content of the log buffer to file.
}
~~~

## Log line format

~~~
<ISO Formatted Time-stamp> [<log source identifier>] <+|-|:> <message>
~~~

Where:
- (+) Indicates the point at which the LogStream object is created.
- (-) Indicates the point at which the LogStream object is destroyed.
- (:) Indicates a log line sent at a point between (+) and (-) 

e.g.
~~~
20141013T154716.537732 [void myFunction()] +
20141013T154716.537732 [void myFunction()] : This is an informational log, at normal severity
20141013T154716.537732 [void myFunction()] : This is still an informational log, at normal severity
20141013T154716.537732 [void myFunction()] : This is now a debug log, at high severity
20141013T154716.537732 [void myFunction()] : This is still a debug log, at high severity
20141013T154716.537732 [void myFunction()] -
~~~

## Log file naming standard
KISSCPP applications will have log files named as follows:

~~~
<application-id>.<application-instance-id>.log
~~~

i.e. An application with the id **foo**, being executed with an instance identifier
of **bar** will write to a log file named:

~~~
foo.bar.log
~~~

## Log file location

As with configuration files KISSCPP has to consider more than just the developer.
So here again, we have to enforce some standards. You will therefore only have
control over the root directory where log files reside.

The default directory for logging, regardless of operating system, is **/tmp**.
This can be over ridden through the use of the **KCPP\_LOG\_ROOT** and
**KCPP\_EXEC\_ENV** environment variables.

The log file path, is constructed thus:

- Where **KCPP\_LOG\_ROOT** and **KCPP\_EXEC\_ENV** are used:
  $KCPP\_LOG\_ROOT/<application-id>/$KCPP\_EXEC\_ENV/<application-log-file-name>
- Where only **KCPP\_LOG\_ROOT** is used:
  $KCPP\_LOG\_ROOT/<application-id>/<application-log-file-name>
- Where **KCPP\_LOG\_ROOT** is not used:
  /tmp/<application-id>/<application-log-file-name>

That means, when you have a KISSCPP application with the name **foo** and you
start instance **bar** of it, while having **KCPP\_LOG\_ROOT** set to
<strong>/put/logfiles/here</strong>

Your application will place log files in:
- /put/logfiles/here/foo
- and the log file name will be: foo.bar.log

Meaning that the full path to the log file will be:
- /put/logfiles/here/foo/foo.bar.log

If you take the extra measure of using **KCPP\_EXEC\_ENV** and set it to a value
**live**, your application will write that same log file to:
- /put/logfiles/here/foo/live/foo.bar.log

