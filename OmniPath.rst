.. _OmniPath:

=============================================
Omni-Path network fabric by Cornelis Networks
=============================================

The Niflheim_ cluster has a 100 Gbit/s Intel OmniPath high speed network fabric.

This page assumes a **RHEL/CentOS 7 or 8** Linux system.

.. Contents::

.. _systemd: https://en.wikipedia.org/wiki/Systemd
.. _slurm: https://wiki.fysik.dtu.dk/niflheim/SLURM
.. _Niflheim: https://wiki.fysik.dtu.dk/Niflheim_Getting_started/niflheim/

OmniPath software and documentation
===================================

From early 2021 the Intel Omni-Path product has been taken over by a spin-out company CornelisNetworks_.

Download of software and documentation is available from CornelisNetworks_:

* Download of OmniPath_software_ (free of charge, but registration is needed).

There is a *Omni-Path User Group* OPUG_ for public discussions about OmniPath.

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
This can be done from the Linux OS by pdsh_::

  pdsh -w <node-list> 'lspci -vvv -s 04:00.0 | grep LnkSta:' | dshbak -c

The output may look like::

  LnkSta: Speed 8GT/s, Width x16, TrErr- Train- SlotClk+ DLActive- BWMgmt- ABWMgmt-

Here the PCIe device ID is 04:00.0, you determine it by::

  lspci | grep Omni-Path

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

* Therefore there is **no requirement to install kmod-ifs-kernel-updates** kernel modules any longer, which is necessary with RHEL/CentOS 7 and RHEL/CentOS 8 up to 8.3.

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

For RHEL/CentOS 8 (and EL8 clones) see the *Release Notes* Table 8 with a list of *RHEL 8.5 Distribution RPMs*.

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

Omni-Path software 10.9 and later
=================================

The *Intel® Omni-Path Fabric Software* from Release 10.9 (and later, such as 10.10) has been redesigned for better software package installation,
and the instructions are consequently updated.

Read the following Release 10.9 documents:

* *Release Notes*
* *Omni-Path Fabric Software Installation Guide*

For RHEL/CentOS 7 and 8 some base OS prerequisite packages must be installed on login and compute nodes, 
see *Intel ® Omni-Path Software Release Notes* section *OS RPMs Installation Prerequisites*.
The package list differs slightly with OS version.

For RHEL/CentOS 7 (EL7) see Table 9 *RHEL* 7.x Distribution RPMs*.
For RHEL/CentOS 8 (and EL8 clones) see Table 10 *RHEL* 8.x Distribution RPMs*.

Install the following sets of packages:

* IB/OPA Centric::

    EL7: yum install ibacm infinipath-psm libibumad libibverbs libnl3 librdmacm opensm-libs perftest qperf rdma-core rdma-core-devel
    EL8: dnf install ibacm libibumad libibverbs libnl3 librdmacm opensm-libs perftest qperf rdma-core rdma-core-devel

* System Centric::

    EL7: yum install atlas bc coreutils expat expect gcc-gfortran glibc json-c libgfortran libgomp libquadmath libstdc++ libstdc++-devel
    EL8: dnf install atlas bc coreutils expat expect gcc-gfortran glibc json-c libgfortran libgomp libquadmath libstdc++ libstdc++-devel ncurses-compat-libs

* Other/Generally Installed::

    EL7: yum install bash irqbalance kmod libgcc perl perl-Getopt-Long perl-PathTools perl-Socket pkgconfig python systemd systemd-libs
    EL8: dnf install bash irqbalance kmod libgcc perl perl-Getopt-Long perl-PathTools perl-Socket pkgconfig systemd systemd-libs kernel-modules-extra

  **NOTE:** The *hfidiags* package contains a script requiring ``/usr/bin/python2``.
  On EL8 it is recommended to **not install hfidiags** and thereby avoid pulling in the obsolete *python2* packages.

* Build Requirements::

    EL7: yum install expat-devel json-c-devel libnl3-devel libpfm libuuid-devel numactl-devel opensm-libs openssl-devel sqlite-devel valgrind-devel
    EL8: dnf install autoconf automake bison elfutils-libelf-devel expat-devel flex gcc-c++.x86_64 kernel-abi-whitelists kernel-rpm-macros libnl3-devel libpfm libtool libuuid-devel ncurses-devel numactl-devel opensm-libs openssl-devel tcl-devel

**WARNING:** The running kernel version must also be identical to the **newest kernel and kernel-devel RPMs** installed on the system!

It is therefore strongly recommended **NOT** to update the kernel and kernel-devel RPMs until after the OPA software installation.
The **kernel-devel** RPM must be installed during OS installation and must correspond to the running kernel during installation.

Installation with OPA Yum repository
------------------------------------

For Intel Omni-Path software installation see *Intel ® Omni-Path Fabric Software Installation Guide Rev. 11.0*, December 2018.,
Chapter *4.0 Install the Intel ® Omni-Path Software*.

See *Intel ® Omni-Path Fabric Software Installation Guide Rev. 11.0*, December 2018., Section *4.4 Install Using Linux* Distribution Software Packages Provided by Intel*.

If you have **HPE servers with OPA adapters**, you can install OPA software using the HPE_Intel_OPA_ SDR repository.
Instructions are provided in the section *Installing Intel_OPA packages*.
The contents of the Yum repository can be browsed at https://downloads.linux.hpe.com/SDR/repo/intel_opa/

.. _HPE_Intel_OPA: https://downloads.linux.hpe.com/SDR/project/intel_opa/

It is strongly recommended to use in stead the **Yum** package manager for installation, and this has become possible starting with release 10.9.

Create the Yum repo file ``/etc/yum.repos.d/IntelOPA.repo`` (**NOTE:** Replace *RHEL77...* by the current version)::

  [IntelOPA-Common]
  name=IntelOPA Common pkgs
  gpgcheck=0
  baseurl=file:///home/opt/IntelOPA-Basic.RHEL77-x86_64.10.10.1.0.36/repos/OPA_PKGS
  enabled=1

where the ``baseurl`` points to where you have unpacked the OPA files (a directory or an http web-site).
Please note that from IntelOPA 10.9.3 the repo files are now in the *repos/OPA_PKGS/* subdirectory.

IMPORTANT: Since CentOS 7 provides an older OPA software version 10.9, you must clean the Yum cache in order to pick up the IntelOPA packages::

  yum clean all

The OPA package list can be learned from *Appendix B Intel ® OPA Software Components to Packages Mapping*.
The above mentioned components correspond to these RPM packages:

.. list-table::
  :widths: 1 3

  * - **Component**
    - **RPM package**
  * - opa_stack
    - opa-scripts kmod-ifs-kernel-updates
  * - oftools
    - opa-basic-tools opa-address-resolution
  * - intel_hfi
    - hfi1-firmware hfi1-firmware_debug libpsm2 libpsm2-devel libpsm2-compat libfabric libfabric-devel libfabric-psm libfabric-psm2 libfabric-verbs hfi1-diagtools-sw hfidiags
  * - delta_ipoib
    - (none)

The complete installation list is now for EL7::

  yum install opa-scripts kmod-ifs-kernel-updates 
  yum install opa-basic-tools opa-address-resolution
  yum install hfi1-firmware hfi1-firmware_debug libpsm2 libpsm2-devel libpsm2-compat libfabric libfabric-devel libfabric-psm libfabric-psm2 libfabric-verbs hfi1-diagtools-sw hfidiags

and for EL8::

  dnf install opa-scripts opa-basic-tools opa-address-resolution
  dnf install hfi1-firmware hfi1-firmware_debug libpsm2 libpsm2-devel libpsm2-compat libfabric libfabric-devel libfabric-psm2 libfabric-verbs hfi1-diagtools-sw

**NOTE:** The *hfidiags* package requires *python2* which is unwanted on EL8 systems.

A reboot is required after the OPA kernel module installation.
It is OK to update the kernel and kernel-devel RPMs **only after** the OPA kernel module installation.

As with releases prior to 10.9, it is still possible to unpack the software tar-ball and install (only) the required OPA basic software (add ``-v`` or ``-vv`` to increase logging)::

  ./INSTALL -i opa_stack -i intel_hfi -i delta_ipoib -i oftools

However, the above *Yum* installation method is preferred.

OPA kernel module gets replaced by "yum update"
-----------------------------------------------

The OPA kernel driver module delivered by the IntelOPA-Basic.RHEL77-x86_64.10.10.*.gz tar-ball is installed into the running kernel,
and the installed driver version can be identified by::

  # modinfo hfi1
  filename:       /lib/modules/3.10.0-1062.9.1.el7.x86_64/extra/ifs-kernel-updates/hfi1.ko
  version:        10.10.1.0
  description:    Intel Omni-Path Architecture driver
  license:        Dual BSD/GPL
  retpoline:      Y
  rhelversion:    7.7
  srcversion:     CEDD1A5E5B37C71A40B745F
  (lines deleted)

The important line is *version: 10.10.1.0* which indicates that it is the Intel-supplied driver.

**IMPORTANT:** Even though the correct kernel module version is **installed**,
this does not necessaily mean that it has been **activated**.
You must compare the ``/sys/module`` file to the ``modinfo`` output to ensure that the **identical source version** is being used.
This command is useful::

  # echo -n "srcversion:     "; cat /sys/module/hfi1/srcversion; modinfo hfi1 | grep srcversion:
  srcversion:     CEDD1A5E5B37C71A40B745F
  srcversion:     CEDD1A5E5B37C71A40B745F

If the source versions differ, you have to reboot the node and compare the ``srcversion`` signatures again.
See also `How to find the version of a compiled kernel module? <https://stackoverflow.com/questions/4839024/how-to-find-the-version-of-a-compiled-kernel-module/4839060>`_.

If you subsequently update the Linux kernel with::

  yum update

the stock CentOS 7 (or RHEL 7) kernel OPA driver module replaces the Intel module!

This can be seen by::

  # modinfo hfi1
  filename:       /lib/modules/3.10.0-1062.12.1.el7.x86_64/kernel/drivers/infiniband/hw/hfi1/hfi1.ko.xz
  description:    Intel Omni-Path Architecture driver
  license:        Dual BSD/GPL
  firmware:       hfi1_pcie.fw
  firmware:       hfi1_sbus.fw
  firmware:       hfi1_fabric.fw
  firmware:       hfi1_dc8051.fw
  retpoline:      Y
  rhelversion:    7.7
  srcversion:     69D92ED8B2436C4B5EF0318
  alias:          pci:v00008086d000024F1sv*sd*bc*sc*i*
  alias:          pci:v00008086d000024F0sv*sd*bc*sc*i*
  depends:        rdmavt,ib_core,i2c-algo-bit
  intree:         Y
  vermagic:       3.10.0-1062.12.1.el7.x86_64 SMP mod_unload modversions 
  signer:         CentOS Linux kernel signing key
  sig_key:        A0:A1:B3:38:BF:C1:AB:19:5F:0B:AF:17:1B:44:90:D1:AF:85:5C:A7
  sig_hashalgo:   sha256
  (lines deleted)

where the *version* information is now absent, and the module signature is by CentOS.
You should compare the ``srcversion`` signatures again as shown above.

Reinstall OPA kernel module after every kernel update
-----------------------------------------------------

As explained above, when a kernel update is installed with ``yum update``, the CentOS OPA driver replaces the Intel OPA kernel module driver.
Intel Support has acknowledged (February 2020) that this is the expected behavior, and no fix will be made.

The proper and correct solution to updating kernel module drivers would be to use DKMS_ `Dynamic Kernel Module Support <https://en.wikipedia.org/wiki/Dynamic_Kernel_Module_Support>`_.

Therefore, **every time** your kernel is updated, it is recommended to unpack the OPA software tar-ball to the local root filesystem, for example::

  cd /root
  tar xzf <...>/IntelOPA-IFS.RHEL78-x86_64.10.11.0.0.577.tgz
  cd /root/IntelOPA-IFS.RHEL78-x86_64.10.11.0.0.577

Then build and reinstall (only) the required OPA stack basic software (add ``-v`` or ``-vv`` to increase logging)::

  ./INSTALL -i opa_stack

or alternatively just build the package from the source RPM (version numbers may differ)::

  rpm -ivh CornelisOPX-OFA_DELTA.RHEL*/SRPMS/ifs-kernel-updates*.src.rpm
  rpmbuild -bb $HOME/rpmbuild/SPECS/ifs-kernel-updates.spec
  yum install $HOME/rpmbuild/RPMS/x86_64/kmod-ifs-kernel-updates-XXXX.x86_64.rpm

Verify the OPA driver version (as above)::

  modinfo hfi1

A better procedure may be to run the ``./INSTALL -i opa_stack`` on just a single OPA compute node which is running the latest Linux kernel.
See the log-file ``/var/log/opa.log`` for the results.

The ``INSTALL`` script actually creates a useful kernel RPM package, for example::

  /root/IntelOPA-Basic.RHEL77-x86_64.10.10.1.0.36/IntelOPA-OFA_DELTA.RHEL77-x86_64.10.10.1.0.37/RPMS/redhat-ES77/kmod-ifs-kernel-updates-3.10.0_1062.18.1.el7.x86_64-1919.x86_64.rpm

Copy the ``kmod-ifs-kernel-updates-3.10.0_1062.18.1.el7.x86_64-1919.x86_64.rpm`` file to your management server and distribute it to all OPA compute nodes running the same kernel.

On each compute node you just need to install this RPM::

  yum install kmod-ifs-kernel-updates-3.10.0_1062.18.1.el7.x86_64-1919.x86_64.rpm

This actually also updates the boot initramfs file::

  /boot/initramfs-3.10.0-1062.18.1.el7.x86_64.img

Verify as above the OPA kernel driver module::

  modinfo hfi1

Using DKMS to install OPA kernel module automatically
-----------------------------------------------------

**WARNING: Experimental procedure - use at your own risk!**

This is an experiment trying to use DKMS_ to build and install the OPA kernel module automatically:

1. Unpack the IntelOPA drivers tar-ball to ``/tmp``, for example::

     cd /tmp; tar xzf IntelOPA-IFS.RHEL77-x86_64.10.10.1.0.36.tgz

2. Install the ``ifs-kernel-updates`` source RPM (destination is ``$HOME/rpmbuild``)::

     rpm -ivh CornelisOPX-OFA_DELTA.RHEL*/SRPMS/ifs-kernel-updates*.src.rpm

   Unpack the source tar-ball to ``/usr/src/``::

     cd /usr/src
     tar xzf $HOME/rpmbuild/SOURCES/ifs-kernel-updates-3.10.0_1062.el7.x86_64.tgz

3. Install the DKMS_ package from  the EPEL_ repository::

     yum install epel-release
     yum install dkms

.. _EPEL: https://fedoraproject.org/wiki/EPEL

   Read the ``man dkms`` manual page about usage of the command.

4. Go to the source directory::

     cd /usr/src/ifs-kernel-updates-3.10.0_1062.el7.x86_64

5. Create the file ``dkms.conf`` in this directory with the contents for the 4 different drivers::

     PACKAGE_NAME="ifs-kernel-updates"
     PACKAGE_VERSION="3.10.0_1062.el7.x86_64"
     BUILT_MODULE_NAME[0]="hfi1"
     DEST_MODULE_LOCATION[0]="/kernel/drivers/infiniband/hw/hfi1"
     BUILT_MODULE_NAME[1]="ib_ipoib"
     DEST_MODULE_LOCATION[1]="/kernel/drivers/infiniband/ulp/ipoib"
     BUILT_MODULE_NAME[2]="ib_qib"
     DEST_MODULE_LOCATION[2]="/kernel/drivers/infiniband/hw/qib"
     BUILT_MODULE_NAME[3]="rdmavt"
     DEST_MODULE_LOCATION[3]="/kernel/drivers/infiniband/sw/rdmavt"
     AUTOINSTALL="yes"

6. Add the kernel module given by ``PACKAGE_NAME`` and ``PACKAGE_VERSION`` to DKMS_ and verify it::

     dkms add ifs-kernel-updates/3.10.0_1062.el7.x86_64
     dkms status

7. Build the modules under DKMS_ control::

     dkms build ifs-kernel-updates/3.10.0_1062.el7.x86_64

   Append ``--verbose`` to print details.

8. Install the modules under DKMS_ control::

     dkms install ifs-kernel-updates/3.10.0_1062.el7.x86_64


Omni-Path firmware tools
------------------------

The Intel® Omni-Path firmware_tools_ package under the downloads of
*Intel® Omni-Path Unified Extensible Firmware Interface (UEFI) Firmware* contains an RPM package::

  yum install hfi1-firmware-tools-10.9.2.0-3.x86_64.rpm

which contains a utility which can query versions of files stored in the HFI adapter EPROM::

  hfi1_eprom -V

Usage of this command is documented in chapter 5 of the *Intel® Omni-Path Fabric Software Installation Guide*.

It may be necessary to update the UEFI firmware using this command::

  hfi1_eprom -d all -u /usr/share/opa/bios_images/*

where the *bios_images* have been installed by the *hfi1-uefi* RPM package.

In addition the *Intel® Omni-Path Thermal Management Module* TMM_Firmware_ should be upgraded.
See *man opatmmtool*.

Install the  TMM_Firmware_ RPM package::

  yum install hfi1-tmm-10.9.0.0-208.noarch.rpm

This RPM provides the firmware file::

  /lib/firmware/updates/hfi1_smbus.fw

Check the TMM_Firmware_ version by::

  opatmmtool fwversion
  opatmmtool -f /lib/firmware/updates/hfi1_smbus.fw fileversion

Usage of this command is documented in chapter 15 of the *Intel® Omni-Path Fabric Software Installation Guide*.

Firmware upgrade operation::

  opatmmtool -f /lib/firmware/updates/hfi1_smbus.fw update
  opatmmtool reboot    # Non-disruptive
  opatmmtool fwversion

.. _firmware_tools: https://downloadcenter.intel.com/download/28722/Intel-Omni-Path-Unified-Extensible-Firmware-Interface-UEFI-Firmware
.. _TMM_Firmware: https://downloadcenter.intel.com/download/28523/Intel-Omni-Path-Thermal-Management-Module-TMM-Firmware

Fabric Manager software 10.9
----------------------------

The *Intel® Omni-Path Fabric Software Installation Guide* section 4.4 *Install Using Linux* Distribution Software Packages Provided by Intel* states:

* An example of additional software that should be installed on Fabric Manager nodes is shown below::

     opa-fm opa-fm-debuginfo opa-fm-debuginfo opa-debuginfo opa-fastfabric opa-mpi-apps

See also the package list in *Appendix B Intel® OPA Software Components to Packages Mapping*.

IMPORTANT: Since CentOS 7.7 provides an older OPA software version 10.9, you must clean the Yum cache in order to pick up the IntelOPA packages::

  yum clean all

It seems that this subset is sufficient for the OPA Fabric Manager::

  yum install opa-fm opa-fastfabric 

Omni-Path software 10.6, 10.7, 10.8
===================================

**NOTICE:** The software release 10.8 and older should now be considered as **obsolete**.

Install RHEL/CentOS 7 prerequisites
-----------------------------------

For RHEL/CentOS 7 the following base prerequisite packages must be installed on login and compute nodes.
There are two distinct situations:

1. The server contains only OmniPath adapters::

     yum install libibmad libibumad libibumad-devel libibverbs librdmacm libibcm libpfm.i686 ibacm qperf perftest rdma infinipath-psm infinipath-psm-devel libhfi1 expat elfutils-libelf-devel libstdc++-devel gcc-gfortran atlas tcl expect tcsh sysfsutils pciutils bc opensm-devel opensm-libs rpm-build redhat-rpm-config kernel-devel papi.i686

2. The server contains both Mellanox Infiniband as well as OmniPath adapters.
   Go to the next section *Mellanox OFED installation*.

Mellanox OFED installation
..........................

In case the server contains both Mellanox Infiniband as well as OmniPath adapters, the required order of installation is:

1. Install RHEL/CentOS 7 prerequisites::

     yum install expect tcl tk

  The ``mlnxofedinstall`` will tell you if any prerequisites are missing.

2. Install the Mellanox_OFED_ software **before** you install any OmniPath software.
   Read the *Mellanox OFED for Linux User Manual* and perform the software installation::

     mlnxofedinstall

3. Install OmniPath software as described below.

.. _Mellanox_OFED: http://www.mellanox.com/content/pages.php?pg=products_dyn&product_family=26

Prevent yum update from overwriting Intel OPA packages
------------------------------------------------------

This section is relevant only for OPA software **prior to 10.9**.

As of Intel OPA software release 10.6 (late 2017), all Intel RPMs are still **not installed with yum**, but in stead by a brain-dead operation::

  rpm -i --force --nodeps <rpm list>

Since all CentOS/RHEL 7 packages are installed with *yum*, the yum database contains no record of the Intel OPA RPMs.

When you subsequently update the OS by::

  yum update

the CentOS/RHEL 7 OPA updates version 10.3 RPMs will replace several Intel OPA RPMs (``opa-*``) previously installed.
This will of course cause havoc on your OPA installation.

Until Intel has solved this problem, it is **mandatory** to exclude all OPA RPM updates from the distribution by appending these rules to ``/etc/yum.conf``::

  exclude=opa-* libpsm2* libfabric* hfi1*


Install OPA IFS software on manager node
----------------------------------------

Follow the *Intel® Omni-Path Fabric Software Installation Guide* chapter *4.0 Install the Intel® Omni-Path Software* for installation details.

On *IFS* servers also the following are required::

  yum install libibverbs-devel libibmad-devel librdmacm-devel ibacm-devel openssl-devel libuuid-devel expat-devel valgrind-devel

Unpack the IntelOPA-IFS.<DISTRO>-x86_64.10.<version>.tgz tar-ball and run the ``INSTALL`` script (add ``-v`` or ``-vv`` to increase logging), 
for example for RHEL/CentOS 7.3::

  tar xf IntelOPA-IFS.RHEL73-x86_64.10.3.0.0.81.tgz
  cd IntelOPA-IFS.RHEL73-x86_64.10.3.0.0.81
  ./INSTALL

Select all appropriate softwares to be installed.
The *Fabric Manager* node requires the *FastFabric* and *OPA FM* components besides the *BASIC* components, see the software installation guide chapter *Upgrade from IntelOPA-Basic to IntelOPA-IFS*

If the manager node should run the OPA *Fabric Manager* service, make sure to enable this *Intel OPA Autostart* item::

  OPA FM (opafm) 

The *opafm* can also be started using *Systemd* services::

  systemctl enable opafm
  systemctl start opafm

Alternatively, just run the CLI version to install the basic software manually as shown below.
Then install and enable *opafm* and *fastfabric*::

  ./INSTALL -i opafm -i fastfabric -E opafm

You must make sure this host's *Static hostname* is set correctly (not just ``localhost.localdomain``)::

  hostnamectl
  hostnamectl set-hostname <hostname>.<domainname>

The node must be **rebooted** after the install to activate new kernel modules and set the correct hostname.

NOTE: It is important to permit the installation to update of the file ``/etc/security/limits.conf`` with memory locking limits::

  * hard memlock unlimited
  * soft memlock unlimited

This file is read by PAM when users log in.
However, system daemons started during the boot process **do not** use ``/etc/security/limits.conf``,
and the correct memory limits must be set inside the daemon startup scripts.
This is especially important for batch job services.

Install OPA software on all nodes using opafastfabric
-----------------------------------------------------

NOTE: This uses Intel's installation tools, but you may alternatively use the manual installation method described below.

Follow the *Intel® Omni-Path Fabric Software Installation Guide* chapter *7.0 Install Host Software on the Remaining Hosts Using the FastFabric TUI*.
Run this on the manager node and select *Host Setup*::

  opafastfabric

Run the following menu items in this order::

   3) Host Setup
   2) Set Up Password-Less SSH/SCP
   1) Verify Hosts Pingable    

The good nodes are listed in the file ``/etc/sysconfig/opa/good``.

The tar-ball ``IntelOPA-BASIC.<DISTRO>-x86_64.10.<version>.tgz`` must be available on the Manager node for installation on the compute nodes.
Now install the OPA software on all good nodes::

  5) Install/Upgrade OPA Software   
  6) Configure IPoIB IP Address  

At the end of the installation select to reboot the nodes::

  8) Reboot Hosts

IPoIB device ib0 not present
............................

We have seen an error when upgrading the OPA software stack from 10.2 to 10.3.
The *ib0* network interface is defined correctly in ``/etc/sysconfig/network-scripts/ifcfg-ib0``, yet the ib0 network device doesn't exist and an error is printed::

  /etc/sysconfig/network-scripts/ifup-ib[3239]: Device ib0 does not seem to be present, delaying initialization.

The OPA software ``INSTALL`` file menu::

  3) Reconfigure Driver Autostart 
     3) OFA IP over IB   [Enable ]

will fix this error after a reboot.

This can also be done with the ``opaconfig`` command::

  # opaconfig -E delta_ipoib
  Configuring autostart for Selected installed OPA Drivers
  Enabling autostart for OFA IP over IB
  Done OPA Driver Autostart Configuration.

To verify *ping over IPoIB* connectivity, use a Manager node with the *IFS* software::

  /usr/sbin/opahostadmin -f /etc/sysconfig/opa/allhosts ipoibping

Manual software installation on a single node
---------------------------------------------

.. _Kickstart: https://en.wikipedia.org/wiki/Kickstart_(Linux)

When individual compute nodes are installed from scratch, the OPA software must be installed from the CLI command line in the Kickstart_ post-install scripts.
The Intel OPA documentation does not describe this procedure, so we have to discover it by trial-and-error.

Start by reading the manual *Intel ® Omni-Path Fabric Software Installation Guide* chapter 4.

The installation steps are:

1. Copy the *Basic* tar-ball to the system root and unpack it::

     cp (some location)/IntelOPA-Basic.RHEL73-x86_64.10.3.0.0.81.tgz /root/
     tar xzf IntelOPA-Basic.RHEL73-x86_64.10.3.0.0.81.tgz
     cd IntelOPA-Basic.RHEL73-x86_64.10.3.0.0.81

2. You can run the ``INSTALL`` TUI script to learn about menu items.
   Then install the basic software::

     ./INSTALL -i opa_stack -i intel_hfi -i delta_ipoib -i oftools

   The installation log will be in ``/var/log/opa.log``.

3. The PSM2_ library ``libpsm2`` is not installed by any of the above components, so install it manually::

     cd ./IntelOPA-OFED_DELTA.RHEL73-x86_64.10.3.0.0.82/RPMS/redhat-ES73
     yum install libpsm2-10.X*rpm libpsm2-devel*rpm

   It seems that the libpsm2-compat RPM is not needed because it conflicts with the required infinipath-psm RPM.

4. The IPoIB network script ``/etc/sysconfig/network-scripts/ifcfg-ib0`` must be edited manually, see the section *IPoIB Configuration* below.

**NOTE:** The ``INSTALL`` TUI script installs RPM packages **not** by using ``yum``, but directly with the ``rpm`` command, for example as seen in ``/var/log/opa.log``::

  /bin/rpm -U --force --nodeps  ./IntelOPA-OFED_DELTA.RHEL73-x86_64.10.3.0.0.82/RPMS/redhat-ES73/kmod-ifs-kernel-updates-3.10.0_514.el7.x86_64-123.x86_64.rpm

It is not a good practice to install packages with ``--force --nodeps`` (forcing installation without checking for dependencies)!
The RPMs installed will unfortunately not be logged to ``/var/log/yum.log`` as is the best practice.

Uninstallation of OPA software
------------------------------

To uninstall all OPA software use the INSTALL script option:

* -u - uninstall all ULPs and drivers with default options

The command is::

  ./INSTALL -u



Installation of Fabric Manager GUI
----------------------------------

For the GUI download the RPM package ``IntelOPA-FMGUI.linux-<VERSION>.noarch.rpm`` (or similar) and install with::

  yum install IntelOPA-FMGUI.linux-10.3.0.0.60.noarch.rpm

Read the *Intel Omni-Path Fabric Software Installation Guide* chapter 14 *Install Intel Omni-Path Fabric Suite Fabric Manager GUI*.
The file ``/etc/opa-fm/opafm.xml`` must be edited to enable running the GUI on *localhost* without SSL encryption::

  <SslSecurityEnable>0</SslSecurityEnable>

Also enable the *Fabric Executive* (FE) component of the *Fabric Manager*::

  <Start>1</Start> <!-- default FE startup for all instances -->

Furthermore, for redundant setups configure also the priorities described in the *Redundant Fabric Manager hosts* below.

Then restart the *Fabric Manager*::

  systemctl restart opafm

Now run the GUI (a Java applet)::

  fmgui

Configure *fmgui*:

* Enter the **localhost** hostname on the Fabric Manager server/node.
* If you use a remote server, enter its hostname.
  You should also enable SSL.

The remote FM GUI requires port 3245 to be open on the Fabric Manager node, so you may have to open it in the firewall (if any)::

  firewall-cmd --zone=public --add-port=3245/tcp --permanent
  firewall-cmd --reload

The next step is:

* Menu item *Subnet*, select *Connect To* and click the network name you defined above.

Read the *Intel Omni-Path Fabric Suite Fabric Manager GUI User Guide*.

OPA kernel modules
------------------

During the above installation the ``INSTALL`` script installs a RPM package with OPA kernel modules.
In ``/var/log/opa.log`` this is logged as::

  installing kmod-ifs-kernel-updates-3.10.0_514.el7.x86_64-123.x86_64...
    /bin/rpm -U --force --nodeps  ./IntelOPA-OFED_DELTA.RHEL73-x86_64.10.3.0.0.82/RPMS/redhat-ES73/kmod-ifs-kernel-updates-3.10.0_514.el7.x86_64-123.x86_64.rpm

The source RPM file is::

  ./IntelOPA-OFED_DELTA.RHEL73-x86_64.10.3.0.0.82/SRPMS/ifs-kernel-updates-3.10.0_514.el7.x86_64-123.src.rpm

The RPM contains the following files::

  # rpm -ql kmod-ifs-kernel-updates
  /etc/depmod.d/ifs-kernel-updates.conf
  /lib/modules/3.10.0-514.el7.x86_64/extra/ifs-kernel-updates/hfi1.ko
  /lib/modules/3.10.0-514.el7.x86_64/extra/ifs-kernel-updates/rdmavt.ko

The problem with this package is that the kernel modules **do not get updated** when you update the Linux kernel!
We are awaiting Intel's response to this problem.
One good method would be to use *Dynamic Kernel Module Support* (DKMS_).

.. _DKMS: https://wiki.centos.org/HowTos/BuildingKernelModules

OPA configuration files
-----------------------

On the management node, the OPA configuration files are stored in this directory::

  /etc/sysconfig/opa/

OPA srpd services
-----------------

The service *srpd* (SCSI RDMA Protocol over InfiniBand) is not used on compute nodes, so turn it off::

  systemctl stop srpd
  systemctl disable srpd

Managing the OPA fabric
=======================

Read the *Intel® Omni-Path Fabric Suite Fabric Manager User Guide*.

Check the fabric
----------------

On each host you can verify the OPA HFI adapter revision by::

  opahfirev

(installed by the *opa-basic-tools* RPM package).

Check the OPA link quality on a list of nodes using pdsh_::

  pdsh -w <node-list>  'opainfo  | grep Link' | dshbak -c

.. _pdsh: https://linux.die.net/man/1/pdsh

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

  yum install opa-fastfabric

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

Make sure the PSM2_ packages have been installed::

  rpm -q libpsm2 libpsm2-devel

and compile the code::

  gcc psm2-demo.c -o psm2-demo -lpsm2

Now run two instances (server and client) on the same or different nodes::

  ./psm2-demo -s  # Server
  ./psm2-demo     # Client

If you get an error, see the *Memory limits* section below.

OpenMPI configuration
=====================

Optimized performance with OPA requires the PSM2_ interface, see https://www.open-mpi.org/faq/?category=building#build-p2p.
Search in the Intel documentation (link at the top of this page) for the document entitled *Intel® Performance Scaled Messaging 2 (PSM2) Programmer’s Guide*.

Intel ® Performance Scaled Messaging 2 (PSM2_) is only available on RHEL/CentOS 7.2 or later, see https://github.com/01org/opa-psm2/blob/master/README which states::

  Building PSM2 is possible on RHEL 7.2 as it ships with hfi1 kernel driver.

.. _PSM2: https://github.com/01org/opa-psm2

On CentOS 7 you must have these prerequisite packages, which are installed as above by the Intel OPA software::

  rpm -q libpsm2 libpsm2-devel

If you get OpenMPI runtime errors like::

  mca: base: components_open: component pml / cm open function failed

then you may need to install also these packages before building OpenMPI::

  yum install infinipath-psm infinipath-psm-devel

see `[OMPI users] Issue about cm PML <https://www.open-mpi.org/community/lists/users/2016/03/28734.php>`_
and `rocks 6.2 infiniband <https://lists.sdsc.edu/pipermail/npaci-rocks-discussion/2016-March/068806.html>`_.

Build OpenMPI on RHEL/CentOS 7.2 or later with the configuration flags::

   --with-psm2=/usr # Build support for the PSM 2 library (starting with the v1.10 series).

Note however in the `RHEL7.2 Release Notes <https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/7.2_Release_Notes/known-issues-kernel.html>`_ the following section:

* PSM2 MTL disabled to avoid conflicts between PSM and PSM2 APIs:

    The new libpsm2 package provides the PSM2 API for use with Intel Omni-Path devices, which overlaps with the Performance Scaled Messaging (PSM) API installed by the infinipath-psm package for use with Truescale devices.
    The API overlap results in undefined behavior when a process links to libraries provided by both packages.
    This problem affects Open MPI if the set of its enabled MCA modules includes the psm2 Matching Transport Layer (MTL) and one or more modules that directly or indirectly depend on the libpsm_infinipath.so.1 library from the infinipath-psm package. 

The older PSM library is not available on CentOS 7::

   --with-psm=<dir> # Build support for the PSM library.

Intel OpenMPI
-------------

The *IntelOPA-Basic.RHEL73-x86_64.10.3.0.0.81* package contains Intel's builds of OpenMPI using the GCC compiler.
Install the **hfi** versions of RPMs to use OmniPath, for example::

  cd IntelOPA-OFED_DELTA.RHEL73-x86_64.10.3.0.0.82/RPMS/redhat-ES73
  install openmpi_gcc_hfi-1.10.4-9.x86_64.rpm mpi-selector-1.0.3-1.x86_64.rpm mpitests_openmpi_gcc_hfi-3.2-930.x86_64.rpm

To use the Intel OpenMPI see the Intel *Omni-Path Fabric Performance Tuning User Guide* chapter 5 *MPI Performance*:

* Load the environment variables::

    source /usr/mpi/gcc/openmpi-1.10.4-hfi/bin/mpivars.sh

* Use the options in your mpirun command to specify the use of PSM2 with OpenMPI::

    mpirun -mca pml cm -mca mtl psm2 ... 

Using OpenMPI with OmniPath
---------------------------

First make the correct version of OpenMPI available to your applications.
If you use *software modules* (see the :ref:`EasyBuild_modules` page) load the appropriate module, for example::

  # module load foss
  # module list
  Currently Loaded Modules:
  1) EasyBuild/3.0.1
  2) GCCcore/5.4.0
  3) binutils/2.26-GCCcore-5.4.0
  4) GCC/5.4.0-2.26
  5) numactl/2.0.11-GCC-5.4.0-2.26
  6) hwloc/1.11.3-GCC-5.4.0-2.26
  7) OpenMPI/1.10.3-GCC-5.4.0-2.26
  8) OpenBLAS/0.2.18-GCC-5.4.0-2.26-LAPACK-3.6.1
  9) gompi/2016b
 10) FFTW/3.3.4-gompi-2016b
 11) ScaLAPACK/2.0.2-gompi-2016b-OpenBLAS-0.2.18-LAPACK-3.6.1
 12) foss/2016b

Now verify that the *psm2* component has been built into OpenMPI::

  # ompi_info | grep psm2
    MCA mtl: psm2 (MCA v2.0.0, API v2.0.0, Component v1.10.3)

MPI performance tuning
----------------------

The Intel *Omni-Path Fabric Performance Tuning User Guide* discusses in chapter 5 *MPI Performance*.

* Use the options in your mpirun command to specify the use of PSM2 with OpenMPI::

    mpirun -mca pml cm -mca mtl psm2 ... 

OpenMPI tests
-------------

The Intel RPM ``mpitests_openmpi_gcc_hfi`` contains a number of MPI testing codes in the ``/usr/mpi/gcc/openmpi-1.10.4-hfi/tests`` subdirectories, for example:

* ``intel/deviation`` - MPI bandwidth and latency deviations from *Intel MPI Benchmarks* (IMB).
* ``osu_benchmarks-3.1.1/osu_bibw`` - Bidirectional Bandwidth Test from OSU_benchmarks_.

.. _OSU_benchmarks: http://mvapich.cse.ohio-state.edu/benchmarks/

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

If the locked memory limit is too low, a rather strange error will be printed by the PSM2_ library::

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

  LimitNOFILE=51200
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

To configure IPoIB_ on CentOS/RHEL 7 see:

* Configuring_IPoIB_
* Configure_IPoIB_Using_the_command_line_
* Configure_InfiniBand_and_RDMA_Networks_
* Configuring_the_Base_RDMA_Subsystem_

For convenience we provide a script which will help you configure OmniPath and/or Mellanox Infiniband adapters on a CentOS/RHEL 7 system:

* https://ftp.fysik.dtu.dk/OmniPath/ipoib_config.sh

**WARNING:** You cannot use the instructions below if you have also installed the Mellanox_OFED_ distribution, since Mellanox_OFED_ replaces many RHEL/CentOS system utilities.
However, the ``ibstat`` command is still the best way to display adapter information.

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

    yum install infiniband-diags

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

  yum install libibverbs-utils infiniband-diags

Then you can list available Infiniband-like devices::

  ibv_devices
  ibv_devinfo

and see the device status::

  ibstat

To display the OPA device *ib0* IP address information on a list of nodes::

  pdsh -w <node-list> '/sbin/ip -4 -o addr show label ib0' | sort 

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
