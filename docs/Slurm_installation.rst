.. _Slurm_installation:

================================
Slurm installation and upgrading
================================

.. Contents::

Jump to our top-level Slurm_ page: :ref:`SLURM`

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
.. _slurmctld: https://slurm.schedmd.com/slurmctld.html
.. _slurmdbd.conf: https://slurm.schedmd.com/slurmdbd.conf.html
.. _slurmdbd: https://slurm.schedmd.com/slurmdbd.html
.. _scontrol: https://slurm.schedmd.com/scontrol.html
.. _configless: https://slurm.schedmd.com/configless_slurm.html
.. _pdsh: https://github.com/grondo/pdsh
.. _ClusterShell: https://clustershell.readthedocs.io/en/latest/intro.html
.. _clush: https://clustershell.readthedocs.io/en/latest/tools/clush.html

Hardware optimization for the slurmctld master server
=====================================================

SchedMD_ recommends that the slurmctld_ server should have only a few, but **very fast CPU cores**, in order to ensure the best responsiveness.

The file system for ``/var/spool/slurmctld/`` should be mounted on the fastest possible disks (SSD or NVMe if possible).

Create global user accounts
===========================

There must be a uniform user and group name space (including UIDs and GIDs) across the cluster,
see the Slurm_Quick_Start_ Administrator Guide.

It is not necessary to permit user logins to the control hosts (*ControlMachine* or *BackupController*),
but the users and groups must be configured on those hosts.
To restrict user login by SSH, use the ``AllowUsers`` parameter in sshd_config_.

Slurm_ and Munge_ require consistent UID and GID across **all servers and nodes in the cluster**,
including the ``slurm`` and ``munge`` users.

**Very important: Avoid UID and GID values below 1000**,
as defined in the standard configuration file ``/etc/login.defs`` by the parameters ``UID_MIN, UID_MAX, GID_MIN, GID_MAX``,
see also the User_identifier_ page.

Create the users/groups for ``slurm`` and ``munge``, for example:

.. code-block:: bash

  export MUNGEUSER=1005
  groupadd -g $MUNGEUSER munge
  useradd  -m -c "MUNGE Uid 'N' Gid Emporium" -d /var/lib/munge -u $MUNGEUSER -g munge  -s /sbin/nologin munge
  export SlurmUSER=1001
  groupadd -g $SlurmUSER slurm
  useradd  -m -c "Slurm workload manager" -d /var/lib/slurm -u $SlurmUSER -g slurm  -s /bin/bash slurm

**Important:** Make sure that these same users are **created identically on all nodes**.
User/group creation must be done **prior to installing RPMs**
(which would create random UID/GID pairs if these users don't exist).

Please note that UIDs and GIDs up to 1000 are currently reserved for EL Linux system users,
see `this article <https://unix.stackexchange.com/questions/343445/user-id-less-than-1000-on-centos-7>`_
and the file ``/etc/login.defs``.

.. _User_identifier: https://en.wikipedia.org/wiki/User_identifier
.. _sshd_config: https://linux.die.net/man/5/sshd_config

Slurm authentication plugins
============================

For an overview of authentication see the Authentication_Plugins_ page.
The authentication method for communications between Slurm components is defined in slurm.conf_
by the AuthType_ parameter which can take one of these **mutually exclusive values:**

* ``auth/munge`` (Slurm's default plugin)
* ``auth/slurm`` (A new plugin from 23.11)

**WARNINGS:**

* All **Slurm daemons, jobs and commands must be terminated** in the entire cluster prior to changing the value of AuthType_!!
* Changes to the AuthType_ value will interrupt outstanding job steps and prevent them from completing. 

.. _Authentication_Plugins: https://slurm.schedmd.com/authentication.html
.. _AuthType: https://slurm.schedmd.com/slurm.conf.html#OPT_AuthType

Munge authentication service
---------------------------------

Munge_ is the default plugin used since the early days of Slurm_ (2003 and before).
The Munge_ authentication plugin identifies and authenticates the user originating a message within Slurm_.
It is recommended to read the Munge_installation_ guide and the Munge_wiki_.

On busy servers such as the slurmctld_ server,
the munged_ daemon could become a bottleneck,
see the presentation *Field Notes 5: From The Frontlines of Slurm Support* in the Slurm_publications_ page.
It is therefore recommended to increase the number of threads,
see the munged_ manual page, however, this is the default in the latest Munge_release_.
The issue is discussed in 
`excessive logging of: "Suspended new connections while processing backlog" <https://github.com/dun/munge/issues/94>`_.

.. _Munge: https://github.com/dun/munge
.. _Munge_installation: https://github.com/dun/munge/wiki/Installation-Guide
.. _Munge_wiki: https://github.com/dun/munge/wiki
.. _munged: https://github.com/dun/munge/wiki/Man-8-munged

.. _install_the_latest_munge_version:

Install the latest Munge version
....................................

We recommend to install the latest Munge_release_ RPMs (currently 0.5.16)
due to new features and bug fixes.
Munge_ prior to version 0.5.15 has an issue_94_ *excessive logging of: "Suspended new connections while processing backlog"*
which might cause the `munged.log` file to **fill up the system disk**.

See also the page :ref:`configure_maximum_number_of_open_files`
where it is **highly recommended** to increase the ``fs.file-max``
limit in ``/etc/sysctl.conf`` significantly on **all Slurm compute nodes**.

Build Munge_ RPM packages by:

.. code-block:: bash

  wget https://github.com/dun/munge/releases/download/munge-0.5.16/munge-0.5.16.tar.xz
  rpmbuild -ta munge-0.5.16.tar.xz

and install them from the directory ``~/rpmbuild/RPMS/x86_64/``.

With Munge_ 0.5.16 a new configuration file ``/etc/sysconfig/munge`` is used by the munged_ service.
It is a good idea to increase the number of threads from 2 to 10 by::

  OPTIONS="--key-file=/etc/munge/munge.key --num-threads=10"

.. _Munge_release: https://github.com/dun/munge/releases
.. _issue_94: https://github.com/dun/munge/issues/94

Install in-distro version of Munge
....................................

The EL8 and EL9 distributions contain Munge_ RPM packages version 0.5.13,
but it is preferred to install the latest version as discussed above.
The in-distro packages may be installed by::

  dnf install munge munge-libs munge-devel

Only in case you have decided to use the **default** EL8/EL9 Munge_ version 0.5.13,
note that this version does have an options file,
see `Let systemd unit file use /etc/sysconfig/munge for munge options <https://github.com/dun/munge/pull/68>`_.

You may want to increase the number of threads in munged_ as follows.
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
....................................

By default Munge_ uses an AES_ AES-128 cipher and SHA-256 HMAC_ (*Hash-based Message Authentication Code*).
Display these encryption options by::

  munge -C
  munge -M

.. _AES: https://en.wikipedia.org/wiki/Advanced_Encryption_Standard
.. _HMAC: https://en.wikipedia.org/wiki/Hash-based_message_authentication_code

Generate the Munge key
......................

On the **Head node (only)** create a **global secret key** file ``/etc/munge/munge.key`` with the mungekey_ command
(see the Munge_installation_ guide) to be used on **every** node in the cluster::

  mungekey --create --verbose

.. _mungekey: https://github.com/dun/munge/wiki/Man-8-mungekey

Alternatively use this command::

  dd if=/dev/urandom bs=1 count=1024 > /etc/munge/munge.key   
  chown munge: /etc/munge/munge.key
  chmod 400 /etc/munge/munge.key

**NOTE:** For a discussion of using ``/dev/random`` in stead of ``/dev/urandom`` (pseudo-random) as recommended in the Munge_installation_ guide,
see `Myths about /dev/urandom <https://www.2uo.de/myths-about-urandom/>`_.

Securely propagate ``/etc/munge/munge.key`` (e.g., via SSH) to all other hosts within the same security realm::

  scp -p /etc/munge/munge.key hostXXX:/etc/munge/munge.key

For multiple destination hosts use the clush_ command::

  clush -w <node-list> --copy /etc/munge/munge.key --dest /etc/munge/munge.key

Make sure to set the correct ownership and mode on all nodes:

.. code-block:: bash

  chown -R munge: /etc/munge/ /var/log/munge/
  chmod 0700 /etc/munge/ /var/log/munge/

Test the Munge service
.........................

Enable and start the Munge_ service on all nodes::

  systemctl enable munge
  systemctl start  munge

Run some **tests** as described in the Munge_installation_ guide::

  munge -n 
  munge -n | unmunge          # Displays information about the Munge key
  munge -n | ssh somehost unmunge 
  remunge 

You may check the Munge_ log file ``/var/log/munge/munged.log`` for any warnings or errors.

The new auth/slurm authentication plugin
--------------------------------------------------

For an overview of authentication see the Authentication_Plugins_ page.
Beginning with version 23.11, Slurm_ has its own ``auth/slurm`` authentication plugin 
(as an alternative to the Munge_ plugin) that can create and validate credentials.
It validates that the requests come from legitimate UIDs and GIDs on other hosts with matching users and groups.

**WARNING:** All **Slurm daemons, jobs and commands must be terminated**
in the entire cluster prior to changing the value of AuthType_ in slurm.conf_!!

For the ``auth/slurm`` authentication you must have a shared key file ``/etc/slurm/slurm.key``,
but beginning with version 24.05 you may alternatively create a ``/etc/slurm/slurm.jwks`` file with multiple keys as shown below.
**Note:** The key file(s) must be distributed securely to all nodes in the cluster (for example using clush_).

The ticket bug_21683_ discusses ``How to get started with auth/slurm?``

.. _bug_21683: https://support.schedmd.com/show_bug.cgi?id=21683

Single slurm.key setup
.........................

For the authentication to happen correctly you must have a shared key file ``/etc/slurm/slurm.key``
on the servers running slurmctld_, slurmdbd_, as well as slurmd_ on the nodes.

You can create a ``/etc/slurm/slurm.key`` file by entering your own text or by generating random data::

  dd if=/dev/urandom of=/etc/slurm/slurm.key bs=1024 count=1

The key file must be owned by ``SlurmUser`` and must not be readable or writable by other users::

  chown slurm:slurm /etc/slurm/slurm.key
  chmod 600 /etc/slurm/slurm.key

Distribute the ``/etc/slurm/slurm.key`` file to all nodes in the cluster (the clush_ command may be used).
The key file needs to be on the machines running slurmctld_, slurmdbd_, slurmd_ and sackd_.
Define these authentication type parameters in slurm.conf_:

* AuthType = auth/slurm
* CredType = cred/slurm

and in slurmdbd.conf_:

* AuthType = auth/slurm

Multiple key setup
.........................

Beginning with version 24.05, you may alternatively create a ``/etc/slurm/slurm.jwks`` file with multiple keys defined,
see the Authentication_Plugins_ page.

The ``slurm.jwks`` file aids with key rotation, as the cluster does not need to be restarted at once when a key is rotated.
Instead, an ``scontrol reconfigure`` is sufficient.
There are no slurm.conf_ parameters required to use the slurm.jwks file, instead, the presence of the slurm.jwks file enables this functionality.
If the slurm.jwks is not present or cannot be read, the cluster defaults to the slurm.key file.

The structure of ``/etc/slurm/slurm.jwks`` is documented as::

  {
    "keys": [
      {
        "alg": "HS256",
        "kty": "oct",
        "kid": "key-identifier-2",
        "k": "Substitute me!!",
        "use": "default"
      }
    ]
  }

Protect the file::

  chown slurm:slurm /etc/slurm/slurm.jwks
  chmod 600 /etc/slurm/slurm.jwks

To generate a random secret key "k" string this command may be helpful::

  dd if=/dev/urandom count=1 bs=1024 | base64 -w 0 

Build Slurm RPMs
================

Read the Slurm_Quick_Start_ Administrator Guide, especially the section below this text::

  Optional Slurm plugins will be built automatically when the configure script detects that the required build requirements are present. 
  Build dependencies for various plugins and commands are denoted below: 

You must decide which Slurm_ plugins to activate in the RPM packages which you build, especially items such as:

* cgroup_ Task Affinity
* cgroup_v2_ plugin (requires `dbus-devel` and `libbpf` libraries)
* Munge_ support
* Lua Support
* PAM support
* NUMA Affinity

.. _cgroup: https://slurm.schedmd.com/cgroups.html
.. _cgroup_v2: https://slurm.schedmd.com/cgroup_v2.html

Install prerequisites
---------------------

You will need to enable the repositories ``PowerTools`` (EL8) or ``CRB`` (EL9),
see the discussion of Rocky_Linux_Repositories_, and then enable also the EPEL_ repository::

  dnf config-manager --set-enabled powertools # EL8
  dnf config-manager --set-enabled crb        # EL9
  dnf install epel-release
  dnf clean all

Slurm_ can be built with a number of **optional plugins**, each of which has some prerequisite library.
The Slurm_Quick_Start_ guide lists these in the section `Building and Installing Slurm <https://slurm.schedmd.com/quickstart_admin.html#build_install>`_.

Install required Slurm_ prerequisite packages, as well as several optional packages that enable the desired Slurm_ plugins::

  dnf install mariadb-server mariadb-devel
  dnf install rpm-build gcc python3 openssl openssl-devel pam-devel numactl numactl-devel hwloc hwloc-devel lua lua-devel readline-devel rrdtool-devel ncurses-devel gtk2-devel libibmad libibumad perl-Switch perl-ExtUtils-MakeMaker xorg-x11-xauth dbus-devel libbpf bash-completion

We recommend to install_the_latest_munge_version_ (currently 0.5.16) because of bug fixes improving the EL8/EL9 version::

  dnf install munge munge-libs munge-devel

Install the following packages from EPEL_::

  dnf install libssh2-devel man2html

.. _Rocky_Linux_Repositories: https://wiki.rockylinux.org/rocky/repo/
.. _EPEL: https://fedoraproject.org/wiki/EPEL

Optional prerequisites
........................

Certain Slurm_ tools and plugins require additional prerequisites **before** building Slurm_ RPM packages:

1. IPMI_ library: If you want to implement power saving as described in the Power_Saving_Guide_ then you must install the FreeIPMI_ development library prerequisite::

     dnf install freeipmi-devel

   See the presentation *Saving Power with Slurm by Ole Nielsen* in the Slurm_publications_ page,
   and the section on :ref:`ipmi_power_monitoring`.

   To build your own EL8/EL9 RPMs with Systemd support from the source tar-ball::

      rpmbuild -ta --with systemd freeipmi-1.6.15.tar.gz

2. If you want to build the **Slurm REST API** daemon named slurmrestd_,
   then you must install these prerequisites also::

     dnf install http-parser-devel json-c-devel libjwt-devel 

   **Notice:** The minimum version requirements are listed in the rest_quickstart_ guide:

   * HTTP Parser (>= v2.6.0),
   * LibYAML (optional, >= v0.2.5),
   * JSON-C (>= v1.12.0).

   See the presentation *Slurm's REST API by Nathan Rini, SchedMD* in the Slurm_publications_ page.
   You may like to install the `jq - Command-line JSON processor <https://jqlang.github.io/jq/>`_ also::
   
     dnf install jq

3. For EL9 only: Enable YAML_ command output (for example, ``sinfo --yaml``) by installing the ``libyaml-devel`` library:

   * **Important**: The `libyaml` **must** be version >= 0.2.5, see bug_17673_,
     and EL9 provides this version.
     The `libyaml` provided by EL8 is version 0.1.X and **should not be used**!
   
.. _IPMI: https://en.wikipedia.org/wiki/Intelligent_Platform_Management_Interface
.. _slurmrestd: https://slurm.schedmd.com/rest.html
.. _rest_quickstart: https://slurm.schedmd.com/rest_quickstart.html#prereq
.. _Power_Saving_Guide: https://slurm.schedmd.com/power_save.html
.. _FreeIPMI: https://www.gnu.org/software/freeipmi/
.. _YAML: https://en.wikipedia.org/wiki/YAML
.. _bug_17673: https://bugs.schedmd.com/show_bug.cgi?id=17673

Install MariaDB database
------------------------

First install the MariaDB_ database version 10.3::

  dnf install mariadb-server mariadb-devel

**NOTICE:** Do not forget to configure the database as described in the :ref:`Slurm_database` page!

If you plan to use Ansible_ to manage the database, it will require this Python package::

  dnf install python3-mysql (EL8)
  dnf install python3-PyMySQL (EL9)

.. _Ansible: https://www.ansible.com/

Build Slurm packages
--------------------

Get the Slurm_ source code from the Slurm_download_ page.

Set the version (for example, 24.11.1 and build Slurm_ RPM packages by::

  export VER=24.11.1
  rpmbuild -ta slurm-$VER.tar.bz2 --with mysql

Notes about the ``--with mysql`` option:

* The ``--with mysql`` option is not strictly necessary because the ``slurm-slurmdbd`` package will be built by default, 
  but using this option will catch the scenario where your forgot to install the ``mariadb-devel`` packages as described above, see also bug_8882_
  and this `mailing list posting <https://lists.schedmd.com/pipermail/slurm-users/2020-April/005245.html>`_.
* From Slurm_ 23.11 the ``--with mysql`` option has been removed, see the NEWS_ file.
  The default behavior now is to always require one of the sql development libraries.

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

* If you want to build the **Slurm REST API** daemon named slurmrestd_ you must add::

    rpmbuild <...> --with slurmrestd

* For EL9 only: Enable YAML_ command output (for example, ``sinfo --yaml``)::

    rpmbuild <...> --with yaml

  **Notice** that `libyaml` version 0.2.5 or later is required (see above), and this is only available starting with EL9,
  so the ``--with yaml`` option should **not** be used on EL8 and older releases!

.. _bug_17900: https://bugs.schedmd.com/show_bug.cgi?id=17900

Installing RPMs
===============

Study the configuration information in the Quick Start Administrator_Guide_.
The RPMs to be installed on the head node, compute nodes, and slurmdbd_ node can vary by configuration, but here is a suggested starting point:

* **Head** node where the slurmctld_ daemon runs::

    export VER=24.11.1
    dnf install slurm-$VER*rpm slurm-devel-$VER*rpm slurm-perlapi-$VER*rpm slurm-torque-$VER*rpm slurm-example-configs-$VER*rpm
    systemctl enable slurmctld

  The following must be done on the Head node because the RPM installation does not include this.
  Create the spool and log directories and make them owned by the slurm user::

    mkdir /var/spool/slurmctld /var/log/slurm
    chown slurm: /var/spool/slurmctld /var/log/slurm
    chmod 755 /var/spool/slurmctld /var/log/slurm

  Create log files::

    touch /var/log/slurm/slurmctld.log 
    chown slurm: /var/log/slurm/slurmctld.log 

  Servers which should offer slurmrestd_ should install also this package::

    dnf install slurm-slurmrestd-$VER*rpm

  The *slurm-torque* package could perhaps be omitted, but it does contain a useful ``/usr/bin/mpiexec`` wrapper script.

* On **Compute nodes** install slurmd_ and possibly also the *slurm-pam_slurm* RPM package to prevent rogue users from logging in::

    export VER=24.11.1
    dnf install slurm-slurmd-$VER*rpm slurm-pam_slurm-$VER*rpm
    systemctl enable slurmd

  The following must be done on each compute node because the RPM installation does not include this.
  Create the slurmd_ spool and log directories and make the correct ownership::

    mkdir /var/spool/slurmd /var/log/slurm
    chown slurm: /var/spool/slurmd  /var/log/slurm
    chmod 755 /var/spool/slurmd  /var/log/slurm

  Create log files::

    touch /var/log/slurm/slurmd.log 
    chown slurm: /var/log/slurm/slurmd.log 

  You may consider this RPM as well with special PMIx libraries::

    dnf install slurm-libpmi-$VER*rpm

* **Database** (slurmdbd_ service) node::

    export VER=24.11.1
    dnf install slurm-$VER*rpm slurm-devel-$VER*rpm slurm-slurmdbd-$VER*rpm 

  Create the slurmdbd_ log directory and log file, and make the correct ownership and permissions::

    mkdir /var/log/slurm
    touch /var/log/slurm/slurmdbd.log 
    chown slurm: /var/log/slurm /var/log/slurm/slurmdbd.log
    chmod 750 /var/log/slurm
    chmod 640 /var/log/slurm/slurmdbd.log 

  Explicitly enable the slurmdbd_ service::

    systemctl enable slurmdbd

* On **Login nodes** install these packages::

    export VER=24.11.1
    dnf install slurm-$VER*rpm slurm-devel-$VER*rpm slurm-contribs-$VER*rpm slurm-perlapi-$VER*rpm 

.. _configure-slurm-logging:

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

.. _Administrator_Guide: https://slurm.schedmd.com/quickstart_admin.html
.. _Bug_8272: https://bugs.schedmd.com/show_bug.cgi?id=8272

.. _upgrading-slurm:

Upgrading Slurm
===============

New Slurm_ updates are released about every 6 months (the interval was 9 months prior to Slurm_ 24.05).
Follow the Upgrades_ instructions in the Slurm_Quick_Start_ page,
see also presentations by Tim Wickberg in the Slurm_publications_ page.
Pay attention to these statements: 

* You may upgrade at most by 2 major versions (3 versions starting from 24.11), see the Upgrades_ page.
* When changing the version to a higher release number (e.g from 22.05.x to 23.02.x) **always** upgrade the slurmdbd_ daemon first.
* Be mindful of your configured ``SlurmdTimeout`` and ``SlurmctldTimeout`` values:
  Increase/decrease them as needed.
* The recommended upgrade order is that versions may be mixed as follows::

    slurmdbd >= slurmctld >= slurmd >= commands

  Actually, the term "commands" here primarily refers to the **login nodes**,
  because all Slurm_ commands (sinfo_ , squeue_ etc.) are **not interoperable** with an older slurmctld_ version,
  as explained in bug_17418_, due to RPC_ changes!
  It is OK to upgrade Slurm_ on login nodes **after** slurmctld_ has been upgraded.
  The slurmd_ on compute nodes can be upgraded over a period of time,
  and older slurmd_ versions will continue to work with an upgraded slurmctld_,
  although it is recommended to upgrade as soon as possible.
* The following command can report current jobs that have been orphaned on the local cluster and are now runaway::

    sacctmgr show runawayjobs

Regarding the Slurm_ database, also make sure to:

* Make a database dump (see :ref:`Slurm_database`) prior to the slurmdbd_ upgrade.
* Start the slurmdbd_ service manually after the upgrade in order to avoid timeouts (see bug_4450_).
  In stead of starting the slurmdbd_ Systemd_ service, it **strongly recommended to start the slurmdbd daemon manually**.
  If you use the ``systemctl`` command, it is very likely to **exceed a system time limit** and kill slurmdbd_ before the database conversion has been completed!
  
  The recommended way to perform the slurmdbd_ database upgrade is therefore::

    time slurmdbd -D -vvv

  See further info below.

.. _Upgrades: https://slurm.schedmd.com/upgrades.html
.. _bug_4450: https://bugs.schedmd.com/show_bug.cgi?id=4450
.. _sinfo: https://slurm.schedmd.com/sinfo.html
.. _squeue: https://slurm.schedmd.com/squeue.html
.. _RPC: https://en.wikipedia.org/wiki/Remote_procedure_call

Upgrade of MySQL/MariaDB
------------------------

If you restore a database dump (see :ref:`Slurm_database`) onto a different server running a **newer MySQL/MariaDB version**, 
there are some extra steps.

See `Upgrading from MySQL to MariaDB <https://mariadb.com/kb/en/library/upgrading-from-mysql-to-mariadb/>`_ 
about running the mysql_upgrade_ command::

  mysql_upgrade

whenever major (or even minor) version upgrades are made, or when migrating from MySQL_ to MariaDB_.

It may be necessary to restart the ``mysqld`` service or reboot the server after this upgrade (??).

.. _mysql_upgrade: https://mariadb.com/kb/en/library/mysql_upgrade/

Make a dry run database upgrade
-------------------------------

**Optional but strongly recommended**: You can test the database upgrade procedure before doing the real upgrade.

In order to verify and time the slurmdbd_ database upgrade you may make a dry_run_ upgrade for testing before actual deployment.

.. _dry_run: https://en.wikipedia.org/wiki/Dry_run_(testing)

Here is a suggested procedure:

1. Drain a compute node running the **current** Slurm_ version and use it for testing the database.

2. Install the database RPM packages and configure the database **EXACTLY** as described in the :ref:`Slurm_database` page::

     dnf install mariadb-server mariadb-devel

3. Configure the MySQL_/MariaDB_ database as described in the :ref:`Slurm_database` page.

4. Copy the latest database dump file (``/root/mysql_dump``, see :ref:`Slurm_database`) from the main server to the compute node.
   Load the dump file into the testing database::

     time mysql -u root -p < /root/mysql_dump

   If the dump file is in some compressed format::

     time zcat mysql_dump.gz | mysql -u root -p
     time bzcat mysql_dump.bz2 | mysql -u root -p

   The MariaDB_/MySQL_ *password* will be asked for.
   Reading in the database dump may take **many minutes** depending on the size of the dump file, the storage system speed, and the CPU performance.
   The ``time`` command will report the time usage.

   Verify the database contents on the compute node by making a new database dump and compare it to the original dump.

5. Select a suitable *slurm* user's **database password**.
   Now follow the :ref:`Slurm_accounting` page instructions (using -p to enter the database password)::

     # mysql -p
     grant all on slurm_acct_db.* TO 'slurm'@'localhost' identified by 'some_pass' with grant option;  ### WARNING: change the some_pass
     SHOW GRANTS;
     SHOW VARIABLES LIKE 'have_innodb';
     create database slurm_acct_db;
     quit;

   **WARNING:** Use the *slurm* database user's password **in stead of** ``some_pass``.

6. The following actions must be performed on the drained compute node.

   First stop the regular slurmd_ daemons on the compute node::

     systemctl stop slurmd

   Install the **OLD** (the cluster's current version, say, NN.NN) additional slurmdbd_ database RPMs as described above::

     VER=NN.NN
     dnf install slurm-slurmdbd-$VER*rpm 

   Information about building RPMs is in the :ref:`Slurm_installation` page.

7. Make sure that the ``/etc/slurm`` directory exists (it is not needed in configless_ Slurm_ clusters)::

     $ ls -lad /etc/slurm
     drwxr-xr-x. 5 root root 4096 Feb 22 10:12 /etc/slurm

   Copy the configuration files from the main server to the compute node::

     /etc/slurm/slurmdbd.conf
     /etc/slurm/slurm.conf

   **Important**: Edit these files to replace the database server name by ``localhost`` so that all further actions take place on the compute node, **not** the *real* database server.

   Configure this in ``slurmdbd.conf``::

     DbdHost=localhost
     StorageHost=localhost
     StoragePass=<slurm database user password>  # See above

   and configure this in ``slurm.conf``::

     AccountingStorageHost=localhost

   Set up files and permissions::

     chown slurm: /etc/slurm/slurmdbd.conf
     chmod 600 /etc/slurm/slurmdbd.conf
     touch /var/log/slurm/slurmdbd.log
     chown slurm: /var/log/slurm/slurmdbd.log

8. Make sure that slurmdbd_ is running, and start it if necessary::

     systemctl status slurmdbd
     systemctl start slurmdbd

   Make some query to test slurmdbd_::

     sacctmgr show user -s

   If all is well, stop the slurmdbd_ before the upgrade below::

     systemctl stop slurmdbd

9. At this point you have a Slurm_ database server running an exact copy of your main Slurm_ database!

   Now it is time to do some testing.
   Update all Slurm_ RPMs to the new version (say, 24.11.1 built as shown above)::

     export VER=24.11.1
     dnf update slurm*$VER*.rpm

   Optional:
   In case you use the auto_tmpdir_ RPM package, you have to remove it first because it will block the Slurm_ upgrade,
   see also `Temporary job directories <https://wiki.fysik.dtu.dk/niflheim/Slurm_configuration#temporary-job-directories>`_.

.. _auto_tmpdir: https://github.com/University-of-Delaware-IT-RCI/auto_tmpdir

10. Perform and time the actual database upgrade::

     time slurmdbd -D -vvv

   and wait for the output::

     slurmdbd: debug2: accounting_storage/as_mysql: as_mysql_roll_usage: Everything rolled up

   and do a Control-C_.
   Please note that the database table conversions may take **several minutes** or longer, depending on the size of the tables.

   Write down the timing information from the ``time`` command, since this will be the expected approximate time when you later perform the *real* upgrade.
   However, the storage system performance is important for all database operations, so timings may vary substantially between servers.

   Now start the service as usual::

     systemctl start slurmdbd

11. Make some query to test slurmdbd_::

     sacctmgr show user -s

   and make some other tests to verify that slurmdbd_ is responding correctly.

12. When all tests have been completed successfully, reinstall the compute node to its default installation.

Upgrading on EL8 and EL9
-------------------------

Let's assume that you have built the updated RPM packages for EL8 or EL9
and copied them to the current directory so you can use ``dnf`` commands on the files directly.

Upgrade slurmdbd
................

The upgrading steps for the slurmdbd_ host are:

1. Stop the slurmdbd_ service::

     systemctl stop slurmdbd

2. Make a dump of the MySQL_/Mariadb_ database (see :ref:`Slurm_database`).

3. Update all RPMs::

     export VER=24.11.1
     dnf update slurm*$VER*.rpm

4. Start the slurmdbd_ service **manually** after the upgrade in order to avoid Systemd_ timeouts (see bug_4450_).
   In stead of starting the slurmdbd_ service with ``systemctl``, it is most likely necessary to **start the daemon manually**.
   If you were to use the ``systemctl`` command, it is very likely to **exceed a system time limit** and kill slurmdbd_ before the database conversion has been completed.

   Perform and time the actual database upgrade::

     time slurmdbd -D -vvv

   The completion of the database conversion may be printed with text like::

     slurmdbd: debug2: accounting_storage/as_mysql: as_mysql_roll_usage: Everything rolled up

   Then stop slurmdbd_ with a Control-C_.
   Please note that the database table conversions may take **a number of minutes** or longer,
   depending on the size of the database tables.

5. Now start the slurmdbd_ service normally::

     systemctl start slurmdbd

6. Make some database query to test slurmdbd_::

     sacctmgr show user -s

**WARNING:** Newer versions of user commands like ``sinfo``, ``squeue`` etc. are **not interoperable** with an older 
slurmctld_ version, as explained in bug_17418_, due to RPC_ changes!

.. _bug_17418: https://bugs.schedmd.com/show_bug.cgi?id=17418
.. _Control-C: https://en.wikipedia.org/wiki/Control-C

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

     export VER=24.11.1
     dnf update slurm*$VER-*.rpm

5. Enable and restart the slurmctld_ service::

     systemctl enable slurmctld
     systemctl restart slurmctld

6. Check the cluster nodes' health using ``sinfo`` and check for any
   ``Nodes ... not responding`` errors in ``slurmctld.log``.
   It may be necessary to restart all the ``slurmd`` on all nodes,
   for example, using the clush_ command (see the :ref:`SLURM` page about ClusterShell_)::

     clush -ba systemctl restart slurmd

7. Restore the previous timeout values in slurm.conf_ (item 1.).

Note: The compute nodes should be upgraded at your earliest convenience.

Install slurm-libpmi
....................


On the compute nodes, only, you may consider this RPM as well with special PMIx libraries::

    dnf install slurm-libpmi-$VER*rpm

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

First determine which Slurm_ version the nodes are running,
for example, using the clush_ command (see the :ref:`SLURM` page about ClusterShell_)::

  clush -bg <partition> slurmd -V

The **quick and usually OK procedure** would be to simply update the RPMs (here: version 24.11.1 on all nodes::

  clush -bw <nodelist> 'dnf -y update /some/path/slurm*24.11.1*.rpm'

This would automatically restart and enable slurmd_ on the nodes without any loss of running batch jobs.

For the compute nodes running slurmd_ the **safe procedure** could be:

1. Drain all desired compute nodes in a <nodelist>::

     scontrol update NodeName=<nodelist> State=draining Reason="Upgrading slurmd"

   Nodes will change from the *DRAINING* to the *DRAINED* state as the jobs are completed.
   Check which nodes have become *DRAINED*::

     sinfo -t drained

2. Stop the slurmd_ daemons on compute nodes::

     clush -bw <nodelist> systemctl stop slurmd

3. Update the RPMs (here: version 24.11.1 on nodes::

     clush -bw <nodelist> 'dnf -y update /some/path/slurm*24.11.1*.rpm'

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

and copy ``/etc/slurm/slurm.conf`` to all nodes (not needed in configless_ Slurm_ clusters).
Then reconfigure the running daemons::

     scontrol reconfigure

Again, consult the Upgrades_ page before you start!

.. _slurmd: https://slurm.schedmd.com/slurmd.html

Migrate the slurmctld service to another server
=================================================

It may be required to migrate the slurmctld_ service to another server, for example,
when a major OS version update is needed, or when the server must be migrated to another hardware.

From Slurm_ 23.11 and later, migrating the slurmctld_ service is quite easy,
and **does not** require to stop all running jobs,
since a major improvement is stated in the
`Release notes <https://github.com/SchedMD/slurm/blob/3dc79bd2eb1471b199159d2265618c6579f365c8/RELEASE_NOTES#L58>`_:

* *Update slurmstepd processes with current SlurmctldHost settings, allowing for controller changes without draining all compute jobs.*

This change allows slurmstepd_ to receive an updated ``SlurmctldHost`` setting so that running jobs will report back to the new controller when they finish.
See the Slurm_publications_ presentation ``Slurm 23.02, 23.11, and Beyond`` by Tim Wickberg, SchedMD.
Notice, however, that slurmd_ ignores any changes in slurm.conf_ or the DNS SRV_record_ (i.e., when running a :ref:`configless-slurm-setup`):

* When slurmd_ is started, it caches its configuration files as is discussed in bug_20462_.

* Therefore it is required to restart slurmd_ on all compute notes after modifying slurm.conf_ and the DNS SRV_record_ (if applicable).

The slurmctld_ migration process for Slurm_ 23.11 and later does **not** require to stop all running jobs,
and the details are discussed in bug_20070_ .

We have successfully performed a slurmctld_ migration following this procedure:

1. On the old ``SlurmctldHost`` server change the timeout values in slurm.conf_ to a high value::

     SlurmctldTimeout=3600
     SlurmdTimeout=3600 

   and make an ``scontrol reconfigure``.

2. Stop and disable the slurmctld_ service on the old ``SlurmctldHost`` server::

     <old-server>: systemctl stop slurmctld
     <old-server>: systemctl disable slurmctld

3. Copy all Slurm_ configuration files ``/etc/slurm/*.conf`` from the old server to the **new** ``SlurmctldHost`` server.
   Also make sure the Slurm_ logfile directory exists and has correct ownership (see :ref:`configure-slurm-logging`)::

     mkdir -pv /var/log/slurm
     touch /var/log/slurm/slurmctld.log
     chown -R slurm.slurm /var/log/slurm

4. In configless_ Slurm_ clusters update the DNS SRV_record_, see :ref:`configless-slurm-setup`

5. Migrate slurmctld_ to new machine:
   Make a tar-ball copy or rsync_ the ``StateSaveLocation`` directory (typically ``/var/spool/slurmctld``)
   to the new server, for example::

     <old-server>: $ rsync -aq /var/spool/slurmctld/ <new-server>:/var/spool/slurmctld/

   Make sure the permissions allow the *SlurmUser* to read and write the folder!

6. **Remember** to update slurm.conf_ with the new ``SlurmctldHost`` name,
   and remember to update the login nodes as well!

7. Start and enable the slurmctld_ service on the new server::

     systemctl start slurmctld
     systemctl enable slurmctld

8. As discussed in bug_20462_ it is necessary to restart slurmd_ on **all compute nodes**
   so they can pick up the new ``SlurmctldHost`` value in slurm.conf_.
   For example, use the clush_ command (see the :ref:`SLURM` page about ClusterShell_)::

     clush -ba systemctl restart slurmd

9. When everything is working correctly, restore the timeout values in slurm.conf_ to their defaults, for example::

     SlurmctldTimeout=600
     SlurmdTimeout=300 

   and make a ``scontrol reconfigure``.

If **not** using :ref:`configless-slurm-setup` you must distribute slurm.conf_ manually to all nodes in step 4.

.. _SRV_record: https://en.wikipedia.org/wiki/SRV_record
.. _slurmstepd: https://slurm.schedmd.com/slurmstepd.html
.. _bug_20070: https://support.schedmd.com/show_bug.cgi?id=20070
.. _bug_20462: https://support.schedmd.com/show_bug.cgi?id=20462
.. _rsync: https://en.wikipedia.org/wiki/Rsync

Migrate slurmctld version <= 23.02
------------------------------------

In Slurm_ 23.02 and older, changes to ``SlurmctldHost`` are not possible while jobs are running on the system.
Therefore you have to **stop all running jobs**, for example by making a :ref:`resource_reservation`.
Read the FAQ `How should I relocate the primary or backup controller? <https://slurm.schedmd.com/faq.html#controller>`_ with the procedure:

* Stop all Slurm_ daemons.
* Modify the ``SlurmctldHost`` values in the slurm.conf_ file.
* Distribute the updated slurm.conf_ file to all nodes.
  When using :ref:`configless-slurm-setup` see the section above.
* Copy the ``StateSaveLocation`` directory to the new host and make sure the permissions allow the SlurmUser to read and write it.
* Restart all Slurm_ daemons.

Log file rotation
=================

The Slurm_ log files may be stored in the ``/var/log/slurm`` directory, and they may grow rapidly on a busy system.
Especially the ``slurmctld.log`` file on the controller machine may grow very large.

Therefore you probably want to configure logrotate_ to administer your log files.
On RHEL Linux and clones the logrotate_ configuration files are in the ``/etc/logrotate.d/`` directory.

Manual configuration of logging is required because the SchedMD_ RPM files do not contain the logrotate setup, see bug_3904_, bug_2215_, and bug_4393_.
See also the section *LOGGING* at the end of the slurm.conf_ page with an example logrotate script.

First install the relevant RPM::

  dnf install logrotate

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
