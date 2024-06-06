.. _Slurm_installation:

================================
Slurm installation and upgrading
================================

.. Contents::

Jump to our top-level Slurm page: :ref:`SLURM`

To get started with Slurm_ see the Slurm_Quick_Start_ Administrator Guide.
See also `CECI Slurm Quick Start Tutorial <https://www.ceci-hpc.be/slurm_tutorial.html>`_.

.. _Slurm_Quick_Start: https://slurm.schedmd.com/quickstart_admin.html

.. _Slurm: https://www.schedmd.com/
.. _SchedMD: https://www.schedmd.com/
.. _Slurm_docs: https://slurm.schedmd.com/
.. _Slurm_FAQ: https://slurm.schedmd.com/faq.html
.. _Slurm_download: https://slurm.schedmd.com/download.html
.. _Slurm_mailing_lists: https://lists.schedmd.com/cgi-bin/dada/mail.cgi/list
.. _slurm_devel_archive: https://groups.google.com/forum/#!forum/slurm-devel
.. _Slurm_publications: https://slurm.schedmd.com/publications.html
.. _Slurm_tutorials: https://slurm.schedmd.com/tutorials.html
.. _Slurm_bugs: https://bugs.schedmd.com
.. _Slurm_man_pages: https://slurm.schedmd.com/man_index.html
.. _slurm.conf: https://slurm.schedmd.com/slurm.conf.html
.. _slurmdbd.conf: https://slurm.schedmd.com/slurmdbd.conf.html
.. _scontrol: https://slurm.schedmd.com/scontrol.html
.. _pdsh: https://github.com/grondo/pdsh
.. _ClusterShell: https://clustershell.readthedocs.io/en/latest/intro.html

Hardware optimization for the slurmctld master server
=====================================================

SchedMD_ recommends that the slurmctld_ server should have only a few, but **very fast CPU cores**, in order to ensure the best responsiveness.

The file system for ``/var/spool/slurmctld/`` should be mounted on the fastest possible disks (SSD or NVMe if possible).

Create global user accounts
===========================

There must be a uniform user and group name space (including UIDs and GIDs) across the cluster,
see the Slurm_Quick_Start_ Administrator Guide.
It is not necessary to permit user logins to the control hosts (*ControlMachine* or *BackupController*), but the users and groups must be configured on those hosts.
To restrict user login by SSH, use the ``AllowUsers`` parameter in ``/etc/ssh/sshd_config``.

Slurm_ and Munge_ require consistent UID and GID across **all servers and nodes in the cluster**, including the *slurm* and *munge* users.

It is very important to **avoid UID and GID below 1000**, as defined in the standard configuration file ``/etc/login.defs`` by the parameters ``UID_MIN, UID_MAX, GID_MIN, GID_MAX``,
see also https://en.wikipedia.org/wiki/User_identifier.

Create the users/groups for *slurm* and *munge*, for example::

  export MUNGEUSER=1005
  groupadd -g $MUNGEUSER munge
  useradd  -m -c "MUNGE Uid 'N' Gid Emporium" -d /var/lib/munge -u $MUNGEUSER -g munge  -s /sbin/nologin munge
  export SlurmUSER=1001
  groupadd -g $SlurmUSER slurm
  useradd  -m -c "Slurm workload manager" -d /var/lib/slurm -u $SlurmUSER -g slurm  -s /bin/bash slurm

and make sure that these same users are created identically on **all nodes**.
This must be done **prior to installing RPMs** (which would create random UID/GID pairs if these users don't exist).

Please note that UIDs and GIDs up to 1000 are currently reserved for the CentOS system users, see `this article <https://unix.stackexchange.com/questions/343445/user-id-less-than-1000-on-centos-7>`_ and the file */etc/login.defs*.

Slurm authentication plugin
============================

For an overview of authentication see the Authentication_Plugins_ page.
Beginning with version 23.11, Slurm has its own plugin that can create and validate credentials.
It validates that the requests come from legitimate UIDs and GIDs on other hosts with matching users and groups.

.. _Authentication_Plugins: https://slurm.schedmd.com/authentication.html

Munge authentication service
============================

For an overview of authentication see the Authentication_Plugins_ page.
The Munge_ authentication plugins identifies the user originating a message.
You should read the Munge_installation_ guide and the Munge_wiki_.

The EL8 and EL9 distributions contain Munge_ RPM packages version 0.5.13, install them by::

  dnf install munge munge-libs munge-devel

For RHEL/CentOS 7: Download Munge_ packages from https://dl.fedoraproject.org/pub/epel/7/x86_64/m/

On busy servers such as the slurmctld_ server, the munged_ daemon could become a bottleneck,
see the presentation *Field Notes 5: From The Frontlines of Slurm Support* in the Slurm_publications_ page.
On such servers it is recommended to increase the number of threads, see the munged_ manual page.
The issue is discussed in 
`excessive logging of: "Suspended new connections while processing backlog" <https://github.com/dun/munge/issues/94>`_.

.. _Munge: https://github.com/dun/munge
.. _Munge_installation: https://github.com/dun/munge/wiki/Installation-Guide
.. _Munge_wiki: https://github.com/dun/munge/wiki
.. _munged: https://github.com/dun/munge/wiki/Man-8-munged

Munge 0.5.16 upgrade
-------------------------

It is advantageous to install the latest Munge_release_ RPMs (currently 0.5.16) due to new features and bug fixes.
Build RPMs by::

  wget https://github.com/dun/munge/releases/download/munge-0.5.16/munge-0.5.16.tar.xz
  rpmbuild -ta munge-0.5.16.tar.xz

and install them from `~/rpmbuild/RPMS/x86_64/`.

With Munge_ 0.5.16 a configuration file ``/etc/sysconfig/munge`` is now used by the `munge` service,
and you may for example add this configuration to increase the number of threads to 10::

  OPTIONS="--key-file=/etc/munge/munge.key --num-threads=10"

Munge_ prior to version 0.5.15 has an issue_94_ *excessive logging of: "Suspended new connections while processing backlog"*
which might cause the `munged.log` file to **fill up the system disk**.

See also the page section :ref:`configure_maximum_number_of_open_files`.
It is highly recommended to increase the file limit in ``/etc/sysctl.conf`` significantly, for example:: 

  fs.file-max = 13107200

and do ``sysctl -p``.

.. _Munge_release: https://github.com/dun/munge/releases
.. _issue_94: https://github.com/dun/munge/issues/94

Increase number of threads in munged 0.5.11/0.5.13
-------------------------------------------------------

The **default** EL7/EL8/EL9 Munge_ versions 0.5.11 and 0.5.13 do not honor an options file,
see `Let systemd unit file use /etc/sysconfig/munge for munge options <https://github.com/dun/munge/pull/68>`_,
so this is how you can increase the number of threads in `munged`:

Copy the Systemd_ unit file::

  cp /usr/lib/systemd/system/munge.service /etc/systemd/system/munge.service

See `Modify systemd unit file without altering upstream unit file <https://serverfault.com/questions/840996/modify-systemd-unit-file-without-altering-upstream-unit-file>`_.
Then edit this line in the copied unit file::

  ExecStart=/usr/sbin/munged --num-threads 10

and restart the `munge` service::

  systemctl daemon-reload 
  systemctl restart munge

.. _Systemd: https://en.wikipedia.org/wiki/Systemd

Munge configuration and testing
-------------------------------

You may check the `munged` log file ``/var/log/munge/munged.log`` for any warnings or errors.

By default Munge_ uses an AES_ AES-128 cipher and SHA-256 HMAC_ (*Hash-based Message Authentication Code*).
Display these encryption options by::

  munge -C
  munge -M

.. _AES: https://en.wikipedia.org/wiki/Advanced_Encryption_Standard
.. _HMAC: https://en.wikipedia.org/wiki/Hash-based_message_authentication_code

On the **Head/Master node (only)** create a secret key to be used globally on every node (see the Munge_installation_ guide)::

  dd if=/dev/urandom bs=1 count=1024 > /etc/munge/munge.key   
  chown munge: /etc/munge/munge.key
  chmod 400 /etc/munge/munge.key

Alternatively use this command (slow)::

  /usr/sbin/create-munge-key -r

**NOTE:** For a discussion of using ``/dev/random`` in stead of ``/dev/urandom`` (pseudo-random) as recommended in the Munge_installation_ guide,
see `Myths about /dev/urandom <https://www.2uo.de/myths-about-urandom/>`_.

Securely propagate ``/etc/munge/munge.key`` (e.g., via SSH) to all other hosts within the same security realm::

  scp -p /etc/munge/munge.key hostXXX:/etc/munge/munge.key

Make sure to set the correct ownership and mode on all nodes::

  chown -R munge: /etc/munge/ /var/log/munge/
  chmod 0700 /etc/munge/ /var/log/munge/

Then enable and start the Munge_ service on all nodes::

  systemctl enable munge
  systemctl start  munge

Run some **tests** as described in the Munge_installation_ guide::

  munge -n 
  munge -n | unmunge          # Displays information about the Munge key
  munge -n | ssh somehost unmunge 
  remunge 

Build Slurm RPMs
================

See the Slurm_Quick_Start_ Administrator Guide, especially the section below this text::

  Optional Slurm plugins will be built automatically when the configure script detects that the required build requirements are present. 
  Build dependencies for various plugins and commands are denoted below: 

You must decide which Slurm_ plugins to activate in the RPM packages which you build, especially items such as:

* MySQL_ for accounting support
* cgroup_ Task Affinity
* Munge_ support
* Lua Support
* PAM support
* NUMA Affinity

.. _cgroup: https://slurm.schedmd.com/cgroups.html

Install prerequisites
---------------------

Slurm_ can be built with a number of **optional plugins**, each of which has some prerequisite library.
The Slurm_Quick_Start_ guide lists these in the section `Building and Installing Slurm <https://slurm.schedmd.com/quickstart_admin.html#build_install>`_.

Install required Slurm_ prerequisites, as well as several optional packages that enable the desired Slurm plugins::

  yum install rpm-build gcc python3 openssl openssl-devel pam-devel numactl numactl-devel hwloc hwloc-devel munge munge-libs munge-devel lua lua-devel readline-devel rrdtool-devel ncurses-devel gtk2-devel libibmad libibumad perl-Switch perl-ExtUtils-MakeMaker xorg-x11-xauth 

If you use the recommended ``AuthType=auth/munge`` in slurm.conf_ and slurmdbd.conf_, then you must also install::

  yum install munge munge-libs munge-devel

Furthermore, enable the EPEL_ repository::

  dnf install https://dl.fedoraproject.org/pub/epel/epel-release-latest-8.noarch.rpm  # EL8
  yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm  # EL7

and install the following EPEL_ packages::

  yum install libssh2-devel man2html

.. _EPEL: https://fedoraproject.org/wiki/EPEL

Optional prerequisites
........................

Certain Slurm tools and plugins require additional prerequisites **before** building Slurm:

1. IPMI_ library: If you want to implement power saving as described in the Power_Saving_Guide_ then you must install the FreeIPMI_ development library prerequisite::

     yum install freeipmi-devel

   See the presentation *Saving Power with Slurm by Ole Nielsen* in the Slurm_publications_ page.

   Since the official RPM repos may contain old versions, it may be necessary to build newer ``freeipmi`` RPMs from a development version (such as master),
   see the section on :ref:`ipmi_power_monitoring`.

2. If you want to build the **Slurm REST API** daemon named slurmrestd_ (from Slurm_ 20.02 and newer),
   then you must install these prerequisites also::

     yum install http-parser-devel json-c-devel libjwt-devel 

   The minimum version requirements are listed in the rest_quickstart_ guide:
   HTTP Parser (>= v2.6.0), LibYAML (optional, >= v0.2.5), JSON-C (>= v1.12.0).

   See the presentation *Slurm's REST API by Nathan Rini, SchedMD* in the Slurm_publications_ page.
   You may like to install the `jq - Command-line JSON processor <https://jqlang.github.io/jq/>`_ also::
   
     dnf install jq

3. Enable YAML_ command output (for example, ``sinfo --yaml``) by installing this library::

     dnf install libyaml-devel

   **Important:** The `libyaml` must be version 0.2.5 or later, see bug_17673_.
   The `libyaml` provided by EL8 or CentOS 7 is version 0.1.X and should not be used!
   The EL9 provides version 0.2.5.
   
.. _IPMI: https://en.wikipedia.org/wiki/Intelligent_Platform_Management_Interface
.. _slurmrestd: https://slurm.schedmd.com/rest.html
.. _rest_quickstart: https://slurm.schedmd.com/rest_quickstart.html#prereq
.. _Power_Saving_Guide: https://slurm.schedmd.com/power_save.html
.. _FreeIPMI: https://www.gnu.org/software/freeipmi/
.. _YAML: https://en.wikipedia.org/wiki/YAML
.. _bug_17673: https://bugs.schedmd.com/show_bug.cgi?id=17673

Install MariaDB database
------------------------

**Important:** If you want to enable accounting, you must install the MariaDB_ (a replacement for MySQL_) 
version 5.5 from CentOS7/RHEL7 packages **before** you build Slurm_ RPMs::

  yum install mariadb-server mariadb-devel

CentOS8/RHEL8 has the newer MariaDB_ version 10.3 which is installed by::

  dnf install mariadb-server mariadb-devel

**NOTICE:** Do not forget to configure the database as described in the :ref:`Slurm_database` page!

**Needs testing**: Alternatively, you can install the MariaDB_ version 10.3 database from the CentOS 7 Software Collections (SCL_) Repository::

  yum install centos-release-scl
  yum install rh-mariadb103-mariadb-server rh-mariadb103-mariadb-devel rh-mariadb103-mariadb rh-mariadb103-mariadb-backup

.. _SCL: https://wiki.centos.org/AdditionalResources/Repositories/SCL

Install the latest MariaDB version
..................................

**Optional:** Install the latest MariaDB_ version.  This is not required, and installation is somewhat involved.

For best results with RPM and DEB packages, use the `Repository Configuration Tool <https://downloads.mariadb.org/mariadb/repositories/>`_.
Configure the Yum repository as instructed and read the `MariaDB Yum page <https://mariadb.com/kb/en/library/yum/>`_.

For building Slurm you need to install these MariaDB 10.4 (or later) packages::

  yum install MariaDB-client MariaDB-shared MariaDB-devel

The MariaDB-shared package contains the required shared libraries for Slurm.
The *slurmdbd* server host will need to install also::

  yum install MariaDB-server MariaDB-backup

Install MySQL Python tools
--------------------------

If you will use Ansible_ to manage the database, Ansible_ needs this Python package::

  yum install MySQL-python    # CentOS7/RHEL7
  dnf install python3-mysql   # CentOS8/RHEL8

.. _Ansible: https://www.ansible.com/

Build Slurm packages
--------------------

Get the Slurm_ source code from the Slurm_download_ page.
At this point you must decide whether to build in Slurm plugins, for example, *mysql* for accounting (see above).

Set the version (for example, 23.11.7) and build Slurm_ RPM packages by::

  export VER=23.11.7
  rpmbuild -ta slurm-$VER.tar.bz2 --with mysql

Notes about the ``--with mysql`` option:

* The ``--with mysql`` option is not strictly necessary because the ``slurm-slurmdbd`` package will be built by default, 
  but using this option will catch the scenario where your forgot to install the ``mariadb-devel`` packages as described above, see also bug_8882_
  and this `mailing list posting <https://lists.schedmd.com/pipermail/slurm-users/2020-April/005245.html>`_.
* From Slurm 23.11 the ``--with mysql`` option has been removed, see the NEWS_ file.
  The default behavior now is to always require one of the sql development libraries.

Note about RHEL 9 (and derivatives):

* You must (currently) disable LTO_ in the SPEC file, see bug_14565_.

The RPM packages will typically be found in ``$HOME/rpmbuild/RPMS/x86_64/`` and should be installed on all relevant nodes.

.. _LTO: https://johanengelen.github.io/ldc/2016/11/10/Link-Time-Optimization-LDC.html
.. _bug_14565: https://bugs.schedmd.com/show_bug.cgi?id=14565
.. _MariaDB: https://mariadb.org/
.. _MySQL: https://www.mysql.com/
.. _bug_8882: https://bugs.schedmd.com/show_bug.cgi?id=8882

Build Slurm with optional features
.......................................

You may build Slurm_ packages including optional features:

* If you want to implement power saving as described in the Power_Saving_Guide_ then you can ensure that FreeIPMI_ gets built in by adding::

    rpmbuild <...> --with freeipmi

  This will be available from Slurm_ 23.11 where the presense of the ``freeipmi-devel`` package gets verified, see bug_17900_.

* If you want to build the **Slurm REST API** daemon named slurmrestd_ (from Slurm 20.02 and newer) you must add::

    rpmbuild <...> --with slurmrestd

* Enable YAML_ command output (for example, ``sinfo --yaml``)::

    rpmbuild <...> --with yaml

  Note that `libyaml` version 0.2.5 or later is required (see above), and this is only available starting with EL9,
  so the ``--with yaml`` option should **not** be used on EL8 and older releases!

.. _bug_17900: https://bugs.schedmd.com/show_bug.cgi?id=17900

Installing RPMs
===============

The RPMs to be installed on the head node, compute nodes, and slurmdbd_ node can vary by configuration, but here is a suggested starting point:

* **Head/Master** Node (where the slurmctld_ daemon runs), **Compute**, and **Login** nodes::

    export VER=23.11.7
    yum install slurm-$VER*rpm slurm-devel-$VER*rpm slurm-perlapi-$VER*rpm slurm-torque-$VER*rpm slurm-example-configs-$VER*rpm

  On the **master node** explicitly enable the *slurmctld* service::

    systemctl enable slurmctld

  The *slurm-torque* package could perhaps be omitted, but it does contain a useful ``/usr/bin/mpiexec`` wrapper script.

  Only if the **database service** will run on the Head/Master node:
  Install the database service RPM::

    export VER=23.11.7
    yum install slurm-slurmdbd-$VER*rpm

  Explicitly enable the service::

    systemctl enable slurmdbd

* On **Compute nodes** you may additionally install the slurm-pam_slurm RPM package to prevent rogue users from logging in::

    yum install slurm-pam_slurm-$VER*rpm

  You may consider this RPM as well with special PMIx libraries::

    yum install slurm-libpmi-$VER*rpm

  Explicitly enable the service::

    systemctl enable slurmd

* **Database-only** (slurmdbd_ service) node::

    export VER=23.11.7
    yum install slurm-$VER*rpm slurm-devel-$VER*rpm slurm-slurmdbd-$VER*rpm 

  Explicitly enable the service::

    systemctl enable slurmdbd

* Servers (from Slurm 20.02 and newer) which should offer slurmrestd_ (which can be used also by normal users) should install also this package::

    yum install slurm-slurmrestd-$VER*rpm

  The slurmctld_ server and the login nodes would typically include slurmrestd_.

Study the configuration information in the Quick Start Administrator_Guide_.

Update Systemd service files
----------------------------

On CentOS/RHEL 8 (EL8) systems the Slurm_ daemons may fail starting up at reboot, when Slurm_ is running in configless_ mode, 
apparently due to DNS failures.
This is actually due to the daemons starting too soon, before the network is fully online.
The issue is tracked in bug_11878_.

The solution (which may be solved in 21.08) is to modify the Systemd_ service files for slurmd_, slurmctld_ and slurmdbd_, for example::

  cp /usr/lib/systemd/system/slurmd.service /etc/systemd/system/

and edit the line in the *service* file::

  After=munge.service network.target remote-fs.target

into::

  After=munge.service network-online.target remote-fs.target

The *network-online* target will ensure that the network is online before starting the daemons.
Reboot the system to verify the daemon startup.

This modification may be beneficial on all Systemd_ systems, including EL8 and EL7.

.. _configless: https://slurm.schedmd.com/configless_slurm.html
.. _bug_11878: https://bugs.schedmd.com/show_bug.cgi?id=11878

Configure Slurm logging
-----------------------

  The Slurm_ logfile directory is undefined in the RPMs since you have to define it in slurm.conf_.
  See *SlurmdLogFile* and *SlurmctldLogFile* in the slurm.conf_ page, and *LogFile* in the slurmdbd.conf_ page.

  Check your logging configuration with::

    # grep -i logfile /etc/slurm/slurm.conf
    SlurmctldLogFile=/var/log/slurm/slurmctld.log
    SlurmdLogFile=/var/log/slurm/slurmd.log
    # scontrol show config | grep -i logfile
    SlurmctldLogFile        = /var/log/slurm/slurmctld.log
    SlurmdLogFile           = /var/log/slurm/slurmd.log
    SlurmSchedLogFile       = /var/log/slurm/slurmsched.log

  If log files are configured, you have to create the log file directory manually::

    mkdir /var/log/slurm
    chown slurm.slurm /var/log/slurm

  See the more general description in Bug_8272_.

.. _slurmctld: https://slurm.schedmd.com/slurmctld.html
.. _slurmdbd: https://slurm.schedmd.com/slurmdbd.html
.. _Administrator_Guide: https://slurm.schedmd.com/quickstart_admin.html
.. _Bug_8272: https://bugs.schedmd.com/show_bug.cgi?id=8272


.. _upgrading-slurm:

Upgrading Slurm
===============

New Slurm_ updates are released about every 9 months.
Follow the Upgrades_ instructions in the Slurm_Quick_Start_ page,
see also presentations by Tim Wickberg in the Slurm_publications_ page.
Pay attention to these statements: 

* You may upgrade at most by 2 major versions, see the Upgrades_ page:

  * Slurm daemons will support RPCs and state files from the **two previous major releases**
    (e.g. a version 23.11.x SlurmDBD will support slurmctld daemons and commands with a version of 23.11.x, 23.02.x or 22.05.x). 

* In other words, when changing the version to a higher release number (e.g from 22.05.x to 23.02.x) always upgrade the slurmdbd_ daemon first.
* Be mindful of your configured ``SlurmdTimeout`` and ``SlurmctldTimeout`` values.
* The recommended upgrade order is that versions may be mixed as follows::

    slurmdbd >= slurmctld >= slurmd >= commands

  Actually, ``commands`` mostly refers to the use of **login nodes** because all Slurm commands (sinfo_ , squeue_ etc.)
  are **not interoperable** with an older slurmctld_ version, as explained in bug_17418_, due to RPC changes!
  It is OK to upgrade Slurm_ on login nodes **after** slurmctld_ has been upgraded.
  The slurmd_ on compute nodes can be upgraded over a period of time, and older slurmd_ versions will continue to work with an upgraded slurmctld_.

If you use a database, also make sure to:

* Make a database dump (see :ref:`Slurm_database`) prior to the slurmdbd_ upgrade.
* Start the slurmdbd_ service manually after the upgrade in order to avoid timeouts (see bug_4450_).
  In stead of starting the slurmdbd_ service, it is most likely necessary to **start the daemon manually**.
  If you use the ``systemctl`` command, it is very likely to **exceed a system time limit** and kill slurmdbd_ before the database conversion has been completed.
  
  The recommended way to perform the slurmdbd_ database upgrade is therefore::

    time slurmdbd -D -vvv

  See further info below.

.. _Upgrades: https://slurm.schedmd.com/quickstart_admin.html#upgrade
.. _bug_4450: https://bugs.schedmd.com/show_bug.cgi?id=4450
.. _sinfo: https://slurm.schedmd.com/sinfo.html
.. _squeue: https://slurm.schedmd.com/squeue.html

This command can report current jobs that have been orphaned on the local cluster and are now runaway::

  sacctmgr show runawayjobs

Database upgrade from Slurm 17.02 and older
-------------------------------------------

If you are upgrading from **Slurm 17.02 and older** to **Slurm 17.11 and newer**, you must be extremely cautious about long database update times, 
since in Slurm 17.11 (and newer) some database structures were changed. 
Read the mailing list thread `Extreme long db upgrade 16.05.6 -> 17.11.3 <https://lists.schedmd.com/pipermail/slurm-users/2019-April/003178.html>`_,
where Lech Nieroda states:

* To sum it up, the issue affects those users who still have 17.02 or prior versions, use their distribution defaults for mysql/mariadb from RHEL6/CentOS6 and RHEL7/CentOS7, have millions of jobs in their database *and* would like to upgrade slurm without upgrading mysql.

The patch is also available from and is discussed in bug_6796_.

Furthermore, the `17.11 Release Notes <https://github.com/SchedMD/slurm/blob/slurm-17.11/RELEASE_NOTES>`_ states::

  NOTE FOR THOSE UPGRADING SLURMDBD: The database conversion process from SlurmDBD 16.05 or 17.02 may not work properly with MySQL 5.1 (as was the default version for RHEL 6).
  Upgrading to a newer version of MariaDB or MySQL is strongly encouraged to prevent this problem. 

and the `18.08 Release Notes <https://github.com/SchedMD/slurm/blob/slurm-18.08/RELEASE_NOTES>`_ added::

  NOTE FOR THOSE UPGRADING SLURMDBD:
  The database conversion process from SlurmDBD 16.05 or 17.02 may not work properly with MySQL 5.1 or 5.5 (as was the default version for RHEL 6).
  Upgrading to a newer version of MariaDB or MySQL is strongly encouraged to prevent this problem.

**NOTE:** MariaDB_ version 5.5 is the default database version delivered with RHEL7/CentOS 7!

More recent MariaDB_ versions 10.x can be downloaded from the MariaDB_repository_.
Some further information:

* This `MariaDB blog <https://mariadb.com/resources/blog/installing-mariadb-10-on-centos-7-rhel-7/>`_ explains the upgrade process from 5.5 to 10.x.
* `Installing MariaDB with yum/dnf <https://mariadb.com/kb/en/library/yum/>`_.

.. _MariaDB_repository: https://downloads.mariadb.org/mariadb/repositories/

The patch in the above thread should be applied **manually** to Slurm 17.11 before upgrading the database from 17.02 or 16.05 to 17.11 (**do not** upgrade by more than 2 Slurm releases!).

.. _bug_6796: https://bugs.schedmd.com/show_bug.cgi?id=6796

Upgrade of MySQL/MariaDB
------------------------

If you restore a database dump (see :ref:`Slurm_database`) onto a different server running a **newer MySQL/MariaDB version**, 
for example upgrading MySQL_ 5.1 on CentOS 6 to MariaDB_ 5.5 on CentOS 7,
there are some extra steps.

See `Upgrading from MySQL to MariaDB <https://mariadb.com/kb/en/library/upgrading-from-mysql-to-mariadb/>`_ 
about running the mysql_upgrade_ command::

  mysql_upgrade

whenever major (or even minor) version upgrades are made, or when migrating from MySQL_ to MariaDB_.

It may be necessary to restart the *mysqld* service or reboot the server after this upgrade (??).

.. _mysql_upgrade: https://mariadb.com/kb/en/library/mysql_upgrade/

Slurm database modifications required for MariaDB 10.2.1 and above
..................................................................

In MariaDB_ 10.2.1 and above there are some important changes to Slurm database tables,
please read instructions in the page :ref:`MariaDB_10.2.1_modifications` (with a reference to bug_15168_).
This has been resolved in Slurm 22.05.7.

.. _bug_15168: https://bugs.schedmd.com/show_bug.cgi?id=15168

Make a dry run database upgrade
-------------------------------

**Optional but strongly recommended**: You can test the database upgrade procedure before doing the real upgrade.

In order to verify and time the slurmdbd_ database upgrade you may make a dry_run_ upgrade for testing before actual deployment.

.. _dry_run: https://en.wikipedia.org/wiki/Dry_run_(testing)

Here is a suggested procedure:

1. Drain a compute node running the **current** Slurm_ version and use it for testing the database.

2. Install the database RPM packages and configure the database **EXACTLY** as described in the :ref:`Slurm_database` page::

     yum install mariadb-server mariadb-devel

   Configure the MySQL_/MariaDB_ database as described in the :ref:`Slurm_database` page.

3. Copy the latest database dump file (``/root/mysql_dump``, see :ref:`Slurm_database`) from the main server to the compute node.
   Load the dump file into the testing database::

     time mysql -u root -p < /root/mysql_dump

   If the dump file is in some compressed format::

     time zcat mysql_dump.gz | mysql -u root -p
     time bzcat mysql_dump.bz2 | mysql -u root -p

   The MariaDB_/MySQL_ *password* will be asked for.
   Reading in the database dump may take **many minutes** depending on the size of the dump file, the storage system speed, and the CPU performance.
   The ``time`` command will report the time usage.

   Verify the database contents on the compute node by making a new database dump and compare it to the original dump.

4. Select a suitable *slurm* user's **database password**.
   Now follow the :ref:`Slurm_accounting` page instructions (using -p to enter the database password)::

     # mysql -p
     grant all on slurm_acct_db.* TO 'slurm'@'localhost' identified by 'some_pass' with grant option;  ### WARNING: change the some_pass
     SHOW GRANTS;
     SHOW VARIABLES LIKE 'have_innodb';
     create database slurm_acct_db;
     quit;

   **WARNING:** Use the *slurm* database user's password **in stead of** ``some_pass``.

5. The following actions must be performed on the drained compute node.

   First stop the regular slurmd_ daemons on the compute node::

     systemctl stop slurmd

   Install the **OLD** (the cluster's current version, say, NN.NN) additional slurmdbd_ database RPMs as described above::

     VER=NN.NN
     yum install slurm-slurmdbd-$VER*rpm 

   Information about building RPMs is in the :ref:`Slurm_installation` page.

6. Make sure that the ``/etc/slurm`` directory exists (it is not needed in configless_ Slurm_ clusters)::

     $ ls -lad /etc/slurm
     drwxr-xr-x. 5 root root 4096 Feb 22 10:12 /etc/slurm

   Copy the configuration files from the main server to the compute node::

     /etc/slurm/slurm.conf
     /etc/slurm/slurmdbd.conf

   **Important**: Edit these files to replace the database server name by ``localhost`` so that all further actions take place on the compute node, **not** the *real* database server.

   Configure this in ``slurmdbd.conf``::

     DbdHost=localhost
     StorageHost=localhost
     StoragePass=<slurm database user password>  # See above

   Configure this in ``slurm.conf``::

     AccountingStorageHost=localhost

   Set up files and permissions::

     chown slurm: /etc/slurm/slurmdbd.conf
     chmod 600 /etc/slurm/slurmdbd.conf
     touch /var/log/slurm/slurmdbd.log
     chown slurm: /var/log/slurm/slurmdbd.log

7. Make sure that slurmdbd_ is running, and start it if necessary::

     systemctl status slurmdbd
     systemctl start slurmdbd

   Make some query to test slurmdbd_::

     sacctmgr show user -s

   If all is well, stop the slurmdbd_ before the upgrade below::

     systemctl stop slurmdbd

8. At this point you have a Slurm database server running an exact copy of your main Slurm database!

   Now it is time to do some testing.
   Update all Slurm_ RPMs to the new version (say, 23.11.7) built as shown above::

     export VER=23.11.7
     yum update slurm*$VER*.rpm

   If you use the auto_tmpdir_ RPM package, you have to remove it first because it will block the upgrade::

     yum remove auto_tmpdir

   See also `Temporary job directories <https://wiki.fysik.dtu.dk/niflheim/Slurm_configuration#temporary-job-directories>`_

.. _auto_tmpdir: https://github.com/University-of-Delaware-IT-RCI/auto_tmpdir

9. Perform and time the actual database upgrade::

     time slurmdbd -D -vvv

   and wait for the output::

     slurmdbd: debug2: accounting_storage/as_mysql: as_mysql_roll_usage: Everything rolled up

   and do a *Control-C*.
   Please note that the database table conversions may take **several minutes** or longer, depending on the size of the tables.

   Write down the timing information from the ``time`` command, since this will be the expected approximate time when you later perform the *real* upgrade.
   However, the storage system performance is important for all database operations, so timings may vary substantially between servers.

   Now start the service as usual::

     systemctl start slurmdbd

10. Make some query to test slurmdbd_::

     sacctmgr show user -s

   and make some other tests to verify that slurmdbd_ is responding correctly.

11. When all tests have been completed successfully, reinstall the compute node to its default installation.

Upgrading on EL8 and CentOS 7
---------------------------------

Let's assume that you have built the updated RPM packages for EL8 or CentOS 7 and copied them to the current directory so you can use ``yum`` on the files directly.

Prerequisites before upgrading
..............................

If you have installed the pdsh_ tool, there may be a module that has been linked against a specific library version ``libslurm.so.30``,
and ``yum`` will then refuse to update the ``slurm-xxx`` RPMs.
You must first do::

  yum remove pdsh-mod-slurm

and then later rebuild and reinstall pdsh-mod-slurm, see the :ref:`SLURM` page.

Upgrade slurmdbd
................

The upgrading steps for the slurmdbd_ host are:

1. Stop the slurmdbd_ service::

     systemctl stop slurmdbd

2. Make a dump of the MySQL_/Mariadb_ database (see :ref:`Slurm_database`).

   If also **upgrading** MariaDB_ to version 10.2.1 and above from an **older version**,
   there are some important changes to Slurm database tables,
   please read instructions in the page :ref:`MariaDB_10.2.1_modifications` (with a reference to bug_15168_).
   This has been resolved in Slurm 22.05.7.

3. Update all RPMs::

     export VER=23.11.7
     yum update slurm*$VER*.rpm

4. Start the slurmdbd_ service manually after the upgrade in order to avoid timeouts (see bug_4450_).
   In stead of starting the slurmdbd_ service, it is most likely necessary to **start the daemon manually**.
   If you use the ``systemctl`` command, it is very likely to **exceed a system time limit** and kill slurmdbd_ before the database conversion has been completed.
   Perform and time the actual database upgrade::

     time slurmdbd -D -vvv

   The completion of the database conversion may be printed as::

     slurmdbd: debug2: accounting_storage/as_mysql: as_mysql_roll_usage: Everything rolled up

   Then do a *Control-C*.
   Please note that the database table conversions may take **several minutes** or longer, depending on the size of the tables.

5. Restart the slurmdbd_ service normally::

     systemctl start slurmdbd

6. Make some query to test slurmdbd_::

     sacctmgr show user -s

**WARNING:** Newer versions of user commands like ``sinfo``, ``squeue`` etc. are **not interoperable** with an older 
slurmctld_ version, as explained in bug_17418_, due to RPC changes!

.. _bug_17418: https://bugs.schedmd.com/show_bug.cgi?id=17418

Upgrade slurmctld
.................

The upgrading steps for the slurmctld_ host are:

1. Change the timeout values in slurm.conf_ to::

     SlurmctldTimeout=3600
     SlurmdTimeout=3600 

   and copy ``/etc/slurm/slurm.conf`` to all nodes (not needed in configless_ Slurm_ clusters).
   Then reconfigure the running daemons and test the timeout and ``StateSaveLocation`` values::

     scontrol reconfigure
     scontrol show config | grep Timeout
     scontrol show config | grep StateSaveLocation

2. Stop the slurmctld_ service::

     systemctl stop slurmctld

3. Make a backup copy of the ``StateSaveLocation`` (check your configuration first) ``/var/spool/slurmctld`` directory:

     * Check the size of the ``StateSaveLocation`` and the backup destination to ensure there is sufficient disk space::

         du -sm /var/spool/slurmctld/
         df -h $HOME

     * Then make a tar-ball backup file::

         tar cf $HOME/var.spool.slurmctld.tar /var/spool/slurmctld/*

     * Make sure the contents of the tar-ball file look correct::

         less $HOME/var.spool.slurmctld.tar

4. Upgrade the RPMs, for example::

     export VER=23.11.7
     yum update slurm*$VER-*.rpm

5. Enable and restart the slurmctld_ service::

     systemctl enable slurmctld
     systemctl restart slurmctld

6. Check the cluster nodes' health using ``sinfo`` and check for any
   ``Nodes ... not responding`` errors in ``slurmctld.log``.
   It may be necessary to restart all the ``slurmd`` on all nodes::

     clush -ba systemctl restart slurmd

7. Restore the previous timeout values in slurm.conf_ (item 1.).

Note: The compute nodes should be upgraded at your earliest convenience.

Install slurm-libpmi
....................


On the compute nodes, only, you may consider this RPM as well with special PMIx libraries::

    yum install slurm-libpmi-$VER*rpm

Upgrade MPI applications
........................

MPI applications such as **OpenMPI** may be linked against the ``/usr/lib64/libslurm.so`` library.
In this context you must understand the remark in the Upgrades_ page::

  The libslurm.so version is increased every major release.
  So things like MPI libraries with Slurm integration should be recompiled.
  Sometimes it works to just symlink the old .so name(s) to the new one, but this has no guarantee of working.

In the thread `Need for recompiling openmpi built with --with-pmi? <https://groups.google.com/forum/#!msg/slurm-devel/oDoHPoAbiPQ/q9pQL2Uw3y0J>`_
it has been found that::

  It looks like it is the presence of lib64/libpmi2.la and lib64/libpmi.la that is the "culprit". They are installed by the slurm-devel RPM.
  Openmpi uses GNU libtool for linking, which finds these files, and follow their "dependency_libs" specification, thus linking directly to libslurm.so. 

Slurm_ version 16.05 and later no longer installs the libpmi*.la files.
This should mean that if your OpenMPI was built against Slurm_ 16.05 or later, there should be no problem (we think),
but otherwise you probably must rebuild your MPI applications and install them again at the same time that you upgrade the slurmd_ on the compute nodes.

To check for the presence of the "bad" files, go to your software build host and search::

  locate libpmi2.la 
  locate libpmi.la 

TODO: Find a way to read relevant MPI libraries like this example::

  readelf -d libmca_common_pmi.so 

Upgrade slurmd on nodes
.......................

First determine which Slurm_ version the nodes are running::

  clush -bg <partition> slurmd -V         # Using ClusterShell
  pdsh -g <partition> slurmd -V | dshbak  # Using PDSH

See the :ref:`SLURM` page about ClusterShell_ or PDSH_.

The **quick and usually OK procedure** would be to simply update the RPMs (here: version 23.11.7) on all nodes::

  clush -bw <nodelist> 'yum -y update /some/path/slurm*23.11.7-*.rpm'

This would automatically restart and enable slurmd_ on the nodes without any loss of running batch jobs.

For the compute nodes running slurmd_ the **safe procedure** could be:

1. Drain all desired compute nodes in a <nodelist>::

     scontrol update NodeName=<nodelist> State=draining Reason="Upgrading slurmd"

   Nodes will change from the *DRAINING* to the *DRAINED* state as the jobs are completed.
   Check which nodes have become *DRAINED*::

     sinfo -t drained

2. Stop the slurmd_ daemons on compute nodes::

     clush -bw <nodelist> systemctl stop slurmd

3. Update the RPMs (here: version 23.11.7) on nodes::

     clush -bw <nodelist> 'yum -y update /some/path/slurm*23.11.7-*.rpm'

   and make sure to install also the new ``slurm-slurmd`` and ``slurm-contribs`` packages.

   Now enable the slurmd_ service::

     clush -bw <nodelist> systemctl enable slurmd

4. For restarting slurmd_ there are two alternatives:

   a. Restart slurmd_ or simply reboot the nodes in the *DRAINED* state::

        clush -bw <nodelist> systemctl daemon-reload
        clush -bw <nodelist> systemctl restart slurmd
          or simply reboot:
        clush -bw <nodelist> shutdown -r now

   b. Reboot the nodes automatically as they become idle using the **RebootProgram** as configured in slurm.conf_, see the scontrol_ **reboot** option and explanation in the man-page::

        scontrol reboot [ASAP] [NodeList]

5. Return upgraded nodes to the IDLE state::

     scontrol update NodeName=<nodelist> State=resume 


Finally, restore the timeout values in slurm.conf_ to their defaults, for example::

     SlurmctldTimeout=600
     SlurmdTimeout=300 

and copy ``/etc/slurm/slurm.conf`` to all nodes. Then reconfigure the running daemons::

     scontrol reconfigure

Again, consult the Upgrades_ page before you start!

.. _slurmd: https://slurm.schedmd.com/slurmd.html

Migrate the slurmctld service to another server
=================================================

It may be required to migrate the slurmctld_ service to another server, for example,
when a major OS version update is needed or when the server must be migrated to another hardware.

With Slurm_ 23.11 and later, migrating the slurmctld_ service is quite easy,
and **does not** require to stop all running jobs,
since a major improvement is stated in the
`Release notes <https://github.com/SchedMD/slurm/blob/3dc79bd2eb1471b199159d2265618c6579f365c8/RELEASE_NOTES#L58>`_:

* Update slurmstepd processes with current SlurmctldHost settings, allowing for controller changes without draining all compute jobs. 

This change allows slurmstepd_ to receive an updated ``SlurmctldHost`` setting so that running jobs will report back to the new controller when they finish.
See the Slurm_publications_ presentation ``Slurm 23.02, 23.11, and Beyond`` by Tim Wickberg, SchedMD.

The migration process for Slurm_ 23.11 and later does not require to stop all running jobs,
the details are discussed in bug_20070_ :

1. Stop slurmctld_ on the old server.
2. Update the *Configless* DNS SRV record (see next section).
3. Migrate slurmctld_ to new machine:
   Copy the ``StateSaveLocation`` directory to the new host and make sure the permissions allow the *SlurmUser* to read and write it.
4. Update slurm.conf_ with new ``SlurmctldHost`` name.
   **Remember** to update the slurmdbd_ server's slurm.conf_ and the login nodes as well!
5. Start slurmctld_ on the new server.
6. If nodes are not communicating, run ``scontrol reconfigure`` or restart slurmd_ on the nodes.

If **not** using :ref:`configless-slurm-setup` you must distribute slurm.conf_ manually to all nodes in step 4.

.. _slurmstepd: https://slurm.schedmd.com/slurmstepd.html
.. _bug_20070: https://support.schedmd.com/show_bug.cgi?id=20070

Configless Slurm migration
--------------------------

When using :ref:`configless-slurm-setup` it is necessary to update the DNS SRV record to point to the new slurmctld_ server.
Start well in advance by changing the DNS SRV record's *TTL* to a small value such as 300 or 600 seconds (restart the *named* service).
After stopping slurmctld_ on the old ``SlurmctldHost``,
change the server name in the DNS SRV record (restart the *named* service).

Later, after the new ``SlurmctldHost`` has been tested successfully, restore the original DNS SRV record's *TTL* value.

Migrate slurmctld version <= 23.02
------------------------------------

In Slurm_ 23.02 and older, changes to ``SlurmctldHost`` are not possible with jobs running on the system.
Therefore you have to **stop all running jobs**, for example by making a :ref:`resource_reservation`.
Read the FAQ `How should I relocate the primary or backup controller? <https://slurm.schedmd.com/faq.html#controller>`_ with the procedure:

* Stop all Slurm daemons.
* Modify the ``SlurmctldHost`` values in the slurm.conf_ file.
* Distribute the updated slurm.conf_ file to all nodes.
  When using :ref:`configless-slurm-setup` see the section above.
* Copy the ``StateSaveLocation`` directory to the new host and make sure the permissions allow the SlurmUser to read and write it.
* Restart all Slurm daemons.

Log file rotation
=================

The Slurm_ log files may be stored in ``/var/log/slurm``, and they may grow rapidly on a busy system.
Especially the ``slurmctld.log`` file on the controller machine may grow very big.

Therefore you probably want to configure logrotate_ to administer your log files.
On RHEL and CentOS the logrotate_ configuration files are in the ``/etc/logrotate.d/`` directory.

Manual configuration is required because the SchedMD_ RPM files do not contain the logrotate setup, see bug_3904_ and bug_2215_ and bug_4393_.
See also the section *LOGGING* at the end of the slurm.conf_ page with an example logrotate script.

First install the relevant RPM::

  yum install logrotate

Create the following script ``/etc/logrotate.d/slurm`` which will rotate and compress the slurmctld_ log file on a weekly basis::

  /var/log/slurm/*.log {
        compress
        missingok
        nocopytruncate
        nodelaycompress
        nomail
        notifempty
        noolddir
        rotate 5
        sharedscripts
        size=5M
        create 640 slurm root
        postrotate
                pkill -x --signal SIGUSR2 slurmctld
                pkill -x --signal SIGUSR2 slurmd
                pkill -x --signal SIGUSR2 slurmdbd
                exit 0
        endscript
  }

**Warning:** Do **not** run ``scontrol reconfig`` or restart slurmctld_ to rotate the log files, since this will incur a huge overhead.

See the NEWS_ file for changes related to SIGUSR2_::

  Modify all daemons to re-open log files on receipt of SIGUSR2 signal. This is much than using SIGHUP to re-read the configuration file and rebuild various tables.

.. _logrotate: https://github.com/logrotate/logrotate
.. _bug_3402: https://bugs.schedmd.com/show_bug.cgi?id=3402
.. _bug_3904: https://bugs.schedmd.com/show_bug.cgi?id=3904
.. _bug_2215: https://bugs.schedmd.com/show_bug.cgi?id=2215
.. _bug_4393: https://bugs.schedmd.com/show_bug.cgi?id=4393
.. _bug_9264: https://bugs.schedmd.com/show_bug.cgi?id=9264
.. _NEWS: https://github.com/SchedMD/slurm/blob/master/NEWS
.. _SIGUSR2: https://www.gnu.org/software/libc/manual/html_node/Miscellaneous-Signals.html
