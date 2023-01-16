.. _PXE-booting:

PXE network booting
===================

.. Contents::

This page described BIOS_ PXE_ booting.
See also the page on PXE_and_UEFI_ network booting,
and the section Automated_network_installation_with_pxeconfig_ below.

.. _BIOS: https://en.wikipedia.org/wiki/BIOS
.. _PXE: http://en.wikipedia.org/wiki/Preboot_Execution_Environment
.. _PXE_and_UEFI: https://wiki.fysik.dtu.dk/ITwiki/PXE_and_UEFI

`PXE network booting <http://en.wikipedia.org/wiki/Preboot_Execution_Environment>`_
from Linux servers uses the `SYSLINUX <http://syslinux.zytor.com/>`_ and 
`PXELINUX <http://syslinux.zytor.com/pxe.php>`_ utilities
(see also this `Wikipedia article <http://en.wikipedia.org/wiki/Syslinux>`_).
Read the documentation on these pages to get an understanding of the process.
The `SYSLINUX source code <http://www.kernel.org/pub/linux/utils/boot/syslinux/>`_ has
additional very useful documentation which we have copied here: :ref:`syslinux.doc` and :ref:`pxelinux.doc`.

There is a `SYSLINUX mailing list <http://www.zytor.com/mailman/listinfo/syslinux>`_
as well as a `SYSLINUX Wiki <http://syslinux.zytor.com/wiki/index.php/Main_Page>`_.

.. toctree::
   :maxdepth: 1
   :caption: SYSLINUX documentation

   README.menu
   pxelinux.doc
   syslinux.doc

Boot process summary
--------------------

When a client computer performs a PXE_ network boot, the Linux DHCP server assigns the client an IP-address 
and further information including a TFTP-server address (DHCP ``next-server`` option)
and a boot image file name ``pxelinux.0`` (DHCP ``filename`` option).
The client retrieves the file ``pxelinux.0`` from the TFTP server and executes it.


The ``pxelinux.0`` PXELINUX boot image then attempts to download configurations files from the TFTP server
in the boot process `explained here <pxelinux.doc#how-to-configure-pxelinux>`_.
To summarize: The PXE/network client will use TFTP to download a PXELINUX configuration file
from the server's ``/tftpboot/pxelinux.cfg/`` directory
whose name is usually either:

 1) the client's hexadecimally encoded IP-address (such as ``0A018219``), or 
 2) the file named ``default``.

With `newer versions of SYSLINUX <http://www.kernel.org/pub/linux/utils/boot/syslinux/>`_
it is also possible to PXE-boot into the
`SYSLINUX menu systems <http://syslinux.zytor.com/menu.php>`_ where many booting options can be configured.
This is a very flexible way to boot, for example, diskette images with BIOS_ upgrades, hardware testers, or
SystemImager installation, etc.

Installing the SYSLINUX tools
-----------------------------

Although your Linux machine may already have some of the SYSLINUX tools installed, 
it is recommended that you get the latest `version of SYSLINUX <http://www.kernel.org/pub/linux/utils/boot/syslinux/>`_.

Unpack the tar-ball and copy the following SYSLINUX files to the ``/tftpboot`` directory on the DHCP/TFTP server::

  tar xzvf syslinux-4.02.tar.gz
  cd syslinux-4.02
  cp core/pxelinux.0 memdisk/memdisk com32/modules/chain.c32 com32/menu/menu.c32 /tftpboot/

(version 4.02 is used in this example). 
Additional com32/\*/\*.c32 modules might be needed if further features from SYSLINUX will be used. 

For the older SYSLINUX version 3.x the locations of files differ a bit, so do::

  tar xzvf syslinux-3.51.tar.gz
  cd syslinux-3.51
  cp pxelinux.0 memdisk/memdisk com32/modules/*.c32 /tftpboot/


If for some reason you must rebuild SYSLINUX, first verify that you have the ``nasm`` package installed::

  rpm -q nasm

Then build SYSLINUX in the top-level directory by doing simply ``make``.  
See also the file ``distrib.doc`` for further details.

The default file
----------------

You must also create a default PXE boot file ``/tftpboot/pxelinux.cfg/default`` instructing the
node how to boot in case there is no hexadecimally encoded IP-address file.
Probably the most sensible default boot method is *local hard disk* which is configured as follows:

We assume that you have installed the SYSLINUX tools as shown above, in particular the `chain.c32 <http://syslinux.zytor.com/wiki/index.php/Comboot/chain.c32>`_ tool.
Then create the file named ``default`` containing these lines::

  default harddisk
  label harddisk
    kernel chain.c32
    append hd0

For comparison, in many places you will find the following recipe for the default file::

  default harddisk
  label harddisk
    localboot 0

This recipe can be error-prone and actually means *boot from the next device in the BIOS boot order*,
rather than booting from the hard disk as you would be led to believe.
For more information read this `article from the SYSLINUX mailing list <http://syslinux.zytor.com/archives/2006-August/007131.html>`_
(look at the bottom of the article).

Etherboot/gPXE
--------------

Something to consider for the future is the `Etherboot/gPXE project <http://www.etherboot.org/wiki/start>`_
which permits a larger set of boot media, includign also HTTP-servers.
For PXE-booting into gPXE see `PXE chainloading <http://www.etherboot.org/wiki/pxechaining>`_.

Linux and Windows deployment (WDS)
----------------------------------

A Microsoft *Windows Deployment Service* WDS_ takes over PXE_ booting of any PXE_ clients it has configured, thus the clients will ignore the Linux PXE_ boot server.

.. _WDS: http://en.wikipedia.org/wiki/Windows_Deployment_Services

For a solution, see `Linux and Windows deployment <http://www.syslinux.org/wiki/index.php/Linux_and_Windows_deployment>`_:

* This covers the setup and deployment of a PXE_ boot solution consisting of 2 pxe servers and one dhcp server. 
  The 2 PXE_ servers are linux and windows - the former running pxelinux and tftp and the latter one running WDS (Windows Deployment Services), with a linux server providing DHCP services.

See also `Peaceful Coexistence: WDS and Linux PXE Servers <http://www.vcritical.com/2011/06/peaceful-coexistence-wds-and-linux-pxe-servers/>`_:

* As it turns out, thanks to the lesser-known pxechain utility, it is possible to seamlessly jump from one PXE host to another.
  With a few tweaks to your WDS server, you can continue to use it for Windows OS installs and bounce over to a Linux host for Linux, ESXi, or rescue-CD purposes.

SYSLINUX Menu systems
=====================

With `newer versions of SYSLINUX <http://www.kernel.org/pub/linux/utils/boot/syslinux/>`_
it is possible to PXE-boot into the
`SYSLINUX menu systems <http://syslinux.zytor.com/menu.php>`_ where many booting options can be configured.
This is a very flexible way to boot, for example, diskette images with BIOS_ upgrades, hardware testers, or
SystemImager installation, etc.

Please consult the :ref:`README.menu` from the  SYSLINUX source.

One must first install SYSLINUX files to ``/tftpboot`` on the DHCP/TFTP server as shown in `Installing the SYSLINUX tools`_.

Secondly, for each client machine that should use the SYSLINUX menu systems a hexadecimally encoded IP-address file
must be created in ``/tftpboot/pxelinux.cfg/``, pointing to the menu configuration file.
This can conveniently be done with the ``pxeconfig`` command discussed below.

A simple ``default.menu`` SYSLINUX menu file in ``/tftpboot/pxelinux.cfg/`` is::

  DEFAULT menu.c32
  PROMPT 0
  
  MENU TITLE Menu from TFTP server

  label harddisk
        menu label Boot from local harddisk
        kernel chain.c32
        append hd0

  label memtest86
        menu label Memtest86 memory tester
        kernel memtest86

  label BIOS
        menu label BIOS upgrade ibm_fw_bios_c0e132a
        kernel memdisk
        append initrd=ibm_fw_bios_c0e132a_anyos_i386.img

  label CentOS-4.4-i386
        menu label Installation of CentOS-4.4-i386, no kickstart
        kernel CentOS-4.4/vmlinuz
        append load_ramdisk=1 initrd=CentOS-4.4/initrd.img network

This configuration will display a menu with 4 items, each performing a different task
as described in the ``menu label`` lines.

Password protection of PXELINUX menu items
------------------------------------------

It is possible to password protect a PXELINUX menu item in recent versions of PXELINUX, see http://www.syslinux.org/wiki/index.php/Menu#MENU_PASSWD.
For example, a menu item may have a line::

  menu passwd <password-hash>

To generate the MD5 or SHA1 password hash, make sure you have a recent version of syslinux, or download the code from http://www.kernel.org/pub/linux/utils/boot/syslinux/.
Locate the scripts ``sha1pass`` and ``md5pass`` (subdirectory ``utils/`` in the source).
Also, install this prerequisite::

  yum install perl-Crypt-PasswdMD5

Then you can execute ``sha1pass`` or ``md5pass`` to generate password hashes.

Alternatively, you can use the command ``/sbin/grub-md5-crypt`` (MD5 passwords only), or find some web-based tools.

Hardware Detection Tool (HDT)
-----------------------------

SYSLINUX version 4 contains a `HDT - Hardware Detection Tool <http://hdt-project.org/>`_.
HDT (stands for Hardware Detection Tool) is a Syslinux com32 module that displays low-level information for any x86 compatible system. 
It provides both a command line interface and a semi-graphical menu mode for browsing. 

To enable HDT install the ``hdt.c32`` module from the SYSLINUX source::

  cd syslinux-4.02
  cp com32/hdt/hdt.c32 /tftpboot/

Then add a PXE menu section to the ``default.menu`` file::

  label hdt
        menu label HDT - Hardware Detection Tool
        COM32 hdt.c32

Boot images from Ultimatebootcd.com
-----------------------------------

If you would like a very complete set of tools to boot from the PXE menus, you may want to take a look at the
`Ultimate Boot CD <http://www.ultimatebootcd.com/>`_ project.
You can `download an ISO image <http://www.ultimatebootcd.com/download.html>`_ for burning your own CD.

While you could boot the Ultimate Boot CD directly, you could also extract the utilities from the CD and put them into your PXE menus.
This is easier than downloading the tools yourself from very many different places.

You can copy the Ultimate Boot CD tools from the CD, or from the ISO image if you mount it first onto ``/mnt``::

  root# mount -o loop /some-path/ubcd503.iso /mnt

You can very simply enable the complete Ultimate Boot CD tools in a PXE environment by copying the entire CD structure to ``/tftpboot/`` on your TFTP server::

  root# cp -rp /mnt/ubcd /tftpboot/

and add this configuration to the SYSLINUX menu file::

  label UBCMenu
        menu label Ultimate Boot CD menu
        kernel menu.c32
        append ubcd/menus/syslinux/main.cfg

Alternatively, with SYSLINUX version 4 and above you can PXE-boot the 300MB UBC ISO-image directly (loading the TFTP server much more).
The loading of ISO CD images with SYSLINUX is described in http://syslinux.zytor.com/wiki/index.php/MEMDISK#ISO_images.
The SYSLINUX menu file could have a section like::

  label UBCMenu-iso
        menu label Ultimate Boot CD (300 MB ISO file)
        linux memdisk
        initrd ubcd503.iso
        append iso

Commandline key strokes
-----------------------

The command line prompt supports the following keystrokes (see :ref:`syslinux.doc`)::

  <Enter>         boot specified command line
  <BackSpace>     erase one character
  <Ctrl-U>        erase the whole line
  <Ctrl-V>        display the current SYSLINUX version
  <Ctrl-W>        erase one word
  <Ctrl-X>        force text mode
  <F1>..<F10>     help screens (if configured)
  <Ctrl-F><digit> equivalent to F1..F10
  <Ctrl-C>        interrupt boot in progress
  <Esc>           interrupt boot in progress


.. _Automated_network_installation_with_pxeconfig:

Automated network installation with pxeconfig
=============================================

You can automate the PXE/network booting process completely using the pxeconfig_toolkit_ written by Bas van der Vlies.
The pxeconfig toolkit manipulates configuration files in the server's ``/tftpboot/pxelinux.cfg/`` directory,
namely the client's hexadecimally encoded IP-address, for example::

  0A018219 => 10.1.130.25

Hint: Use ``gethostip`` from the ``syslinux`` package to convert hostnames and IP-addresses to hexadecimal, for example::

  $ gethostip -f s001
  s001.(domainname) 10.2.130.21 0A028215
  $ gethostip -x s001
  0A028215

Download the pxeconfig_toolkit_ and read the 
`pxeconfig installation instructions <https://gitlab.com/surfsara/pxeconfig/-/blob/master/INSTALL>`_. 

.. _pxeconfig_toolkit: https://gitlab.com/surfsara/pxeconfig

Ansible setup for pxeconfig
---------------------------

For CentOS/RHEL 7 we have written an Ansible_ role to automatically install and start the pxeconfig service:

* https://github.com/OleHolmNielsen/ansible/tree/master/roles/pxeconfigd

Download the role files to your Ansible_ setup and use this role in your Ansible_ playbooks by::

  - pxeconfigd

.. _Ansible: https://www.ansible.com/

The pxeconfig command
---------------------

To use pxeconfig you should create any number of PXELINUX configuration files named ``default.<something>``
which contain different PXELINUX commands that perform the desired actions, for example,
BIOS_ updates, firmware updates, hardware diagnostics, or network installation.

Use the ``pxeconfig`` command to configure those client nodes that you wish to install 
(the remaining nodes will simply boot from their hard disk).
An example is::

  # pxeconfig n003
  Which pxe config file must we use: ?
  1 : default.memdisk_ibm_s50_bios
  2 : default.memdisk_326m_bmc_fw118
  3 : default.memtest86
  4 : default.node_install.s50
  5 : default.harddisk.BAK
  6 : default.node_install.thul
  7 : default.node_install.ibm326
  Select a number: 7


The ``pxeconfig`` command creates soft-links in the ``/tftpboot/pxelinux.cfg/`` directory named as 
the hexadecimally encoded IP-address of the clients, and these links will point to one of the files ``default.*``. 
As designed, the PXE network booting process will download the file given by the hexadecimal IP-address, 
and hence network installation of the node will take place.

The hexls command
-----------------

To list the soft links created, use the *pxeconfig* tool ``hexls`` and look for the IP-addresses and/or hostnames.  
A sample output is::

  # hexls /tftpboot/pxelinux.cfg/ | sort
  0A018103 => 10.1.129.3 => n003.dcsc.fysik.dtu.dk -> default.node_install.ibm326
  0A028248 => 10.2.130.72 => t072.dcsc.fysik.dtu.dk -> default.node_install.s50
  0A02826B => 10.2.130.107 => t107.dcsc.fysik.dtu.dk -> default.node_install.s50
  0A02833D => 10.2.131.61 => u061.dcsc.fysik.dtu.dk -> default.node_install.s50
  default
  default.harddisk
  default.harddisk.BAK
  default.memdisk_326m_bmc_fw118
  default.memdisk_ibm_s50_bios
  default.memtest86
  default.node_install.ibm326
  default.node_install.s50
  default.node_install.thul

The pxeconfigd daemon
---------------------

The second part of the pxeconfig_toolkit_ is the *pxeconfigd daemon*.
This tool is presumably only useful when doing network installation using https://www.ansible.com/
`SystemImager <System_administration#cloning-of-nodes-with-systemimager>`_.

The *pxeconfigd daemon* will remove the hexadecimally encoded IP-address soft-link on the server when contacted by the client node. 
In order for this to happen, you must go to the image server's ``/var/lib/systemimager/scripts/post-install`` directory 
and create the file ``30all.pxeconfig``::

  #!/bin/sh
  # To be used with the pxeconfig tool.
  # Remove the <hex_ipaddr> file from the pxelinux.cfg directory so the client will boot from disk.
  # Get pxeconfig from ftp://ftp.surfsara.nl/pub/outgoing/pxeconfig.tar.gz
  # Get the Systemimager variables
  . /tmp/post-install/variables.txt
  telnet $IMAGESERVER 6611
  sleep 1
  exit 0

When this script is executed on the node by SystemImager in the post-install phase,
the ``telnet`` command connects to the *pxeconfigd daemon* on the image server,
and this daemon will remove the hexadecimally encoded IP-address soft-link in ``/tftpboot/pxelinux.cfg/``
corresponding to the client IP-address which did the ``telnet`` connection.
