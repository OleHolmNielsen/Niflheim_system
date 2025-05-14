.. _OmniPath_switches:

==================
Omni-Path switches
==================

.. Contents::

OmniPath software and documentation
===================================

From early 2021 the Intel Omni-Path product has been taken over by a spin-out company CornelisNetworks_.

Download of software and documentation is available from CornelisNetworks_:

* Download of OmniPath_software_ (free of charge, but registration is needed).

There is a *Omni-Path User Group* OPUG_ for public discussions about OmniPath.

.. _OPUG: https://www.psc.edu/user-resources/computing/omni-path-user-group
.. _CornelisNetworks: https://www.cornelisnetworks.com/
.. _OmniPath_software: https://customercenter.cornelisnetworks.com/#/login?returnUrl=%2Fcustomer%2Fassets%2Fsoftware-and-documentation%2Frelease

Console CLI connection
======================

Connect a server or PC to the serial console using the (unusual) supplied USB type A (male) to type A (male) cable.

Linux serial port configuration
-------------------------------

Perform the following tasks to connect a terminal to the switch console port:

* Connect the USB type B connector on the supplied switch and connect the other end to a computer running VT100 terminal emulation software.

* The Linux PC will create a new serial USB device::

    /dev/ttyUSB0

  Change device permissions::

    chmod 666 /dev/ttyUSB0

  Install the *minicom* RPM package.

  Connect using *minicom* (or *screen*)::

    minicom -D /dev/ttyUSB0

* Configure the terminal emulation software as follows:
 
  * Select the appropriate serial port (for example, COM 1) to connect to the console.
  * Set the data rate to **115,200 baud**.
  * Set the data format to 8 data bits, 1 stop bit, and no parity.
  * Set the **flow control to none**.
  * Set the terminal emulation mode to VT100.
  * Select Terminal keys for Function, Arrow, and Ctrl keys. Make sure that the setting is for Terminal keys (not Microsoft Windows keys).

Windows serial port configuration
---------------------------------

The switch contains a serial port device of type FT231X_ USB UART from *FTDI Chip*:

* Download and unpack the appropriate FT231X_drivers_.
* Update device drivers from the downloaded files.
* A new USB serial port device appears, update its drivers from the same files.
* Connect to the serial port using a terminal (for example, PuTTy_) with port speed 115200 baud and 8-N-1.
  Select **No Flow Control**.

.. _FT231X: http://www.ftdichip.com/Products/ICs/FT231X.html
.. _FT231X_drivers: http://www.ftdichip.com/Drivers/VCP.htm
.. _PuTTy: http://www.chiark.greenend.org.uk/~sgtatham/putty/download.html

Ethernet CLI connection
-----------------------

Only switches with the *Management Module* installed can be accessed via the RJ45 Ethernet port (Unmanaged switches have a deactivated RJ45 port).
To access the CLI interface via Ethernet:

* Connect a *cross-over* network cable to the switch RJ45 port and a PC network port.
* The switch factory default network configuration is:

  - IP 192.168.100.9 netmask 255.255.255.0
  - Administrator login: admin password: adminpass

* Configure the PC network port with another address in the same subnet 192.168.100.0/24.

* From the PC try the factory default IP address::

    ping 192.168.100.9

If the ``ping`` is successful, try to login by **SSH** to the IP address:

* The system prompts for a user name. The CLI has the following default user names::

    Operator access: operator
    Administrator access: admin

  Type the appropriate user name and press ENTER.

* The system prompts for a password. The CLI has the following default passwords::

    Operator access: operpass
    Administrator access: adminpass

Please note that the SSH connection will appear to be very slow (up to 1 minute) because SSH will try public-key logins before trying the password for login.
Question: How to make SSH logins and ignoring the public-keys?

**Notice:** The **telnet** connection doesn't work, even though the Intel manual advises to use it!

Managed switches
================

Configure SSH to use obsolete ciphers
-------------------------------------

Managed switches are accessed using the SSH protocol.
Unfortunately, the switches' SSH server only accepts some obsolete ciphers as shown by::

  $ ssh admin@<switch-address>
  Unable to negotiate with 10.x.x.x port 22: no matching cipher found. Their offer: aes128-cbc,aes192-cbc,aes256-cbc

This was observed on OPA switches running firmware 10.8.4.0.5 (current as of March 2022).

This will occur on all systems with **OpenSSH version 7.6** and later, for example, in RHEL 8 and clones, but not on older RHEL 7 systems.
See the `OpenSSH 7.6 release notes <https://www.openssh.com/txt/release-7.6>`_::

  ssh(1): do not offer CBC ciphers by default.

As explained in `ssh Unable to negotiate: no matching cipher found, is rejecting cbc <https://unix.stackexchange.com/questions/459074/ssh-unable-to-negotiate-no-matching-cipher-found-is-rejecting-cbc>`_::

  The -cbc algorithms have turned out to be vulnerable to an attack. As a result, up-to-date versions of OpenSSH will now reject those algorithms by default: for now, they are still available if you need them.

The CBC cipher security advisory dates back to 2008: https://www.kb.cert.org/vuls/id/958563

It is necessary to make a workaround:
Create a file ``~/.ssh/config`` with this content::

  Host *
    SendEnv LANG LC_*
    Ciphers +aes256-cbc

List of managed switches
------------------------

See chapter 8 of the *Fabric Manager* user guide. 
Install the software by::

  yum install opa-fastfabric

In a working Omni-Path fabric inquire the list of all **Managed switches** and append the list of switch names to the file ``/etc/opa/chassis``::

  opagenchassis >> /etc/opa/chassis

To inquire the switch chassises use::

  opachassisadmin -S getconfig

where the ``-S`` option will ask for the switches' common password.

See also `Configure SSH to use obsolete ciphers`_ above regarding obsolete SSH ciphers.
The ``getconfig`` command will then fail with errors::

  TEST SUITE getconfig CASE (getconfig.<name>.getconfig) get <name> FAILED

Managed switch configuration
----------------------------

* Once logged into the switch CLI type::

    list
    list all

  to display brief help for all available commands.

* Learn the network MAC address information by::

    ifShow

* To change the switch to use standard DHCP network configuration, first configure your DHCP server with MAC and IP address for the switch.
  Then configure the switch using::

    dhcpEnable

  and reconnect the switch to the normal network. 
  The original network session will be disconnected.

* Logging out::

    exit

* **Saving the settings:** As far as we can see in the manual, system settings are permanent when entered.
  The good side of this is that you don't have to remember saving the settings to permanent memory.

Additional switch configurations
--------------------------------

* You must set the system hostname (choose a name XYZ)::

    setSystemName "XYZ"

  as well as the default Node Name (SMA Description) ::

    setNodeDesc "XYZ"

  There is no documentation of the meaning of nor difference between the *System Name* and *Node Description* parameters.

  After a few minutes the *Subnet Manager* seems to recognize the new system name.

* To display the system hostname and description::

    setSystemName
    showNodeDesc

* To set the timezone offset in hours relative to GMT::

    timeZoneConf [<offset>]
    timeZoneConf 1      # Example

  <offset> :time offset in relation to GMT.

* To set the daylight savings time settings::

    timeDSTConf [<sw> <sd> <sm> <ew> <ed> <em>]

  For European DST::

    timeDSTConf 5 1 3 5 1 10

* To set the switch time and date use the `time`` command::

    time [{-S <hostname or IP address> | -T <hhmmss>[<mmddyyyy>]}]

  Using an NTP time server::

    time -S 192.38.82.136 # Example

* Change the logged in user's password (default password for admin is **adminpass**)::

    passwd

* Set the conventional CLI Backspace and Delete keys by swapping the factory default::

    swapBsDel

* To change the CLI prompt::

    prompt

    OPTIONS
       <prompt> :new prompt.

    NOTES
       The prompt may not exceed 11 characters and is not saved across reboots. 
       If the prompt contains a space, asterisk, comma, parenthesis or semicolon
       it must be enclosed with double quotes. For example: "*a prompt*". Also, 
       if a prompt is not accepted, try to enclose it with double quotes.
       In order for some FastFabric Tools to function correctly, the prompt must
       end in "-> "(note trailing space).

* Hardware check::

    hwCheck -verbose

* Check and update the timezone and time::

    timeZoneConf 1  # Hours East/West of UTC
    timeDSTConf [<sw> <sd> <sm> <ew> <ed> <em>]  # For example: timeDSTConf 5 1 3 5 1 10
    time
    time -T <hhmmss>[<mmddyyyy>]

Subnet Manager
--------------

To start a *Subnet Manager* (fabric size up to **only 100 nodes supported**, 200 nodes hard limit)::

  smControl start

The command options are::

  smControl [{start | stop | restart | status}]
  Options:
   start   :start the SM.
   stop    :stop the SM.
   restart :restarts the SM.
   status  :prints out the SM Status.


Make sure the *Subnet Manager* gets started at reboot::

  smConfig startAtBoot yes

Check the status::

  smControl

SNMP configuration
------------------

By default the managed switches do not respond to SNMP requests, one has to enable this explicitly in the CLI interface::

  -> uiconfig   # Lists current settings
  -> uiconfig -snmp 1 -snmpv1 1 -snmpv2 1

To set the system name, contact and location information::

  -> snmpSystem edit -n opa-c3 -c support@fysik.dtu.dk -l "DK;Lyngby;Niflheim;B309;059;rack 22;38"

This information may also be entered on the switch web interface under the *System* tab, but this currently has a bug causing the settings to be ignored.

From a management host one may then send inquiries by SNMP::

  snmpwalk -Os -c public -v 2c <IP-address> system

SNMP monitoring of managed switches
===================================

Managed OPA switches have a limited support for monitoring by SNMP (as of firmware release 10.8, end of 2018).
Unmanaged switches do not have TCP/IP and SNMP capabilities.

In the switch CLI you must first enable SNMP read-only access as shown above.

Install the ``snmpwalk`` and ``unzip`` commands::

  yum install net-snmp-utils unzip

Documentation of the Net-SNMP_ tools include:

* Using_and_loading_MIBS_ tutorial.

* The default MIBs directory search paths are displayed by::

    net-snmp-config --default-mibdirs

* See ``man snmp.conf`` regarding configuration of applications built using the Net-SNMP_ libraries.

* See ``man snmpcmd`` about arguments to the ``snmpwalk`` command.

* See ``man net-snmp-config`` about installed Net-SNMP_ libraries and binaries.

.. _Net-SNMP: https://en.wikipedia.org/wiki/Net-SNMP
.. _Using_and_loading_MIBS: http://net-snmp.sourceforge.net/wiki/index.php/TUT:Using_and_loading_MIBS

Omni-Path MIB files
-------------------

Download the MIB files from OmniPath_software_ page, for example::

  IntelOPA-STL1_MIBs-10.8.0.0.186.zip

The following steps makes it possible to use Intel's MIB files for defining SNMP OIDs:

1. Create a directory for the MIB files and unpack the .mi2 files::

     mkdir -p $HOME/.snmp/mibs 
     unzip -d $HOME/.snmp/mibs <download-location>/IntelOPA-STL1_MIBs-10.8.0.0.186.zip

2. Create a file ``$HOME/.snmp/snmp.conf`` that includes entries corresponding to the "``DEFINITIONS ::= BEGIN``" lines in all the ``.mi2`` files::

     mibs +ICS-CHASSIS-MIB
     mibs +ICS-CHASSIS-NOTIFICATION-MIB
     mibs +ICS-MASTER-MIB
     mibs +OPA-STAT-MIB

   The following one-liner will print the lines needed for ``$HOME/.snmp/snmp.conf``::

     grep DEFINITIONS $HOME/.snmp/mibs/*.mi2 | awk -F: '{print $2}' | awk '{printf("mibs +%s\n", $1)}'

3. Run the ``snmpwalk`` command for the given managed switch name (or IP address) specifying **ics** as the OID value::

     snmpwalk -Oa -v2c -cpublic <switchname> ics

Serial number of managed switches
---------------------------------

It is possible to read the switch serial number and model name using the above SNMP setup with these commands::

  $ snmpwalk -Oa -v2c -cpublic <IP-address> icsChassisSystemUnitFruSerialNumber.1
  ICS-CHASSIS-MIB::icsChassisSystemUnitFruSerialNumber.1 = STRING: USFUxxxx
  $ snmpwalk -Oa -v2c -cpublic <IP-address> icsChassisSystemUnitFruModel.1
  ICS-CHASSIS-MIB::icsChassisSystemUnitFruModel.1 = STRING: 100SWE48QF2

All switch information can be read by::

  $ snmpwalk -Oa -v2c -cpublic <IP-address> icsChassisMib 

Unmanaged switch configurations
===============================

Only switches with the *Management Module* installed can be accessed via the RJ45 network port.
Unmanaged switches have a deactivated RJ45 port.

See the document *Intel® Omni-Path Fabric Suite FastFabric User Guide*.

To list all switches (managed and unmanaged) in the OPA fabric::

  opasaquery -t sw 

To configure the switch names, see section *3.5 FastFabric OPA Switch Setup/Admin Menu*.
Run the command::

  opafastfabric
   2) Externally Managed Switch Setup/Admin
     2) Test for Switch Presence
     0) Edit Config and Select/Edit Switch File 

A **better way** is to use the OPA *Fabric Manager GUI*, see https://wiki.fysik.dtu.dk/niflheim/OmniPath#installation-of-fabric-manager-gui.

The ``fmgui`` tool allows you discover the network topology and see which nodes are connected to the unmanaged switches.

The ``opaswitchadmin`` command (see ``man opaswitchadmin``) is used to manage the externally managed switches:

* Performs a number of multi-step initialization and verification operations against one or more externally managed Intel(R) Omni-Path switches.
* The operations include initial switch setup, firmware upgrades, chassis reboot, and others.

The /etc/opa/switches file
--------------------------

To produce a list of Externally Managed switches in the required format for the ``/etc/opa/switches`` file and append the switch names (if necessary)::

  opagenswitches >> /etc/opa/switches

There is one line per switch of the form **guid,nodeDesc,distance**:

* guid - node guid of the switch

* nodeDesc - optional node description which should be programmed into the switch by FastFabric.
  It is recommended to supply a unique nodeDesc for each switch to simplify management of the cluster.

* distance - optional relative distance of the switch from the FastFabric node this is used by reboot operations to first operate on switches furthest from the FastFabric node.
  Nodes without a distance specified will be treated as furthest.

Copy the output of ``opagenswitches`` to the file ``/etc/opa/switches``.
Assign appropriate switch names in the *nodeDesc* field.

Every time the ``/etc/opa/switches`` file is modified you **must push** the switch names (*Node Descriptions*) to the switches by the command::

  opaswitchadmin configure
    Do you wish to configure the switch Link Width Options? [n]:
    Do you wish to configure the switch Node Description as it is set in the switches file? [n]: y
    (lines deleted)

**Notice:** When the switch firmware is updated, a ``opaswitchadmin configure`` is performed implicitly, thus pushing updated information to the switches.

**Notice:** The switch names only take effect when the switch is rebooted::

  opaswitchadmin reboot

An example ``/etc/opa/switches`` file::

  0x0011750102754f00,opa-L2,3
  0x001175010275c4ed,opa-L1,3
  0x001175010275c70f,opa-L5,3
  0x00117501027788dc,opa-L3,3
  0x00117501027795db,opa-L6,3
  0x0011750102779b92,opa-L4,3
  0x001175010277a5fb,opa-L7,1
  0x0011750102702b27,opa-L8,3

Information about unmanaged switches
------------------------------------

Report firmware and hardware version, part number, and data rate capability of all nodes::

  opaswitchadmin info

Read the switch hardware information including serial number etc.::

  opaswitchadmin hwvpd

Firmware updates
================

Firmware update on managed switch
---------------------------------

Display managed switch firmware versions by this OPA command::

  opachassisadmin -S getconfig

See also `Configure SSH to use obsolete ciphers`_ above regarding obsolete SSH ciphers.

Firmware update requires an FTP server.
Assume that the firmware files are stored in the directory ``pub/OmniPath``.

On a Managed switch's CLI interface you can configure download from the FTP server's IP-address (10.5.128.3 in this example)::

  fwversion   # Display fw versions
  fwSetUpdateParams -c management -h 10.5.128.4 -d pub/OmniPath -f STL1.q7.10.8.5.0.3.spkg
  fwSetUpdateParams -c management -f STL1.q7.10.8.5.0.3.spkg # Update only the file name
  fwShowUpdateParams
  fwUpdate

Useful commands::

  chassisQuery   # Check presence of module
  bootQuery module -all
  bootSelect module   # choose image to boot from

When everything looks fine, reboot the switch::

  reboot

Updating the Omni-Path Q7 BIOS and Board Controller Firmware 
------------------------------------------------------------

Intel has Omni-Path Q7 BIOS and Board Controller Firmware updates.
Read the switch documentation *Intel® Omni-Path Fabric Switches Hardware Installation Guide* in **Appendix A Q7 BIOS Update** for upgrade instructions.

If updates are required, this requires a host with an SFTP_ client (part of the *openssh-clients* RPM package).
The procedure is documented in the Appendix A:

Make sure that the subnet manager (SM) is not running on the switch::

  Edge-> smControl
  Subnet manager is not started.

Display versions by::

  bcfwversion
  biosversion

You **must read the detailed instructions in Appendix A** at this time before performing the upgrade.

Use SFTP_ on a remote host to upload firmware files to the OPA switch::

  $ sftp admin@opa-switch
  ...
  Connected to opa-switch.
  sftp> 

Upload the BIOS file from the current directory::

  $ sftp> put QA3ER962.q7.spkg /firmware/bios.pkg update

**On the OPA switch CLI** monitor the update progress until completed (this takes several minutes)::

  Edge-> showLastBiosRetCode -all
  Module System BIOS Last Exit Code: 0: Success

Upload the *board controller firmware (BCFW)* file from the current directory::

  sftp> put CGBCP429.q7.spkg /firmware/bcfw.pkg update

**On the OPA switch CLI** monitor the update progress until completed::

  Edge-> showLastBcFwRetCode -all
  Module Board Controller Firmware Last Exit Code: 0: Success

If everything is OK, reboot the switch via CLI::

  Edge-> reboot

After rebooting verify versions by::

  bcfwversion
  biosversion

.. _SFTP: https://en.wikipedia.org/wiki/SSH_File_Transfer_Protocol#SFTP_client

Firmware update on unmanaged switch
-----------------------------------

The CLI tool ``opaswitchadmin`` is used for unmanaged switch operations, see ``man opaswitchadmin``.
This command is part of the RPM package::

  rpm -q opa-fastfabric

To select individual or subsets of switches, see the *Intel® Omni-Path Fabric Suite FastFabric User Guide* section *3.3.2.3 Selection of Switches*:

To perform operations against a set of externally-managed switches, you can specify
the switch on which to operate using one of the following methods:

• On the command line, using the -N option.
• Using the environment variable SWITCHES to specify a space-separated list of switches. Useful when multiple commands are performed against the same small set of switches.
• Using the -L option or the SWITCHES_FILE environment variable to specify a file containing the set of switches. Useful for groups of switches that are used often.
  The file is located here: /etc/opa/switches by default. The file must list all switches in the cluster.  When the file is changed you must run ``opaswitchadmin configure`` and reboot the switches.

Within the tools, the options are considered in the following order:

1. -N <nodes> option
2. SWITCHES environment variable
3. -L option
4. SWITCHES_FILE environment variable
5. /etc/opa/switches file


On the OPA management host copy the firmware file like::

  Intel_PRREdge_V1_firmware.10.8.2.0.6.emfw

To identify leaf/edge switch GUIDs, pull out the tab at the back of the switch and read the GUID.

To list all unmanaged switches in the fabric::

  opagenswitches

The output should be copied to ``/etc/opa/switches`` by ``opaswitchadmin``.

To ping the unmanaged switches::

  /usr/sbin/opaswitchadmin ping

Report unmanaged switch firmware and hardware Info::

  /usr/sbin/opaswitchadmin info

Upgrade switch firmware::

  /usr/sbin/opaswitchadmin -a run -P Intel_PRREdge_V1_firmware.10.8.2.0.6.emfw upgrade

Reboot **all switches**::

  /usr/sbin/opaswitchadmin reboot

**WARNING**: The *Reboot Switch* action will reboot **all switches**.
To select a subset of switches, use the **-N <nodes>** flag or other options shown above.

To operate on subsets of switches that are defined in the file specified by -L add flags like::

  -L /etc/opa/switches 
  -L /etc/opa/switch-subset

To install a new firmware on unmanaged switches using the TUI tool::

  # opafastfabric
    2) Externally Managed Switch Setup/Admin
    4) Update Switch Firmware
    Enter Files/Directories to use (or none): Intel_PRREdge_V1_firmware.10.8.2.0.6.emfw
    6) Reboot Switch  # When all is OK
    7) Report Switch Firmware & Hardware Info  # Verification

**WARNING**: The *Reboot Switch* action will reboot **all switches**.
