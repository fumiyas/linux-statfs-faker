Preloadable library to fake Linux statfs(2) information
======================================================================

  * Copyright (c) 2019 SATOH Fumiyasu @ OSS Technology Corp., Japan
  * License: GNU General Public License version 2
  * URL: <https://GitHub.com/fumiyas/linux-statfs-faker>
  * Twitter: <https://twitter.com/satoh_fumiyasu>

What's this?
---------------------------------------------------------------------

This is a `$LD_PRELOAD`-able library and a wrapper script to
run a command with the special `statfs`(2) function for faking
the type of filesystems.

Use case
---------------------------------------------------------------------

  * Force to run a filesystem-aware command on an unsupported
    filesystem. (e.g., Dropbox on XFS)
  * Prevent a command from using filesystem-specific functions that
    trigger a bug. (https://bugzilla.redhat.com/show_bug.cgi?id=1751934)

How to build
---------------------------------------------------------------------

Required packages: autoconf, automake, libtool, make, cc (gcc or misc)

```console
$ sh autogen.sh
$ ./configure --prefix=/usr/local
$ make
$ sudo make install
```

Usage
---------------------------------------------------------------------

Run a command with `$LD_PRELOAD`-able library:

```console
$ /usr/local/bin/statfs-faker
Usage: /usr/local/bin/statfs-faker [OPTIONS] COMMAND [ARGUMENT ...]

Options:
 -t, --type TYPE
    Specify the fake type value (f_type) of filesystem
    (See statfs(2) manual page for valid f_type values)
$ stat -f / |grep Type:
    ID: fe0000000000 Namelen: 255     Type: xfs
$ /usr/local/bin/statfs-faker -t 0xEF53 stat -f / |grep Type:
    ID: fe0000000000 Namelen: 255     Type: ext2/ext3
$ /usr/local/bin/statfs-faker -t 0x9123683E stat -f / |grep Type:
    ID: fe0000000000 Namelen: 255     Type: btrfs
```
