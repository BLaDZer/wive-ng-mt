EOIP
====

Kernel mode EOIP (Ethernet Over IP) tunnel compatible with MikroTik RouterOS

There are several projects doing the same job with userland utilities via tap interfaces and raw sockets. While a userland application is easier to install and maintain it lacks the perfomance and stability of an in-kernel module. Especially for the simple job of adding and stripping the EOIP tunneling headers. The userland tunneling application may be good for testing, research or concept proof projects but not suitable for production environments with high bandwidth requirements.

This project's goals are:

- to solve the perfomance issue with EOIP on Linux
- to make EOIP tunneling support a standard part of the Linux world

Userland management utility
---------------------------

##### `eoip` - tunnel management utility

- to create new eoip tunnel interface:

```
    eoip add tunnel-id <tunnel-id> [name <if-name>]
             [local <src-address>] [remote <dst-address>]
             [link <ifindex>] [ttl <ttl>] [tos <tos>]
```

- to change existing eoip tunnel interface:

```
    eoip change name <if-name> tunnel-id <tunnel-id>
                [local <src-address>] [remote <dst-address>]
                [link <ifindex>] [ttl <ttl>] [tos <tos>]
```

- to list existing eoip tunnels:

```
    eoip list
```


License
-------

All code and code modifications in this project are released under the GPL licence. Look at the COPYING file.

