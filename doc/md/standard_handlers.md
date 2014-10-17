# Standard Handlers

Your KISSCPP derived applications, will be implementing handlers of it's own.
But there are certain things that all applications need. Here is a short list
of some of those things:

- keeping and retrieving of statistics
- keeping, retrieving and clearing application error states.
- The ability to adjust an applications log level, without restarting the
application.
- The ability to see a list of handlers that an application implements.

KISSCPP aims to provide standardised mechanisms for all of the above, and more.

Here is a list of the standard handlers, currently implemented by KISSCPP.

_The **kch** prefix, is shorthand for (K)iss(C)pp (H)andler. To prevent
conflict between KISSCPP specific handlers and your own, do not prefix any of
your custom handler identifiers with **kch**._

| **Handler Id**         | **Purpose**                                               |
|------------------------|-----------------------------------------------------------|
| kch-loglevel           | Adjust the logging to specified levels                    |
| kch-handlers           | shows a list of the request handlers for this application |
| kch-errstat            | retrieves the application error states                    |
| kch-errclear           | Used to marks an application error state as cleared.      |
| kch-stat               | retrieves the application statistics                      |

In order to ease the introduction to this here, we'll start with discussing the
adjustment of log levels.

## Adjusting log level on the fly.

For a discussion on what log levels are and how they are used, please see the
section on [logging](md_logging.md).

| **Handler**  | **Purpose**                                                                   |
| :----------: | :---------------------------------------------------------------------------: |
| kch-loglevel | Used to set the logging level to new values, while an application is running. |

### Parameters

| **Parameter name**     | **Mandatory?**       | **possible values**      |
| :--------------------- | :------------------: | :----------------------- |
| type                   | Yes                  | "debug", "info", "error" |
| severity               | Yes                  | "low", "normal", "high"  |

### Examples
- The example blow, will cause a KISSCPP application to start logging everything
at all log levels, because the lowest log level has a log type of _debug_, and a
severity of _low_.
~~~
{"kcm-cmd":"kch_loglevel","type":"debug","severity":"low","kcm-client":{"id":"foo","instance":"1"}}
~~~
- And in the second example bellow, logging will be restricted to logs of
type _info_ or _error_ with a severity of at least "normal".
~~~
{"kcm-cmd":"kch_loglevel","type":"info","severity":"normal","kcm-client":{"id":"foo","instance":"1"}}
~~~

Here is an example of a bash script one can use for setting an application's log
levels from the command line.

~~~(.sh)
#!/bin/bash
KISSCPP_HOSTNAME=""
PORT=""
LOG_TYPE="info"
LOG_SEVERITY="normal"
CLIENT_ID="script"
CLIENT_INSTANCE="1"

if [ -n $1 ]; then
  KISSCPP_HOSTNAME=$1
else
  echo "You need to specify an address to send too."
  exit 1
fi

if [ -n $2 ]; then
  PORT=$2
else
  echo "You need to specify a port to send on."
  exit 1
fi

if [ -n $3 ]; then LOG_TYPE=$3;        fi
if [ -n $4 ]; then LOG_SEVERITY=$4;    fi
if [ -n $5 ]; then CLIENT_ID=$5;       fi
if [ -n $6 ]; then CLIENT_INSTANCE=$6; fi

echo '{"kcm-cmd":"kch-loglevel","type":"'$LOG_TYPE'","severity":"'$LOG_SEVERITY'","kcm-client":{"id":"'$CLIENT_ID'","instance":"'$CLIENT_INSTANCE'"}}' | nc -w 2 $KISSCPP_HOSTNAME $PORT
~~~
















Logging and it's implementation in KISSCPP is discussed [here](md_logging.md).

This is a discussion on how to change the type and severity of logs that are
written by an application, while the application is running.

The standard handler for this is called 


## Format

After much investigation and debate, JSON was yet again selected as the
preferred format. In the case of configuration files, we found that INI format
lacks the ability for nested records and XML quickly becomes an unreadable mess.
The only down side we have identified with the use of JSON as a configuration
file format, was it's lack of native support for comments. This is a shortcoming
we are more than willing to live with.

## Reserved identifiers

The following are reserved identifiers, for use in configuration files:
Note: The prefix of **kcc** is shorthand for (K)iss(C)pp (C)onfiguration.

_The **kcc** prefix, is shorthand for (K)iss(C)pp (C)onfiguration. To prevent
conflict between KISSCPP specific configuration and your own application data, do
not prefix any of your custom configuration variables with **kcc**._


| **Identifier**          | **Description**                                                                                                                     |
|-------------------------|-------------------------------------------------------------------------------------------------------------------------------------|
|kcc-server.address       | the hostname or ip address of the server.                                                                                           |
|kcc-server.port          | the port of the server.                                                                                                             |
|kcc-stats.gather-period  | Seconds between gathering statistics for historic purposes.                                                                         |
|kcc-stats.history-length | Number of historic stats gatherings to keep.                                                                                        |
|kcc-log-level.type       | The default type limitation on logs.                                                                                                |
|kcc-log-level.severity   | The default severity limitation on logs.                                                                                            |
|kcc-white-list           | A root node containing data regarding white list communications. More detail available [here](md_white_listed_communications.html). |

## Configuration file naming standard
KISSCPP applications will have configuration files named as follows:

~~~
<application-id>.<application-instance-id or "common">.kcppcfg
~~~

i.e. An application with the id "foo", being executed with an instance identifier
of "bar" will look for a configuration file named:

~~~
foo.bar.kcppcfg
~~~

This file must contain configuration data that is specific to the **foo**
application running as instance **bar**.

There is however, a mechanism for allowing shared configuration to exist in a
single file. This is known as the common-configuration file.
This file will have the instance id portion of the file name, replaced by the
string "common".

i.e. An application with the id **foo**, being executed with an instance identifier
of **bar** will look for shared configuration details in a file named.

~~~
foo.common.kcppcfg
~~~

## Configuration file location

Due to the fact that KISSCPP is a library meant for large systems consisting of
interdependent processes, we did have to standardize on most aspects of the
configuration files. The control we can give you, is based around a holistic
approach, and the fact that the developer is not the only one that needs to be
considered here. System administrators and support personnel also need to have
a sane standard to work with.

As such, the following decision were made:

-# Only the root path for configuration files, will be under user control.
i.e. You can specify nothing more than the path under which KISSCPP will search
for the following:
   + A sub-directory with a name matching the application-id.
   + Files contained in that sub-directory, that match the file naming standard
   discussed above.
-# A mechanism for environment separation will be available.
i.e. You have the ability to keep environment specific files separated from
each other. This mechanism is provided through the **KCPP\_EXEC\_ENV**
environment variable

The configuration file path, is constructed in one of three ways:

- Where **KCPP\_CFG\_ROOT** and **KCPP\_EXEC\_ENV** are used:
  $KCPP\_CFG\_ROOT/<application-id>/$KCPP\_EXEC\_ENV/<application-configuration-file-name>
- Where only **KCPP\_CFG\_ROOT** is used:
  $KCPP\_CFG\_ROOT/<application-id>/<application-configuration-file-name>
- Developers can override any setting of **KCPP\_CFG\_ROOT** by specifying the
  path at the time of instantiating the Server class.

That means, when you have a KISSCPP application with the name **foo** and you
start instance **bar** of it, while having **KCPP\_CFG\_ROOT** set to
<strong>/put/configurations/here</strong>

Your application will look in:
- /put/configurations/here/foo
- for a files named
  + foo.common.kcppcfg and
  + foo.bar.kcppcfg

Meaning that the full path to these files will be:
- /put/configurations/here/foo/foo.common.kcppcfg and
- /put/configurations/here/foo/foo.bar.kcppcfg

Now, with all of that in play, you can still set the **KCPP\_EXEC\_ENV**
environment variable. This comes in handy when you have differing configurations
for your test, quality control and production environments.

Thus, with all the above and **KCPP\_EXEC\_ENV** set too: **live**

Your application will look in:
- /put/configurations/here/foo/live
- for files named
  + foo.common.kcppcfg and
  + foo.bar.kcppcfg

Meaning that the full path to these files will be:
- /put/configurations/here/foo/live/foo.common.kcppcfg and
- /put/configurations/here/foo/live/foo.bar.kcppcfg

## Example.

Ok, let's say you have an application called **ninjarules** and you need to run
three instances of that application on the same machine.

To keep things sane, you set **KCPP\_CFG\_ROOT** too <strong>/my/central/config</strong>.
You also know that your test and your live environments will have different
configurations so you plan on setting **KCPP\_EXEC\_ENV** to **live** and **test**
respectively, so that you can avoid having to maintain both sets of configurations
in one file.

You further decide, although you can use any arbitrary string as an instance
identifier, that you'll stick to the simple numbers 1, 2 and 3, for your various
instances.

Your application will therefore look in these directories:
- In the **live** environment: /my/central/config/ninjarules/live
- In the **test** environment: /my/central/config/ninjarules/test

For files named:
- ninjarules.common.kcppcfg,
- ninjarules.1.kcppcfg,
- ninjarules.2.kcppcfg and
- ninjarules.3.kcppcfg

There are some things that will be common for all instances of **ninjarules**,
so you'll want to keep them in the common configuration. Here's an example of
that:
~~~~(ninjarules.common.json)
{
  "kcc-stats"     : {
    "gather-period"  : "300",
    "history-length" : "12"
  },
  
  "kcc-log-level" : {
    "type"           : "info",
    "severity"       : "normal"
  }
}
~~~~

And then of course, there will be the instance level configuration files: 
~~~~(ninjarules.1.kcppcfg)
{
  "kcc-server"    : {
    "address"        : "locahost",
    "port"           : "9001"
  }
}
~~~~
~~~~(ninjarules.2.kcppcfg)
{
  "kcc-server"    : {
    "address"        : "locahost",
    "port"           : "9002"
  }
}
~~~~
~~~~(ninjarules.3.kcppcfg)
{
  "kcc-server"    : {
    "address"        : "locahost",
    "port"           : "9003"
  }
}
~~~~

