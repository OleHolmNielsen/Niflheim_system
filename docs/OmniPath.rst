.. _OmniPath:

=============================================
Omni-Path network fabric by Cornelis Networks
=============================================

The Niflheim_ cluster has a 100 Gbit/s Intel OmniPath high speed network fabric.

This page assumes a RHEL 8 Linux or EL8 clones system.
Older archived versions of these pages with information related to CentOS 7 Linux may be found in the archive_ or in the v24.06 release of these pages.

.. _archive: https://wiki.fysik.dtu.dk/Niflheim_system_archive/


.. Contents::

.. _systemd: https://en.wikipedia.org/wiki/Systemd
.. _slurm: https://wiki.fysik.dtu.dk/niflheim/SLURM
.. _Niflheim: https://wiki.fysik.dtu.dk/Niflheim_Getting_started/niflheim/

OmniPath software and documentation
===================================

From early 2021 the Intel Omni-Path product has been taken over by a spin-out company CornelisNetworks_.

Download of software and documentation is available from CornelisNetworks_:

* Download of OmniPath_software_ (free of charge, but registration is needed).

.. _OPUG: https://www.psc.edu/user-resources/computing/omni-path-user-group
.. _CornelisNetworks: https://www.cornelisnetworks.com/
.. _OmniPath_software: https://customercenter.cornelisnetworks.com/#/login?returnUrl=%2Fcustomer%2Fassets%2Fsoftware-and-documentation%2Frelease

OmniPath switches
-----------------

Please see the :ref:`OmniPath_switches` page.

There is (apparently) a requirement that the OPA node software version and the switch firmware version must not differ by more than 1 version.
For example, software version 10.9 is compatible with switch firmware 10.8, but older firmware releases (10.7, etc.) are not supported or tested.

Hardware installation
=====================

Read the document *Intel Omni-Path Host Fabric Interface Installation Guide*, especially the section *Hardware Installation*.

The default BIOS setting must be that the PCIe speed is set to **Auto** (may vary with BIOS).
For a PCIe Gen3 x16 adapter the PCIe bus speed should be 8 GT/s, whereas Gen2 speed would only be 5 GT/s.
In older versions of this manual there was an incorrect requirement of Gen2 speed setting.

Please verify your adapter's speed and PCIe width.
This can be done from the Linux OS by ClusterShell_::

  clush -w <node-list> 'lspci -vvv -s 04:00.0 | grep LnkSta:' | dshbak -c

The output may look like::

  LnkSta: Speed 8GT/s, Width x16, TrErr- Train- SlotClk+ DLActive- BWMgmt- ABWMgmt-

Here the PCIe device ID is 04:00.0, you determine it by::

  lspci | grep Omni-Path

.. _ClusterShell: https://clustershell.readthedocs.io/en/latest/intro.html

Software installation
=====================

OmniPath HFI adapter hardware is supported with OPA software 10.11 on compute nodes with the following Intel and AMD processors:

* Haswell_ (HSW, Xeon v3)
* Broadwell_ (BDW, Xeon v4)
* Skylake_ (SKL, Xeon v5)
* Cascade_Lake_ (CSL)
* Ice Lake
* 2nd and 3rd Gen AMD EPYC Processors (Rome, Milan)

Please note that older processors are **not supported**.
However, the OmniPath adapter may well work on older Xeon servers, even though it's not officially supported by Intel.
For example, we have tested OmniPath on an old *Sandy Bridge* server successfully.

Also note that **OmniPath software versions must be identical** on all compute nodes, or at most differ by 1 minor version (such as 10.6 and 10.5).  Documentation???

The following software installation packages are available for an Intel ® Omni-Path Fabric:

* Intel *Omni-Path Fabric Host Software*:
  – This is the basic installation package that installs the Intel ® Omni-Path Fabric Host Software components needed to set up compute, I/O, and Service nodes with drivers, stacks, and basic tools for local configuration and monitoring.

* Intel *Omni-Path Fabric Suite (IFS) Software*:
  – This installation package provides special features and includes the Intel ® Omni-Path Fabric Host Software package, along with the Intel ® Omni-Path Fabric Suite FastFabric Toolset (FastFabric) and the Intel ® Omni-Path Fabric Suite Fabric Manager (Fabric Manager).

* Intel *Omni-Path Fabric Suite Fabric Manager GUI (Fabric Manager GUI)*:
  – This installation package provides a set of features for viewing and monitoring the fabric or multiple fabrics, and is installed on a computer outside of the fabric.

Operating Systems supported are listed in the *Release Notes*.
Intel Omni-Path Architecture (OPA) host software is fully supported in Red Hat Enterprise Linux 7.6, see 
https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/7/html-single/7.6_release_notes/#BZ1627126

.. _Haswell: https://en.wikichip.org/wiki/intel/microarchitectures/haswell_(client)
.. _Broadwell: https://en.wikichip.org/wiki/intel/microarchitectures/broadwell_(client)
.. _Skylake: https://en.wikichip.org/wiki/intel/microarchitectures/skylake_(server)
.. _Cascade_Lake: https://en.wikichip.org/wiki/intel/microarchitectures/cascade_lake

Download the latest version *Intel® Omni-Path Fabric Software (Including Intel® Omni-Path HFI Driver)* from the OmniPath_software_ page:

* ``IntelOPA-Basic.RHEL7*-x86_64.10.X.*.tgz`` for compute nodes.
* ``IntelOPA-IFS.RHEL7*-x86_64.10.X.*.tgz`` for the management node.

Read the *Intel® Omni-Path Fabric Software Installation Guide* from the *Publications* page.

Cornelis Omni-Path software 10.11
=================================

The current Omni-Path software release from CornelisNetworks_ as of February 2022 is **release 10.11.1.3.1** with support for RHEL 8.5 and 7.8.


**NOTICE:** 

* The RHEL 8.5 (and clones thereof such as AlmaLinux_ and RockyLinux_) fortunately include all required kernel modules as well as some (almost) up-to-date OPA software!

The `RHEL 8.4 Release Notes <https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/8/html-single/8.4_release_notes/index>`_
introduced Omni-Path support:

* **Cornelis Omni-Path Architecture (OPA) Host Software**

  Omni-Path Architecture (OPA) host software is fully supported in Red Hat Enterprise Linux 8.4. OPA provides Host Fabric Interface (HFI) hardware with initialization and setup for high performance data transfers (high bandwidth, high message rate, low latency) between compute and I/O nodes in a clustered environment.

  For instructions on installing Omni-Path Architecture, see: `Cornelis Omni-Path Fabric Software Release Notes <https://customercenter.cornelisnetworks.com/#/customer/assets/download/213>`_ file.
  (`BZ#1960412 <https://bugzilla.redhat.com/show_bug.cgi?id=1960412>`_) 

Cornelis OPA DNF repo
---------------------

Download the latest (RHEL 8.5) Cornelis Omni-Path Fabric OPXS (Formerly IFS) tar-ball file from the `Release Library <https://customercenter.cornelisnetworks.com/#/customer/assets/software-and-documentation/release>`_ page,
for example ``CornelisOPX-OPXS.RHEL85-x86_64.10.11.1.3.1.tgz``.

Create a DNF repository in some selected directory by unpacking the tar-ball, for example::

  cd /home/opt
  tar xzf CornelisOPX-OPXS.RHEL85-x86_64.10.11.1.3.1.tgz

Create a repo file ``/etc/yum.repos.d/CornelisOPX.repo`` pointing to the above directory::

  [CornelisOPX-Common]
  name=CornelisOPX Common pkgs
  gpgcheck=0
  baseurl=file:///home/opt/CornelisOPX-OPXS.RHEL85-x86_64.10.11.1.3.1/repos/OPA_PKGS
  enabled=1

The *baseurl* may alternatively point to an https/http server in your network.

On all nodes using this repo, make sure to clean the DNF cache::

  dnf clean all

OPA software installation on EL8.5
----------------------------------

For RHEL and EL8 clones see the *Release Notes* Table 8 with a list of *RHEL 8 Distribution RPMs*.

Install the following sets of packages:

* IB/OPA Centric::

    dnf install ibacm libibumad libibverbs libnl3 librdmacm opensm-libs perftest qperf rdma-core rdma-core-devel

* System Centric::

    dnf install atlas bc coreutils expat expect gcc-gfortran glibc json-c libgfortran libgomp libquadmath libstdc++ libstdc++-devel ncurses-compat-libs

* Other/Generally Installed::

    dnf install bash irqbalance kmod libgcc perl perl-Getopt-Long perl-PathTools perl-Socket pkgconfig systemd systemd-libs kernel-modules-extra

* Build Requirements::

    dnf install autoconf automake bison elfutils-libelf-devel expat-devel flex gcc-c++.x86_64 kernel-abi-whitelists kernel-rpm-macros libnl3-devel libpfm libtool libuuid-devel ncurses-devel numactl-devel opensm-libs openssl-devel tcl-devel

Also install these EL8 RPMs::

  dnf install libibverbs-utils 

Finally install the *CornelisOPX* packages from the repo configured above::

  dnf install hfi1-diagtools-sw hfi1-firmware hfi1-firmware_debug ifs-kernel-updates-devel libfabric libfabric-devel libfabric-psm2 libfabric-verbs libpsm2 libpsm2-compat libpsm2-devel opa-address-resolution opa-basic-tools opaconfig opa-libopamgt opa-libopamgt-devel opa-scripts opa-fastfabric opa-fm

**NOTE:** The *hfidiags* package contains a script requiring ``/usr/bin/python2``.
On EL8 it is recommended to **not install hfidiags** and thereby avoid pulling in the obsolete and insecure *python2* packages.

There is no need to install the *CornelisOPX* kernel module because EL8 from EL8.4 and onwards contains OPA support in the kernel.
However, if you wish to install the very latest kernel module, build the package from the source RPM (version numbers may differ)::

  rpm -ivh CornelisOPX-OFA_DELTA.RHEL*/SRPMS/ifs-kernel-updates*.src.rpm
  rpmbuild -bb $HOME/rpmbuild/SPECS/ifs-kernel-updates.spec
  dnf install $HOME/rpmbuild/RPMS/x86_64/kmod-ifs-kernel-updates-XXXX.x86_64.rpm

.. _AlmaLinux: https://almalinux.org/
.. _RockyLinux: https://rockylinux.org/

Managing the OPA fabric
=======================

Read the *Intel® Omni-Path Fabric Suite Fabric Manager User Guide*.

Check the fabric
----------------

On each host you can verify the OPA HFI adapter revision by::

  opahfirev

(installed by the *opa-basic-tools* RPM package).

Check the OPA link quality on a list of nodes using ClusterShell_::

  clush -w <node-list>  'opainfo  | grep Link' | dshbak -c

The *opa-fastfabric* RPM package (part of the IFS software package) contains a useful host checking script::

  /usr/share/opa/samples/hostverify.sh    # From OPA software version 10.7

You may copy this from an IFS host to other hosts and run it.
To see available options run::

  hostverify.sh --help

Check the Fabric Manager (FM)
-----------------------------

The *OPA FM* Fabric Manager was installed above on the Manager node.
Manage the ``opafm`` service by::

  systemctl status opafm 
  systemctl enable opafm 
  systemctl start opafm 
  systemctl restart opafm 
  systemctl stop opafm 

The ``/usr/lib/opa-fm/bin/opafmctrl`` allows the user to manage the instances of the FM that are running after the ``opafm`` service has been started.

The *OPA FM* configuration file is ``/etc/opa-fm/opafm.xml``.
Other OPA configuration files are in ``/etc/sysconfig/opa/``.

Fabric Manager commands
-----------------------

See chapter 8 of the FM user guide.  Install the software by::

  dnf install opa-fastfabric

Some useful commands are:

* ``opafmconfigcheck``: Parses and verifies the configuration file of a Fabric Manager (FM). Displays debugging and status information.
* ``opafabricinfo``: Provides a brief summary of the components in the fabric.
* ``opatop``: Fabric Performance Monitor menu to display performance, congestion, and error information about a fabric.
* ``opareport``: Provides powerful fabric analysis and reporting capabilities.
* ``opafmcmd``: Executes a command to a specific instance of the Fabric Manager (FM).

The ``opareport`` command displays information about nodes and links in the fabric, see the man-page  or the FM user guide.
For example, to list the *Master Subnet Manager* host in the fabric::

  opareport -F sm

To list also other subnet manager hosts, it is simpler to do::

  opareport | tail

To display link problems::

  opareport -o errors -o slowlinks
  opareport --clear     # Clears the port counters

Requirement of setting static hostname
--------------------------------------

Unfortunately, the OPA driver by default use the hostname ``localhost.localdomain`` in stead of the node name obtained from DHCP.

Any hostname or SM *Name* fields from the ``opareport`` command are obtained as the host's *Static hostname*, which by default is ``localhost.localdomain`` (see ``man hostnamectl`` and the file ``/etc/hostname``).

This is rather inconvenient, so you **must change** the *Static hostname* using the correct hostname using one of these commands::

  hostnamectl set-hostname <hostname>.<domainname>
  hostnamectl set-hostname `hostname`

Then you have to **reboot the system** to reinitialize the OPA driver setup.

Redundant Fabric Manager hosts
------------------------------

You may want to run the FM on two hosts, an active **Master** and an *Standby* **Slave** FM.

The use of Redundant Fabric Manager hosts is described in the *Intel ® Omni-Path Fabric Suite Fabric Manager User Guide* section *4.1 Redundant FMs in a Fabric*.
See especially the section *4.1.2 Master FM Failover*.

The configuration file ``/etc/opa-fm/opafm.xml`` must be consistent on all FM hosts, use the tool ``opafmconfigdiff`` to verify this.

**NOTE:**
All FM hosts must be running the same **exact** same minor version of Omni-Path software.

The FM hosts priority is configured in ``/etc/opa-fm/opafm.xml`` by the parameters::

    <!-- Priority and Elevated Priority control failover for SM, PM. -->
    <!-- Priority is used during initial negotiation, higher Priority wins. -->
    <!-- ElevatedPriority is assumed by winning master, this can prevent -->
    <!-- fallback when previous master comes back on line.  -->
    <Priority>8</Priority> <!-- 0 to 15, higher wins -->
    <ElevatedPriority>15</ElevatedPriority> <!-- 0 to 15, higher wins -->

The *Priority* settings must be:

* For the master set **Priority=8**
* For the slaves set **Priority=1**

There are some points to note in the documentation:

* The first *opafm* service running on a host will be the master.

* When several hosts/switches run *opafm*, an election will decide the master.

* Any switches running a FM instance will have a lower priority and yield to a host-based master.

* If the master's *opafm* is stopped, one of the standby slaves will become the new master after some timeout.

* One can flexibly add and remove *opafm* hosts, as long as there is one host/switch who will be the master.

Check the FM servers by::

  opareport | tail

which should show a status similar to::

  2 Connected SMs in Fabric:
  State       GUID               Name
  Standby     0x00117501010963d1 server2 hfi1_0
  Master      0x0011750101097827 server1 hfi1_0

Inactive secondary fabric manager
.................................

**Note:** The secondary server must have a status of **Standby**, for example::

  $ opareport | tail 
  ...
  2 Connected SMs in Fabric:
  State       GUID               Name
  Standby     0x00117501010963d1 server2 hfi1_0
  Master      0x0011750101097827 server1 hfi1_0


An **Inactive** state indicates an **error condition** which you must resolve.

Typically, this is due to a **version mismatch** between the OPA softwares on the master and standby nodes.
This may occur, for example, if you inadvertently install the OS distro RPM packages in stead of the Intel OPA packages.
Check the OPA FM package versions by::

  rpm -q opa-fm opa-fastfabric

OPA cable errors
================

See https://www.intel.com/content/www/us/en/support/articles/000029514/network-and-io/fabric-products.html

Create a fabric log file::

  opacapture -d 3 <filename>

After the command has completed, attach the tgz file to your troubleshooting case.


Intel PSM2 Sample Program
=========================

To verify the basic functionality of the OmniPath network, copy the *Intel® PSM2 Sample Program* code from the PDF documentation file 
*Intel® Performance Scaled Messaging 2 (PSM2) Programmer’s Guide* in Intel's *End User Publications* web page.
We attach the file psm2-demo.c__ for convenience.

__ attachment:attachments/psm2-demo.c

Make sure the PSM2 packages have been installed::

  rpm -q libpsm2 libpsm2-devel

and compile the code::

  gcc psm2-demo.c -o psm2-demo -lpsm2

Now run two instances (server and client) on the same or different nodes::

  ./psm2-demo -s  # Server
  ./psm2-demo     # Client

If you get an error, see the *Memory limits* section below.

Memory limits
=============

:ref:`OmniPath` requires all user processes to have **unlimited locked memory**.
For normal users starting a shell, this is configured in ``/etc/security/limits.conf`` by adding the lines::

  * hard memlock unlimited
  * soft memlock unlimited

This file is read by PAM when users log in.
However, system daemons started during the boot process **do not** use ``/etc/security/limits.conf``,
and the correct memory limits must be set inside the daemon startup scripts.
This is especially important for batch job services.

Users may verify the correct locked memory limits by the command::

  # ulimit -l
  unlimited

If the locked memory limit is too low, a rather strange error will be printed by the PSM2 library::

  PSM2 can't open hfi unit: -1 (err=23)
  PSM2 was unable to open an endpoint. Please make sure that the network link is
  active on the node and the hardware is functioning.
    Error: Failure in initializing endpoint
  hfi_userinit: mmap of rcvhdrq at dabbad0004030000 failed: Resource temporarily unavailable

There will be system syslog messages as well like::

  psm2-demo: (hfi/PSM)[4982]: PSM2 can't open hfi unit: -1 (err=23)
  kernel: cache_from_obj: Wrong slab cache. kmalloc-64(382:step_batch) but object is from kmem_cache_node

In the *libpsm2* source code the error originate from the function ``hfi_userinit()`` in the file ``libpsm2-10.*/opa/opa_proto.c``.

Slurm configuration
===================

MPI jobs and other tasks using the :ref:`OmniPath` fabric must have **unlimited locked memory**, see above.
For ``slurmd`` running under systemd_ the limits are configured in ``/usr/lib/systemd/system/slurmd.service`` as::

  LimitNOFILE=131072
  LimitMEMLOCK=infinity
  LimitSTACK=infinity

Limits defined in ``/etc/security/limits.conf`` or ``/etc/security/limits.d/\*.conf`` are **not** effective for systemd_ services, see https://access.redhat.com/solutions/1257953,
so any limits must be defined in the service file, see ``man systemd.exec``.

To ensure that job tasks running under Slurm_ have this configuration, verify the ``slurmd`` daemon's limits by::

  # grep locked /proc/$(pgrep -u 0 slurmd)/limits
  Max locked memory         unlimited            unlimited            bytes

Also, the ``slurm.conf`` file must have this configuration::

  PropagateResourceLimitsExcept=MEMLOCK

as explained in https://slurm.schedmd.com/faq.html#memlock.

The memory limit error with :ref:`OmniPath` was discussed in `Slurm bug 3363 <https://bugs.schedmd.com/show_bug.cgi?id=3363>`_.

IPoIB configuration
===================

.. _IPoIB: https://tools.ietf.org/html/rfc4392

The role of IPoIB_ is to provide an IP network emulation layer on top of InfiniBand RDMA networks, see Understanding_InfiniBand_and_RDMA_technologies_.

.. _Understanding_InfiniBand_and_RDMA_technologies: https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Networking_Guide/ch-Configure_InfiniBand_and_RDMA_Networks.html#sec-Understanding_InfiniBand_and_RDMA_technologies

DNS hostnames: For the IPoIB_ convention, append **-opa** to the hostname.

To configure IPoIB_ on RHEL see:

* Configuring_IPoIB_
* Configure_IPoIB_Using_the_command_line_
* Configure_InfiniBand_and_RDMA_Networks_
* Configuring_the_Base_RDMA_Subsystem_

The ``ibstat`` command is still the best way to display adapter information.

.. _Mellanox_OFED: http://www.mellanox.com/content/pages.php?pg=products_dyn&product_family=26
.. _Configuring_IPoIB: https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Networking_Guide/sec-Configuring_IPoIB.html
.. _Configure_IPoIB_Using_the_command_line: https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Networking_Guide/sec-Configure_IPoIB_Using_the_command_line.html
.. _Configure_InfiniBand_and_RDMA_Networks: https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Networking_Guide/ch-Configure_InfiniBand_and_RDMA_Networks.html
.. _Configuring_the_Base_RDMA_Subsystem: https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Networking_Guide/sec-Configuring_the_Base_RDMA_Subsystem.html
.. _Usage_of_70-persistent-ipoib: https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Networking_Guide/sec-Configuring_the_Base_RDMA_Subsystem.html#sec-Usage_of_70-persistent-ipoib.rules

You must also configure ``/etc/rdma/rdma.conf`` as shown in Configuring_the_Base_RDMA_Subsystem_.
Suggested parameters (including NFS service) are::

    IPOIB_LOAD=yes
    SRP_LOAD=no
    SRPT_LOAD=no
    ISER_LOAD=no
    ISERT_LOAD=no
    RDS_LOAD=no
    XPRTRDMA_LOAD=yes
    SVCRDMA_LOAD=yes
    FIXUP_MTRR_REGS=no
    ARPTABLE_TUNING=yes

IPoIB devices
-------------

The IPoIB_ network devices must be configured carefully by hand,
since this is not done automatically, and there are no standard device names.

If you use only a single Infiniband adapter and network interface, it will probably be named **ib0**, and you do not necessarily have to perform any device configuration.
Then go to the IPoIB_ network configuration below.

However, if you have:

* Multiple Infiniband and/or OmniPath adapters, 
* Multiple ports per adapter,
* You want to control the device names in stead of the default ib0, ib1 etc.,

then you must configure the Infiniband devices carefully:

* Install the prequisite RPM::

    dnf install infiniband-diags

  and then discover the link/infiniband hardware addresses by::

    ibstat

  You can also get the link/infiniband hardware address of all network interfaces by::

    ip link show 

* Select device names for the IPoIB_ devices, since there doesn't seem to be any naming standard for these (for Ethernet there is a Consistent_Network_Device_Naming_ standard).

  The ``ibstat`` command lists adapter names::

    mlx4_0, mlx4_1 etc. for Mellanox adapters no. 0 and 1.
    hfi1_0, hfi1_1 etc. for Intel OmniPath adapters no. 0 and 1.

  The adapter ports may either be configured for Infiniband or for Ethernet, so it may be reasonable to name the IPoIB_ ports as XXXib0, XXXib1 etc., since there may be several adapters.
  The kernel's internal device names ib0 etc. should **not** be reused in a manual configuration.

  Suggested interface names might be concatenating adapter and port names like::

    mlx4_0ib0
    mlx4_0ib1
    hfi1_0ib0

* Edit the udev_ file ``/etc/udev/rules.d/70-persistent-ipoib.rules`` as explained in Usage_of_70-persistent-ipoib_ using the last 8 bytes of each link/infiniband hardware address.
  An example file may be::

    ACTION=="add", SUBSYSTEM=="net", DRIVERS=="?*", ATTR{type}=="32", ATTR{address}=="?*70:10:6f:ff:ff:a0:74:71", NAME="mlx4_0ib0"
    ACTION=="add", SUBSYSTEM=="net", DRIVERS=="?*", ATTR{type}=="32", ATTR{address}=="?*70:10:6f:ff:ff:a0:74:72", NAME="mlx4_0ib1"
    ACTION=="add", SUBSYSTEM=="net", DRIVERS=="?*", ATTR{type}=="32", ATTR{address}=="?*00:11:75:01:01:7a:ff:df", NAME="hfi1_0ib0"

  It is perfectly possible for OmniPath adapters to coexist with Mellanox adapters in this way.

* You can force the IPoIB_ interfaces to be renamed without performing a reboot by removing the ib_ipoib kernel module and then reloading it as follows::

    rmmod ib_ipoib
    modprobe ib_ipoib

.. _Consistent_Network_Device_Naming: https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Networking_Guide/ch-Consistent_Network_Device_Naming.html
.. _udev: https://www.kernel.org/pub/linux/utils/kernel/hotplug/udev/udev.html

IPoIB network configuration
---------------------------

When you have set up the Infiniband devices, the next step is to configure the IPoIB_ interfaces:

* See Configure_IPoIB_Using_the_command_line_ about creating *ifcfg* files in ``/etc/sysconfig/network-scripts/``.
  Notice these points:

  - The DEVICE field must match the custom name created in any udev_ renaming rules.
  - The NAME entry need not match the device name. If the GUI connection editor is started, the NAME field is what is used to present a name for this connection to the user.
  - The TYPE field must be InfiniBand in order for InfiniBand options to be processed properly.
  - CONNECTED_MODE is either yes or no, where **yes** will use connected mode and no will use datagram mode for communications, see 
    see https://www.kernel.org/doc/Documentation/infiniband/ipoib.txt.
    The value **yes** should be used for performance reasons.

* An example *ifcfg* file ``ifcfg-OmniPath`` would be::

    NM_CONTROLLED=no
    CONNECTED_MODE=yes
    TYPE=InfiniBand
    BOOTPROTO=none
    IPADDR=10.4.128.107
    PREFIX=16
    DEFROUTE=no
    IPV4_FAILURE_FATAL=yes
    IPV6INIT=no
    NAME=OmniPath
    DEVICE=hfi1_0ib0
    ONBOOT=yes
    MTU=65520

  On EL8 we have used this configuration successfully::

    DEVICE=ib0
    TYPE='InfiniBand'
    BOOTPROTO=static
    IPADDR=10.4.135.3
    ONBOOT=yes
    CONNECTED_MODE=yes
    DEFROUTE=no
    IPV6INIT=no
    MTU=65520
    NETMASK=255.255.0.0
    PREFIX=16
    BROADCAST=10.4.255.255
    NETWORK=10.4.0.0

With the above configurations in place you can restart the network service::

  systemctl restart network

and display all network interfaces::

  ifconfig -a

where the OmniPath and/or Infiniband interfaces should now be shown.

Monitoring IPoIB interfaces
---------------------------

Install these RPMs::

  dnf install libibverbs-utils infiniband-diags

Then you can list available Infiniband-like devices::

  ibv_devices
  ibv_devinfo

and see the device status::

  ibstat

To display the OPA device *ib0* IP address information on a list of nodes::

  clush -w <node-list> '/sbin/ip -4 -o addr show label ib0' | sort 

Performance tuning
==================

Download the manual *Intel® Omni-Path Performance Tuning User Guide*.
See Chapter **2.0 BIOS Settings** about recommended settings, they include:

* CPU power and performance policy = **Performance** or **Balanced performance**.
* Enhanced Intel SpeedStep Technology = **Enabled**.
* Intel Turbo Boost Technology = **Enabled**.
* Intel VT for Directed I/O (VT-d) = **Disabled**.
* CPU C-State  = **Enabled**.
* Processor C3 = **Disabled**.
* Processor C6 = **Enabled**.
* IOU Non-posted Prefetch = **Disabled** (where available).
* Cluster-on-Die = **Disabled**.
* Early Snoop = **Disabled**.
* Home Snoop = **Enabled**.
* NUMA Optimized = **Enabled**.
* MaxPayloadSize = **Auto** or **256B**.
