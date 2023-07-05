.. _README.menu:

===========
README.menu
===========

Version: syslinux-3.35

This is the ``README.menu`` file from the SYSLINUX source.

.. Contents::

There are two menu systems included with SYSLINUX, the advanced menu
system, and the simple menu system.


THE ADVANCED MENU SYSTEM
========================

The advanced menu system, written by Murali Krishnan Ganapathy, is
located in the menu/ subdirectly.  It allows the user to create
hierarchial submenus, dynamic options, checkboxes, and just about
anything you want.  It requires that the menu is compiled from a
simple C file, see menu/simple.c and menu/complex.c for examples.

The advanced menu system doesn't support serial console at this time.

See menu/README for more information.


THE SIMPLE MENU SYSTEM
======================

The simple menu system is a single module located at
com32/modules/vesamenu.c32 (graphical) or com32/modules/menu.c32 (text
mode only).  It uses the same configuration file as the regular
SYSLINUX command line, and displays all the LABEL statements.

To use the menu system, simply make sure [vesa]menu.c32 is in the
appropriate location for your boot medium (the same directory as the
configuration file for SYSLINUX, EXTLINUX and ISOLINUX, and the same
directory as pxelinux.0 for PXELINUX), and put the following options
in your configuration file::

  DEFAULT menu.c32
  PROMPT 0


There are a few menu additions to the command line, all starting with
the keyword MENU; like the rest of the SYSLINUX config file
language, it is case insensitive:

MENU TITLE title
----------------

        Give the menu a title.  The title is presented at the top of
        the menu.

MENU LABEL label
----------------

        (Only valid after a LABEL statement.)
        Changes the label displayed for a specific entry.  This allows
        you to have a label that isn't suitable for the command line,
        for example::

          # Soft Cap Linux
          LABEL softcap
                MENU LABEL Soft Cap ^Linux 9.6.36
                KERNEL softcap-9.6.36.bzi
                APPEND whatever
  
          # A very dense operating system
          LABEL brick
                MENU LABEL ^Windows CE/ME/NT
                KERNEL chain.c32
                APPEND hd0 2

        The ^ symbol in a MENU LABEL statement defines a hotkey.
        The hotkey will be highlighted in the menu and will move the
        menu cursor immediately to that entry.

        Reusing hotkeys is disallowed, subsequent entries will not be
        highlighted, and will not work.

        Keep in mind that the LABELs, not MENU LABELs, must be unique,
        or odd things will happen to the command-line.

MENU HIDE
---------

        (Only valid after a LABEL statement.)
        Suppresses a particular LABEL entry from the menu.


MENU DEFAULT
------------

        (Only valid after a LABEL statement.)
        Indicates that this entry should be the default.  If no
        default is specified, use the first one.


MENU PASSWD passwd
------------------

        (Only valid after a LABEL statement.)
        Sets a password on this menu entry.  "passwd" can be either a
        cleartext password or a SHA-1 encrypted password; use the
        included Perl script "sha1pass" to encrypt passwords.
        (Obviously, if you don't encrypt your passwords they will not
        be very secure at all.)

        If you are using passwords, you want to make sure you also use
        the settings "NOESCAPE 1", "PROMPT 0", and either set
        "ALLOWOPTIONS 0" or use a master password (see below.)

        If passwd is an empty string, this menu entry can only be
        unlocked with the master password.


MENU MASTER PASSWD passwd
-------------------------

        Sets a master password.  This password can be used to boot any
        menu entry, and is required for the [Tab] and [Esc] keys to
        work.


MENU BACKGROUND filename
------------------------

        For vesamenu.c32, sets the background image.  The image should
        be 640x480 pixels and either in PNG or JPEG format.


MENU INCLUDE filename
---------------------

        Include the contents of the configuration file filename at
        this point.  Keep in mind that the included data is only seen
        by the menu system; the core syslinux code does not parse this
        command, so any labels defined in it are unavailable.


MENU COLOR element ansi foreground background shadow
----------------------------------------------------

        Sets the color of element "element" to the specified color
        sequence::

          screen          Rest of the screen
          border          Border area
          title           Title bar
          unsel           Unselected menu item
          hotkey          Unselected hotkey
          sel             Selection bar
          hotsel          Selected hotkey
          scrollbar       Scroll bar
          tabmsg          Press [Tab] message
          cmdmark         Command line marker
          cmdline         Command line
          pwdborder       Password box border
          pwdheader       Password box header
          pwdentry        Password box contents
          timeout_msg     Timeout message
          timeout         Timeout counter


        "ansi" is a sequence of semicolon-separated ECMA-48 Set
        Graphics Rendition (<ESC>[m) sequences::

          0     reset all attributes to their defaults
          1     set bold
          4     set underscore (simulated with color on a color display)
          5     set blink
          7     set reverse video
          22    set normal intensity
          24    underline off
          25    blink off
          27    reverse video off
          30    set black foreground
          31    set red foreground
          32    set green foreground
          33    set brown foreground
          34    set blue foreground
          35    set magenta foreground
          36    set cyan foreground
          37    set white foreground
          38    set underscore on, set default foreground color
          39    set underscore off, set default foreground color
          40    set black background
          41    set red background
          42    set green background
          43    set brown background
          44    set blue background
          45    set magenta background
          46    set cyan background
          47    set white background
          49    set default background color

        These are used (a) in text mode, and (b) on the serial
        console.


        "foreground" and "background" are color codes in #AARRGGBB
        notation, where AA RR GG BB are hexadecimal digits for alpha
        (opacity), red, green and blue, respectively.  #00000000
        represents fully transparent, and #ffffffff represents opaque
        white.


        "shadow" controls the handling of the graphical console text
        shadow.  Permitted values are "none" (no shadowing), "std" or
        "standard" (standard shadowing - foreground pixels are
        raised), "all" (both background and foreground raised), and
        "rev" or "reverse" (background pixels are raised.)


        If any field is set to "*" or omitted (at the end of the line)
        then that field is left unchanged.


        The current defaults are::

          menu color screen       37;40      #80ffffff #00000000 std
          menu color border       30;44      #40000000 #00000000 std
          menu color title        1;36;44    #c00090f0 #00000000 std
          menu color unsel        37;44      #90ffffff #00000000 std
          menu color hotkey       1;37;44    #ffffffff #00000000 std
          menu color sel          7;37;40    #e0000000 #20ff8000 all
          menu color hotsel       1;7;37;40  #e0400000 #20ff8000 all
          menu color scrollbar    30;44      #40000000 #00000000 std
          menu color tabmsg       31;40      #90ffff00 #00000000 std
          menu color cmdmark      1;36;40    #c000ffff #00000000 std
          menu color cmdline      37;40      #c0ffffff #00000000 std
          menu color pwdborder    30;47      #80ffffff #20ffffff std
          menu color pwdheader    31;47      #80ff8080 #20ffffff std
          menu color pwdentry     30;47      #80ffffff #20ffffff std
          menu color timeout_msg  37;40      #80ffffff #00000000 std
          menu color timeout      1;37;40    #c0ffffff #00000000 std

  ::

    MENU WIDTH 80
    MENU MARGIN 10
    MENU PASSWORDMARGIN 3
    MENU ROWS 12
    MENU TABMSGROW 18
    MENU CMDLINEROW 18
    MENU ENDROW 24
    MENU PASSWORDROW 11
    MENU TIMEOUTROW 20

These options control the layout of the menu on the screen.
The values above are the defaults.

The menu system honours the TIMEOUT command; if TIMEOUT is specified
it will execute the ONTIMEOUT command if one exists, otherwise it will
pick the default menu option.

Normally, the user can press [Tab] to edit the menu entry, and [Esc]
to return to the SYSLINUX command line.  However, if the configuration
file specifies ALLOWOPTIONS 0, these keys will be disabled, and if
MENU MASTER PASSWD is set, they require the master password.

The simple menu system supports serial console, using the normal
SERIAL directive.  However, it can be quite slow over a slow serial
link; you probably want to set your baudrate to 38400 or higher if
possible.  It requires a Linux/VT220/ANSI-compatible terminal on the
other end.


USING AN ALTERNATE CONFIGURATION FILE
-------------------------------------

It is also possible to load a secondary configuration file, to get to
another menu.  To do that, invoke menu.c32 with the name of the
secondary configuration file::

  LABEL othermenu
        MENU LABEL Another Menu
        KERNEL menu.c32
        APPEND othermenu.conf

If you specify more than one file, they will all be read, in the order
specified.  The dummy filename ~ (tilde) is replaced with the filename
of the main configuration file::

  # The file graphics.conf contains common color and layout commands for
  # all menus.
  LABEL othermenu
        MENU LABEL Another Menu
        KERNEL vesamenu.c32
        APPEND graphics.conf othermenu.conf

  # Return to the main menu
  LABEL mainmenu
        MENU LABEL Return to Main Menu
        KERNEL vesamenu.c32
        APPEND graphics.conf ~

To use the secondary configuration file, simply make sure it is in the
appropriate location for your boot medium (the same directory as the
configuration file for SYSLINUX, EXTLINUX and ISOLINUX, and the same
directory as pxelinux.0 for PXELINUX).
It should *not* be located in the pxelinux.cfg/ subdirectory.

See also the MENU INCLUDE directive above.
