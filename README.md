
# ABOUT

**dsblogoutmgr**
is a Qt utility that allows you to leave your current window manager-session,
reboot, suspend or shutdown your system. Furthermore, it supports
time-controlled shutdown.

# INSTALLATION

## Dependencies

**dsblogoutmgr**
depends on devel/qt5-buildtools, devel/qt5-core, devel/qt5-linguisttools,
devel/qt5-qmake, x11-toolkits/qt5-gui, and x11-toolkits/qt5-widgets

## Building and installation

	# git clone https://github.com/mrclksr/DSBLogoutmgr
	# git clone https://github.com/mrclksr/dsbcfg.git
	
	# cd DSBLogoutmgr && qmake && make
	# make install

# USAGE

**dsblogoutmgr**
\[**-L** *&lt;lock command&gt;*]
\[**-S** *&lt;suspend command&gt;*]
\[**-l** *&lt;logout command&gt;*]
\[**-r** *&lt;reboot command&gt;*]
\[**-s** *&lt;shutdown command&gt;*]

# OPTIONS

**-L**

> Defines the screen lock command. Default
> "metalock"

**-S**

> Defines the suspend command. Default
> "acpiconf -s 3"

**-l**

> Defines the logout command. Default
> "fluxbox exit"

**-r**

> Defines the reboot command. Default
> "shutdown -r now"

**-s**

> Defines the shutdown command. Default
> "shutdown -p now"

# SETUP

Commands for the actions can also be defined in
*~/.config/DSB/dsblogoutmgr/config*
using variable=value pairs by setting the variables
*logout*, *shutdown*, *reboot*,
and
*suspend*.
Commands containing white spaces must be enclosed in double
quotes. If an action is defined on the command line, it is used instead
of the one defined in
*~/.config/DSB/dsblogoutmgr/config*.

## Permissions

In order to be able to execute
*shutdown*
as regular user, you can either use
sudo(8) (see below), or you can add your username to the
*operator*
group:

	# pw groupmod operator -m yourusername

If you want to be able to suspend your system as
regular user who is member of the wheel group, you can use
sudo(8).
Add

	%wheel  ALL=(ALL) NOPASSWD: /usr/sbin/acpiconf *

to
*/usr/local/etc/sudoers*,
and define the suspend action accordingly by using
the
**-S**
flag on the command line, or set

	suspend = "sudo acpiconf -s 3"

in
*~/.config/DSB/dsblogoutmgr/config*

## Closing window manager-sessions

**Openbox**

> Users of
> *OpenBox*
> can set the
> *logout*
> action to
> "openbox --exit"
> using the
> **-l**
> flag.

**Fluxbox**

> Users of
> *Fluxbox*
> can set the
> *logout*
> action to
> "fluxbox-remote exit"
> using the
> **-l**
> flag. This requires

> > session.screen0.allowRemoteActions: true

> to be set in
> *~/.fluxbox/init*.

