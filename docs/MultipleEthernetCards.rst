.. _MultipleEthernetCards:

=============================
Using Multiple Ethernet Cards
=============================

.. Contents::

Linux port bonding
==================

Some machines, especially servers, are equipped with dual Ethernet ports on the motherboard.  
In order to use both ports for increased bandwidth and/or redundancy, Linux must be configured appropriately.

You should consult this very nice `overview of the Linux bonding driver <http://linux-net.osdl.org/index.php/Bonding>`_
and the `Linux Ethernet Bonding Driver HOWTO <http://www.mjmwired.net/kernel/Documentation/networking/bonding.txt>`_.
The *kernel-doc* RPM also documents port bonding in the file ``/usr/share/doc/kernel-doc-*/Documentation/networking/bonding.txt``
or in http://www.kernel.org/doc/Documentation/networking/bonding.txt.

.. _bonding_Module_Directives: http://www.centos.org/docs/5/html/5.2/Deployment_Guide/s3-modules-bonding-directives.html

Loading the bonding kernel module
=================================

Read the Channel_Bonding_Interfaces_ manual for the parameter values.
Apparently it is preferred to enter bonding parameters in the file ``/etc/sysconfig/network-scripts/ifcfg-bond0``.

For RHEL6 read `Using Channel Bonding <http://docs.redhat.com/docs/en-US/Red_Hat_Enterprise_Linux/6/html/Deployment_Guide/sec-Using_Channel_Bonding.html>`_.

Our current instructions are:
Add this line to ``/etc/modprobe.conf`` (*not* ``/etc/modules.conf``)::

  alias bond0 bonding
  options bond0 mode=6 miimon=100 updelay=200

The ``mode=6`` refers to::

  Sets an Active Load Balancing (ALB) policy for fault tolerance and load balancing.
  Includes transmit and receive load balancing for IPV4 traffic.
  Receive load balancing is achieved through ARP negotiation.

The ``miimon=100`` refers to::

	Specifies the MII link monitoring frequency in milliseconds.
	This determines how often the link state of each slave is
	inspected for link failures.  A value of zero disables MII
	link monitoring.  A value of 100 is a good starting point.
	The use_carrier option, below, affects how the link state is
	determined.  See the High Availability section for additional
	information.  The default value is 0.


The ``updelay=200`` refers to:: 

	Specifies the time, in milliseconds, to wait before enabling a
	slave after a link recovery has been detected.  This option is
	only valid for the miimon link monitor.  The updelay value
	should be a multiple of the miimon value; if not, it will be
	rounded down to the nearest multiple.  The default value is 0.

If you do *not* set the ``updelay`` parameter, the syslog may show this warning::

  kernel: bonding: In ALB mode you might experience client disconnections upon reconnection of a link if the bonding module updelay parameter (0 msec) is incompatible with the forwarding delay time of the switch

Switch forward delay is related to the Spanning Tree Protocol (if it's configured), see 
`Spanning Tree Protocol Timers <http://www.cisco.com/en/US/tech/tk389/tk621/technologies_tech_note09186a0080094954.shtml>`_::

      forward delay: The forward delay is the time that is spent in the listening and learning state.

Restart network services
========================

At this stage the network should be restarted by ``service network restart``,
or the system should be rebooted, 
in order to activate the ``bond0`` device in stead of the normal ``eth0`` device.

Port bonding troubleshooting
============================

No DHCP response for the bond0 device
-------------------------------------

If you've set up the *bond0* device for DHCP by ``BOOTPROTO=dhcp`` and you don't get a DHCP response from the server,
then it may be because *bond0* uses the **first Ethernet device** (usually *eth0*) for DHCP.
If your DHCP server is configured with the Ethernet MAC-address of another device (for example, *eth1*), then DHCP will fail.

This scenario happens when the Linux kernel has swapped around the Ethernet devices eth0 and eth1 opposite to what the hardware thinks.
Check this by::

  ifconfig -a

to see the MAC-addresses of the network interfaces.

Getting all interface MAC addresses for DHCP
............................................

The correct solution to the problem of the *bond0* bonding device using a random network interface for its DHCP client is adding **all MAC-addresses** to your DHCP server.
In this way the DHCP server will respond correctly no matter *which* of the bonding slave interfaces happen to be used by the DHCP client.

Of course you have to use the same DNS name (or IP address) for all of the bonding interface MAC addresses.
An example from a DHCP configuration file is::

  host a069-enp5s0f0 { hardware ethernet 00:24:81:f8:a9:d5; fixed-address a069.mydomain;}
  host a069-enp5s0f1 { hardware ethernet 00:24:81:f8:a9:d4; fixed-address a069.mydomain;}

Please note the the DHCP host field must be unique, so the interface named has been added.

To learn the interface MAC-addresses you may use this command::

  /sbin/ip -4 -o link show

We offer a script getmaclist__ using ClusterShell_ for gathering this information.

__ attachment:attachments/getmaclist
.. _pdsh: https://linux.die.net/man/1/pdsh
.. _ClusterShell: https://clustershell.readthedocs.io/en/latest/intro.html
