# Not So Great Escape
__Category__: Misc  
__Points__: 440
>We've set up a chroot for you to develop your musl code in. It's bare, so install whatever you need.
>
>Feel free to log in with a raw TCP socket at challenges.tamuctf.com:4353.
>
>The password is "2ff6b0b9733a294cb0e0aeb7269dea5ae05d2a2de569e8464b5967c6c207548e"

#### Overview

The server sets up a remote shell in a chroot for us where we can   
do basically anything. Below is a snapshot of an interaction with   
the server which shows essential information needed later on:
```
sh-5.0$ nc challenges.tamuctf.com 4353
Password: 2ff6b0b9733a294cb0e0aeb7269dea5ae05d2a2de569e8464b5967c6c207548e

/ # ls -l
ls -l
total 0
drwxr-xr-x    2 root     root          1740 Mar 29 12:23 bin
drwxr-xr-x    4 root     root           100 Mar 29 12:23 dev
drwxr-xr-x   19 root     root           840 Mar 29 12:23 etc
drwxr-xr-x    2 root     root            40 Mar 29 12:23 home
drwxr-xr-x    6 root     root           320 Mar 29 12:23 lib
drwxr-xr-x    5 root     root           100 Mar 29 12:23 media
drwxr-xr-x    2 root     root            40 Mar 29 12:23 mnt
drwxr-xr-x    2 root     root            40 Mar 29 12:23 opt
dr-xr-xr-x    2 root     root            40 Mar 29 12:23 proc
drwx------    2 root     root            60 Mar 29 12:23 root
drwxr-xr-x    2 root     root            40 Mar 29 12:23 run
drwxr-xr-x    2 root     root          2080 Mar 29 12:23 sbin
drwxr-xr-x    2 root     root            40 Mar 29 12:23 srv
drwxr-xr-x    2 root     root            40 Mar 29 12:23 sys
drwxr-xr-t    2 root     root            40 Mar 29 12:23 tmp
drwxr-xr-x    7 root     root           140 Mar 29 12:23 usr
drwxr-xr-x   12 root     root           260 Mar 29 12:23 var
/ # id
id
uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),3(sys),4(adm),6(disk),10(wheel),11(floppy),20(dialout),26(tape),27(video)
/ # env
env
chroot_dir=/tmp/tmp.AFCIBh
HOSTNAME=96e785d2213a
SHLVL=3
SOCAT_PEERADDR=178.6.193.66
HOME=/root
SOCAT_PEERPORT=54244
SOCAT_SOCKADDR=172.17.0.5
version=2.10.4-r3
SOCAT_VERSION=1.7.3.3
SOCAT_SOCKPORT=4353
arch=x86_64
PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
mirror=http://mirror.math.princeton.edu/pub/alpinelinux/
PWD=/
SOCAT_PID=8
SOCAT_PPID=1
/ # exit
exit
sh-5.0$ 
```
Interesting things from the snapshot:
- We are root
- We have a full-blown Alpine linux setup
- We know the chroot directory

The objective is clear: Do a chroot escape.

#### Chroot Escapes in Theory
A process has two important properties when it comes to locating files in the filesystem.
- `cwd` (current working directory) which is used as the basis for relative paths
- `root` which is used as the basis for absolute paths
  
Everybody knows that when `cwd = /home` opening `asdf` would cause the operating system to open
`/home/asdf`. In the same way when `root = /home` opening `/asdf` would still result in the operating system looking for `/home/asdf`.   
We have the view of the process on the one hand and the view of the operating system on the other hand.  
It starts to get interesting when the `cwd` is above the `root` like so:   
`cwd = /tmp/tmp.AFCIBh` and `root = /tmp/tmp.AFCIBh/some-sub-dir`.    
Opening `asdf` now would result in the operating system looking for `/tmp/tmp.AFCIBh/asdf`
while opening `/asdf` would result in the operating system looking for `/tmp/tmp.AFCIBh/some-sub-dir/asdf`.    

And all that chroot() does is setting the `root` property of a process to the given path.   
Note that only the root user is allowed to change this property and that doing a chroot()
does not affect the `cwd`. You have to change `cwd` manually with chdir().

Utilizing that knowledge we can create a chroot escape plan.

#### Crafting a Chroot Escape
The basic exploit plan is as follows:

| # | Command            | cwd of process (process view)          | root of process        |
|---|--------------------|----------------------------------------|------------------------|
| 0 | _Login_            | /                                      | /tmp/tmp.AFCIBh        |
| 1 | mkdir("subdir")    | /                                      | /tmp/tmp.AFCIBh        |
| 2 | chroot("./subdir") | /tmp/tmp.AFCIBh                        | /tmp/tmp.AFCIBh/subdir |
| 3 | chdir("..")        | /tmp                                   | /tmp/tmp.AFCIBh/subdir |
| 4 | chdir("..")        | /                                      | /tmp/tmp.AFCIBh/subdir |
| 5 | exec("./bin/sh")   | /                                      | /tmp/tmp.AFCIBh/subdir |

See [payload.c](./payload.c) for an implementation of this in C.

#### Crafting an Exploit
The exploit for this challenge is:
- Build a binary that implements a chroot escape like above
- Convert that binary to base64
- Upload it to the server
- Decode it 
- Execute it

See [exploit.py](./exploit.py) for the concrete exploit.

After all that we get the flag
```
gigem{up_up_&_a_way_0u7}
```
