.. _System_administration:

===================================
Linux cluster system administration
===================================

Some topics of relevance for cluster system administrators is described on this page.
The **Niflheim** project required the use a number of techniques, and we document below
in some detail how we install and manage a cluster of some 800 nodes.

.. Contents::

System and network setup
========================

Node's BIOS boot order
----------------------

We recommend to configure the client node BIOS to a *boot order* similar to the following:

  1. USB devices
  2. CD-ROM
  3. Network (PXE)
  4. Hard disk

The first 2 items enable you to perform node diagnostics and configuration.
The Network (PXE) option will be the normal boot mode.
The final hard disk option is only used if all the preceding ones fail (for troubleshooting, only).

Please consult this page for detailed information about PXE network booting:

    * :ref:`PXE network booting <PXE-booting>`

DHCP setup
----------

The DHCP server daemon should be configured correctly for booting and installation of client nodes, 
please see the `SYSLINUX documentation <http://syslinux.zytor.com/pxe.php#dhcp>`_. 
Our configuration in ``/etc/dhcpd.conf`` is (partially)::

  # option-140 is the IP address of your SystemImager image server
  option option-140 code 140 = text;
  filename "pxelinux.0";          # For i386
  #filename "elilo.efi";          # For ia64
  subnet 10.1.0.0 netmask 255.255.0.0 {
    option domain-name "dcsc.fysik.dtu.dk";
    option domain-name-servers 10.1.128.5;
    option routers 10.1.128.5;            # Fake default gateway
    option ntp-servers    10.1.128.5;     # NTP server
    option time-offset    3600;           # Middle European Time (MET), in seconds
    option option-140 "10.1.128.5";
    next-server 10.1.128.5;               # next-server is your network boot server
    option log-servers 10.1.128.5;        # log-servers

    host n001 { hardware ethernet 00:11:25:c4:8e:82; fixed-address n001.dcsc.fysik.dtu.dk;}
    # Lots of additional hosts...
    }

Of course, you have to change IP addresses and domain-names for your own cluster. 

If your cluster is on a `private Internet <http://en.wikipedia.org/wiki/Private_network>`_ 
(such as the 10.x.y.z net) and your DHCP server has multiple network interfaces, 
you must make sure that your DHCP server doesn't offer DHCP-service to the non-cluster networks (a sure way to find a lot of angry colleagues before long :-). 
Edit the Redhat configuration file ``/etc/sysconfig/dhcpd`` to contain::

   DHCPDARGS=eth1 

(where ``eth1`` is the interface connected to your cluster) and restart the *dhcpd daemon* (``service dhcpd restart``).

Registering client node MAC addresses
-------------------------------------

The client nodes' Ethernet *MAC addresses* must be configured into the ``/etc/dhcpd.conf`` file. 
Alternatively, you can let the DHCP server hand out IP addresses freely, 
but then you may loose the ability to identify nodes physically from their IP addresses. 

We recommend to use the statically assigned IP addresses in the ``/etc/dhcpd.conf``. 
This can be achieved by the following procedure:

   1. Configure the DHCP server without the clients' MAC-addresses and use the ``deny unknown-clients``
      DHCP option in ``/etc/dhcpd.conf``.
   2. Connect the client nodes to the network and turn them on one by one.
      In the NIFLHEIM installation we did this as part of the setup process, at the same time as we customized the BIOS settings.
   3. For all the client node names, copy from the server's ``/var/log/messages`` file the client's Ethernet MAC-address.
      Label each client node with an adhesive label containing the correct node name.
   4. In a file with a list of client node names you add the MAC-address to the node's line in the file.
   5. When all nodes have been registered, use a simple awk-script or similar to convert this list into lines for
      the ``/etc/dhcpd.conf`` file, such as this one::

        host n001 { hardware ethernet 00:08:02:8e:05:f2; fixed-address n001.dcsc.fysik.dtu.dk;}


Automated network installation
------------------------------

Having to watch the installation process and finally change the client nodes' BIOS setup is cumbersome 
when you have more than a dozen or two client nodes. 

After having tested the network installation process manually as described above, 
you can automate the process completely using the pxeconfig_ toolkit written by Bas van der Vlies. 
Now a client node installation is as simple as configuring on the central server whether a node should perform a network installation or simply boot from hard disk: 
When the node is turned on, it all happens automatically with no operator intervention at all ! 
The BIOS boot order must still have PXE/network before the hard disk.

Please see the following page for information about the pxeconfig toolkit:

* :ref:`Automated_network_installation_with_pxeconfig`.

.. _pxeconfig: https://gitlab.com/surfsara/pxeconfig

EL Linux installation with Kickstart
----------------------------------------

RHEL_ Linux and EL clones such as AlmaLinux_ or RockyLinux_, as well as Fedora_, can be installed using Kickstart_.
See a general description from the Fedora page:

* Many system administrators would prefer to use an automated installation method to install Fedora_ or Red Hat Enterprise Linux on their machines.
  To answer this need, Red Hat created the Kickstart_ installation method.
  Using Kickstart_, a system administrator can create a single file containing the answers to all the questions that would normally be asked during a typical installation.

* Kickstart_ files can be kept on a server system and read by individual computers during the installation.
  This installation method can support the use of a single Kickstart_file_ to install Fedora_ or Red Hat Enterprise Linux on multiple machines,
  making it ideal for network and system administrators.

There is documentation of the Kickstart_file_ syntax.

A Kickstart_ installation can be made using PXE network booting, see PXE-booting_ and PXE_and_UEFI_ network booting.

.. _Kickstart: https://pykickstart.readthedocs.io/en/latest/kickstart-docs.html#chapter-1-introduction
.. _Kickstart_file: https://anaconda-installer.readthedocs.io/en/latest/kickstart.html
.. _RHEL: https://en.wikipedia.org/wiki/Red_Hat_Enterprise_Linux
.. _AlmaLinux: https://almalinux.org/
.. _RockyLinux: https://www.rockylinux.org
.. _Fedora: https://fedoraproject.org/
.. _PXE-booting: https://wiki.fysik.dtu.dk/ITwiki/PXE-booting
.. _PXE_and_UEFI: https://wiki.fysik.dtu.dk/ITwiki/PXE_and_UEFI

Networking considerations
=========================

Using multiple network adapters
-------------------------------

Some machines, especially servers, are equipped with dual Ethernet ports on the motherboard. 
In order to use both ports for increased bandwidth and/or redundancy, Linux must be configured appropriately.

We have a page about :ref:`MultipleEthernetCards`.

SSH setup
---------

In order to run parallel codes we use the MPI message-passing interface 
a prerequisite is the ability for all users to start processes on remote nodes without having to enter their password. 
This is accomplished using the Secure Shell (SSH) remote login in combination with a globally available ``/etc/hosts.equiv`` 
file that controls the way that nodes permit password-less logins.

The way we have chosen to configure SSH within the NIFLHEIM cluster is to clone the SystemImager Golden Client's 
SSH configuration files in the ``/etc/ssh`` directory on all nodes, meaning that all nodes have identical SSH keys. 
In addition, the SSH public-key database file ``ssh_known_hosts`` contains a single line for all cluster nodes, 
where all nodes have identical public keys. 

When you have determined the Golden Client's public key, 
you can automatically generate the ``ssh_known_hosts`` file using our simple C-code clusterlabel.c__
(define the SSH_KEY constant in the code using your own public key). 
Place the resulting ``ssh_known_hosts`` file in all the nodes' ``/etc/ssh`` directory, 
which is easily accomplished on the Golden Client first, 
before cloning the other nodes (alternatively, the file can be distributed later).

__ attachment:attachments/clusterlabel.c

The *root superuser* is a special case, since ``/etc/hosts.equiv`` is ignored for this user.
The best method for password-less root logins is to create public keys on the
(few) central servers that you wish to grant password-less root login to all cluster nodes.
We have made a useful script `authorized_keys <ftp://ftp.fysik.dtu.dk/pub/SystemImager/authorized_keys>`_
for this purpose, useable for any user including *root*.
In the case of the root user, 
the contents of the file ``/root/.ssh/id_rsa.pub`` is appended to ``/root/.ssh/authorized_keys``,
and this file must be distributed onto all client nodes, thereby enabling password-less root access.

In an alternative method, 
for all client nodes you must have the ``/root/.shosts`` file created with a line 
for each of the central servers.

Kernel ARP cache
----------------

If the number of network devices (cluster nodes plus switches etc.) approaches or exceeds 512, 
you must consider the Linux kernel's limited dynamic ARP-cache size. 
Please read the man-page *man 7 arp* about the kernel's ARP-cache.

ARP (Address Resolution Protocol) is the kernel's mapping between IP-addresses (such as 10.1.2.3) and Ethernet MAC-addresses 
(such as 00:08:02:8E:05:F2). 
If the soft maximum number of entries to keep in the ARP cache, *gc_thresh2=512*, is exceeded, 
the kernel will try to remove ARP-cache entries by a garbage collection process. 
This is going to hit you in terms of sporadic loss of connectivitiy between pairs of nodes. 
No garbage collection will take place if the ARP-cache has fewer than *gc_thresh1=128* entries, 
so you should be safe if your network is smaller than this number.

The best solution to this ARP-cache trashing problem is to increase the kernel's ARP-cache garbage collection (gc) 
parameters by adding these lines to ``/etc/sysctl.conf``::

  # Don't allow the arp table to become bigger than this
  net.ipv4.neigh.default.gc_thresh3 = 4096
  # Tell the gc when to become aggressive with arp table cleaning.
  net.ipv4.neigh.default.gc_thresh2 = 2048
  # Adjust where the gc will leave arp table alone
  net.ipv4.neigh.default.gc_thresh1 = 1024
  # Adjust to arp table gc to clean-up more often
  net.ipv4.neigh.default.gc_interval = 3600
  # ARP cache entry timeout
  net.ipv4.neigh.default.gc_stale_time = 3600

Then run ``/sbin/sysctl -p`` to reread this configuration file.

Another solution, although more cumbersome in daily adminsitration, 
is to create a static ARP database, which is customarily kept in the file ``/etc/ethers``. 
It may look like this (see *man 5 ethers*)::

  00:08:02:8E:05:F2 n001
  00:08:02:89:9E:5E n002
  00:08:02:89:62:E6 n003
  ...

This file can easily be created from the DHCP configuration file ``/etc/dhcpd.conf``
by extracting hostnames and MAC-address fields (using ``awk``, for example). 
In order to add this information to the permanent ARP-cache, run the command ``arp -f /etc/ethers``.

In order to do this at boot time, the Redhat Linux file ``/etc/rc.local`` can be used. 
Add these lines to ``/etc/rc.local``::

  # Load the static ARP cache from /etc/ethers, if present
  if test -f /etc/ethers then
    /sbin/arp -f /etc/ethers
  fi 

This configuration should be performed on all nodes and servers in the cluster, 
as well as any other network device that can be be configured in this way.

It doesn't hurt to use this configuration also on clusters with 128-512 network devices, 
since the dynamic ARP-cache will then have less work to do. 
However, you must maintain a consistent ``/etc/ethers`` as compared to the nodes defined in ``/etc/dhcpd.conf``, 
and you must run the arp command every time the ``/etc/ethers`` file is modified
(for example, when a node's network card is replaced).
