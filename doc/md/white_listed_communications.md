# White listed communications

KISSCPP now supports white listed communications. This can be configured at
application or instance level, through the data in the **kcc-white-list** node
in your configuration file.

Note: Where the **kcc-white-list** node does not exist in your configurations,
KISSCPP will default to allowing all ip addresses and all instances of all
applications.

i.e. Not specifying a white list, leaves your application open to communication
from all sources.

Nested directly under **kcc-white-list**, you have nodes to indicate:
- which ip addresses are allowed,
- which other applications are allowed,
- as well which instances of those applications are allowed.
- There are also nodes to specify that you want to allow all ip-addresses,
  applications and instances of an application.

## IP white listing.
Please take note, you do need to specify either a list of allowed ip addresses,
or explicitly set **all-ip-addrs** to **true**. If you neglect to have either of
these in your configuration, your application simply won't start. The moment you
have a **kcc-white-list** node in your configuration, it is assumed that you do
want to have white listed communications, and KISSCPP expects you to be explicit
with what should be allowed.

### Allowing all ip addresses
This is achieved through setting **kcc-white-list.all-ip-addrs** to **true**.
~~~
{
  "kcc-white-list" : {
    "all-ip-addrs" : "true",
    .
    .
    .
    .
  }
}
~~~
Take note: If **all-ip-addrs** is set to **true** at either common or instance
level configuration, it will take effect over any **ip-list** configuration, 
at any level. To prevent problems, make sure to only have **all-ip-addrs** in
your configuration files, if you **really** want all ip addresses to be allowed.

### Allowing only a set of ip addresses
In the example below, only communications from 127.0.0.1 and 192.168.1.10 will
be allowed to do requests to your application.
~~~
{
  "kcc-white-list" : {
    "ip-list"      : {
      "ip" : "127.0.0.1",
      "ip" : "192.168.1.10"
    },
    .
    .
    .
    .
  }
}
~~~

## Application white listing.
Application white listing, works in conjunction with ip white listing.
i.e. Even if an application is allowed, if it is not running on an allowed
ip address, it will still be blocked.

### Allow all applications
Setting **all-apps** to true, will result in all instances of all applications
being allowed to communicate with your application.
Also note that, as with **all-ip-addrs**, if it is specified at any configuration
level, it will take effect over all application-lists.
i.e. Only have **all-apps** in your configurations, if you really want all
applications to be allowed.
~~~
{
  "kcc-white-list" : {
    "all-apps"     : "true",
      .
      .
      .
      .
  }
}
~~~

### Allow only a set of applications, but all instances of each application
In the list below, only applications having the application id of **foo**
and **bar** will be allowed, but all instances of those applications will be
allowed.
~~~
{
  "kcc-white-list" : {
    "application-list" : {
      "application" : {
        "id"            : "foo",
        "all-instances" : "true"
      },
      "application" : {
        "id"            : "bar",
        "all-instances" : "true"
      },
    }
    .
    .
    .
    .
  }
}
~~~

### Allow only a set of applications, and only a set of instances for an application.
In the example below, only applications **foo**, **bar** and **coolcats** will
be allowed. For **foo** only instances 3, 5 and 7 will be allowed. For **bar**
only instances 1, 2 and 3 will be allowed. On the other hand, any instance of
**coolcats** will be allowed.
~~~
{
  "kcc-white-list" : {
    "application-list" : {
      "application" : {
        "id"            : "foo",
        "instance_list" : {
          "id" : "3",
          "id" : "5",
          "id" : "7",
        },
      },
      "application" : {
        "id"            : "bar",
        "instance_list" : {
          "id" : "1",
          "id" : "2",
          "id" : "3",
        },
      },
      "application" : {
        "id"            : "coolcats",
        "all-instances" : "true"
      },
    }
    .
    .
    .
    .
  }
}
~~~

## Putting it all into practice.
### Explicitly allowing everything.
This is the functional equivalent of excluding the **kcc-white-list** node from
you configuration.
~~~
{
  "kcc-white-list" : {
    "all-ip-addrs" : "true",
    "all-apps"     : "true"
  }
}
~~~

### Explicitly allowing all ip addresses, but limiting applications.
~~~
{
  "kcc-white-list" : {
    "all-ip-addrs"     : "true",

    "application-list" : {
      "application" : {
        "id"            : "foo",
        "instance_list" : {
          "id" : "3",
          "id" : "5",
          "id" : "7",
        },
      },
      "application" : {
        "id"            : "bar",
        "instance_list" : {
          "id" : "1",
          "id" : "2",
          "id" : "3",
        },
      },
    }
  }
}
~~~

### Explicitly allowing all applications, but limiting ip addresses.
~~~
{
  "kcc-white-list" : {
    "ip-list"      : {
      "ip" : "127.0.0.1",
      "ip" : "192.168.1.10"
    },

    "all-apps" : "true"
  }
}
~~~

### Limiting both ip and applications.
~~~
{
  "kcc-white-list" : {
    "ip-list"      : {
      "ip" : "127.0.0.1",
      "ip" : "192.168.1.10"
    },

    "application-list" : {
      "application" : {
        "id"            : "foo",
        "all-instances" : "true"
      },
      "application" : {
        "id"            : "bar",
        "all-instances" : "true"
      },
    }
  }
}
~~~

### Limiting ip, application and instances of allowed applications
~~~
{
  "kcc-white-list" : {
    "ip-list"      : {
      "ip" : "127.0.0.1",
      "ip" : "192.168.1.10"
    },

    "application-list" : {
      "application" : {
        "id"            : "foo",
        "instance_list" : {
          "id" : "3",
          "id" : "5",
          "id" : "7",
        },
      },
      "application" : {
        "id"            : "bar",
        "instance_list" : {
          "id" : "1",
          "id" : "2",
          "id" : "3",
        }
      }
    }
  }
}
~~~

