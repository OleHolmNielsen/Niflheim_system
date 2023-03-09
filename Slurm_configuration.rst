.. _Slurm_configuration:

===================
Slurm configuration
===================

.. contents:: 
   :depth: 3

.. _Slurm_Quick_Start: https://slurm.schedmd.com/quickstart_admin.html
.. _Slurm: https://www.schedmd.com/
.. _Slurm_docs: https://slurm.schedmd.com/option
.. _Slurm_FAQ: https://slurm.schedmd.com/faq.html
.. _Slurm_download: https://slurm.schedmd.com/download.html
.. _Slurm_mailing_lists: https://lists.schedmd.com/cgi-bin/dada/mail.cgi/list
.. _slurm_devel_archive: https://groups.google.com/forum/#!forum/slurm-devel
.. _Slurm_publications: https://slurm.schedmd.com/publications.html
.. _Slurm_tutorials: https://slurm.schedmd.com/tutorials.html
.. _Slurm_bugs: https://bugs.schedmd.com
.. _Slurm_man_pages: https://slurm.schedmd.com/man_index.html

Jump to our top-level Slurm page: :ref:`SLURM` 

Slurm configuration and slurm.conf
==================================

Starting from Slurm_ 17.11 you probably want to look at the example configuration files found in this RPM::

  rpm -q slurm-example-configs

On the *Head/Master* node you should build a slurm.conf_ configuration file.
When it has been fully tested, then slurm.conf_ must be copied to all other nodes.

It is **mandatory** that the slurm.conf_ file is identical on all nodes in the system!

Consult the Slurm_Quick_Start_ Administrator Guide.
See also ``man slurm.conf`` or the on-line slurm.conf_ documentation.

.. _slurm.conf: https://slurm.schedmd.com/slurm.conf.html

Copy the HTML files to your $HOME directory, for example::

  mkdir $HOME/slurm/
  cp -rp /usr/share/doc/slurm-*/html $HOME/slurm/

.. _configless-slurm-setup:

Configless Slurm setup
----------------------

With Slurm_ 20.02 there is a new configless_ feature that allows the compute nodes — specifically the slurmd_ process — 
and user commands running on login nodes to pull configuration information directly from the slurmctld_ instead of from a pre-distributed local file. 

Notes: 

* Slurm versions 20.02.0 and 20.02.1 had a `slurm_pam_adopt issue <https://lists.schedmd.com/pipermail/slurm-users/2020-March/005044.html>`_ when using configless_ mode, see bug_8712_.

* Slurm versions up to an including 20.11.7 may start the slurmd_ service before the network is fully up, causing slurmd_ to fail.  Observed on some CentOS 8 systems, see bug_11878_.  
  The workaround is to restart the slurmd_ service manually.

.. _bug_8712: https://bugs.schedmd.com/show_bug.cgi?id=8712
.. _bug_11878: https://bugs.schedmd.com/show_bug.cgi?id=11878

The order of precedence for determining what configuration source to use is listed in the configless_ page.

On startup the compute node slurmd_ will query the slurmctld_ server that you specify, and the configuration files will be pulled to the node's local disk.
The pulled slurmd_ conguration files are stored in this folder::

  $ ls -ld /run/slurm/conf
  lrwxrwxrwx. 1 root root 28 Mar 18 08:24 /run/slurm/conf -> /var/spool/slurmd/conf-cache
  $ ls -la /var/spool/slurmd/conf-cache
  total 24
  drwxr-xr-x. 2 root  root     81 Mar 18 08:24 .
  drwxr-xr-x. 3 slurm slurm    92 Mar 18 08:24 ..
  -rw-r--r--. 1 root  root    506 Mar 18 08:24 cgroup.conf
  -rw-r--r--. 1 root  root    165 Mar 18 08:24 gres.conf
  -rw-r--r--. 1 root  root  11711 Mar 18 08:24 slurm.conf
  -rw-r--r--. 1 root  root   2538 Mar 18 08:24 topology.conf

Testing configless setup
........................

The slurmctld_ server information can preferably be provided in a DNS SRV_ record for your DNS zone, pointing to port 6817 on your slurmctld_ server(s)::

  _slurmctld._tcp 3600 IN SRV 10 0 6817 slurm-backup
  _slurmctld._tcp 3600 IN SRV 0 0 6817 slurm-master

To verify the DNS setup, install these packages with tools required below::

  yum install bind-utils hostname

Lookup the SRV_ record by either of::

  dig +short -t SRV -n _slurmctld._tcp.`dnsdomainname`
  host -t SRV _slurmctld._tcp.`dnsdomainname`

.. _configless: https://slurm.schedmd.com/configless_slurm.html
.. _SRV: https://en.wikipedia.org/wiki/SRV_record

Add login and submit nodes to slurm.conf
........................................

The SLUG 2020 talk (see Slurm_Publications_) *Field Notes 4: From The Frontlines of Slurm Support* by Jason Booth 
recommends on slide 31 to run slurmd on all login nodes in configless_ Slurm mode::

  We generally suggest that you run a slurmd to manage the configs on those nodes that run client commands, including submit or login nodes

The simplest way to achieve this is described in bug_9832_:

1. Add the login and submit nodes to slurm.conf_ as default-configured nodes, for example::

     NodeName=login1,login2

   and **do not** add these nodes to any partitions!

   Remember to add these nodes to the ``topology.conf`` file as well, for example::

     SwitchName=switch1 Nodes=login1,login2

   and open the firewall on these nodes (see the firewall section below).

2. Install the *slurm-slurmd* RPM on the login nodes and make sure to create the logging directory::

     mkdir /var/log/slurm
     chown slurm.slurm /var/log/slurm

   Then start the slurmd service::

     systemctl enable slurmd
     systemctl start slurmd

3. Verify that the Slurm config files have been downloaded::

     ls -l /run/slurm/conf

.. _bug_9832: https://bugs.schedmd.com/show_bug.cgi?id=9832

Configuring a custom slurmd service
-----------------------------------

The ``SLURMD_OPTIONS`` can be defined in the file ``/etc/sysconfig/slurmd``::

  SLURMD_OPTIONS=-M --conf-server <name of slurmctld server>

which is read by the Systemd_ service file ``/usr/lib/systemd/system/slurmd.service``.

Another way is to use ``systemctl edit slurmd`` to create an override file, see the systemctl manual page.
The override files will be placed in the ``/etc/systemd/system/slurmd.service.d/`` folder.

An example file ``/etc/systemd/system/slurmd.service.d/override.conf`` file could be::

  [Service]
  Environment="SLURMD_OPTIONS=-M --conf-server <name of slurmctld server>"

In this example the slurmd_ option ``-M`` locks ``slurmd`` in memory, and the slurmctld server name is given.
See configless_ and the slurmd_ manual page.

Configurator for slurm.conf
---------------------------

You can generate an initial slurm.conf_ file using several tools:

* The *Slurm Version 17.02 Configuration Tool* configurator_.
* The *Slurm Version 17.02 Configuration Tool - Easy Version* configurator.easy_.
* Build a configuration file using your favorite web browser and open ``file://$HOME/slurm/html/configurator.html`` or the simpler file ``configurator.easy.html``.
* Copy the more extensive sample configuration file ``.../etc/slurm.conf.example`` from the source tar-ball and use it as a starting point.

.. _configurator: https://slurm.schedmd.com/configurator.html
.. _configurator.easy: https://slurm.schedmd.com/configurator.easy.html

Save the resulting output to ``/etc/slurm/slurm.conf``.

The parameters are documented in ``man slurm.conf`` and slurm.conf_, and it's recommended to read through the long list of parameters.

In slurm.conf_ it's essential that the important spool directories and the slurm user are defined correctly::

  SlurmUser=slurm
  SlurmdSpoolDir=/var/spool/slurmd
  StateSaveLocation=/var/spool/slurmctld

**NOTE:** These spool directories must be created manually and owned by user *slurm* (see below), as they are **not** part of the RPM installation.

Starting slurm daemons at boot time
-----------------------------------

Enable startup of services as appropriate for the given node::

  systemctl enable slurmd      # Compute node
  systemctl enable slurmctld   # Master/head server
  systemctl enable slurmdbd    # Database server

The systemd_ service files are ``/usr/lib/systemd/system/slurm*.service``.

Slurm 16.05 init script bug
...........................

The Slurm_ **16.05** RPM packages install and configure (it's `bug 3371 <https://bugs.schedmd.com/show_bug.cgi?id=3371>`_) the init boot script ``/etc/init.d/slurm`` - even for systems like RHEL/CentOS 7 which use systemd_!
The bug has been fixed in Slurm_ 17.02.

If you have Slurm_ 16.05 (or older) on RHEL/CentOS 7, check if you have enabled the init script::

  chkconfig --list slurm

We should modify this setup to use systemd_ exclusively.
First disable the init script on all nodes, including login-nodes::

  chkconfig --del slurm

In order to avoid accidentally starting services with ``/etc/init.d/slurm``, it is best to also remove the offending script::

  rm -f /etc/init.d/slurm

Then enable the services properly as shown above.

Beware that any update of the Slurm_ 16.05 RPMs will recreate the missing ``/etc/init.d/slurm`` file, so you must remember to remove it after every update.

.. _systemd: https://en.wikipedia.org/wiki/Systemd

Manual startup of services
..........................

If there is any question about:

* The availability and sanity of the daemons' spool directories (perhaps on remote storage)
* The MySQL database
* If Slurm_ has been upgraded to a new version

it may be a good idea to start each service manually in stead of automatically as shown above.
For example::

  slurmctld -Dvvvv

Watch the the output for any signs of problems.
If the daemon looks sane, type Control-C and start the service in the normal way::

  systemctl start slurmctld

E-mail notification setup
-------------------------

The slurm.conf_ variables ``MailProg`` and ``MailDomain`` determine the delivery of E-mail messages from Slurm_.
You may want to use ``smail`` from the ``slurm-contribs`` RPM package by setting::

  MailProg=/usr/bin/smail

This will include some job statistics in the message.

Another possibility is Goslmailer_ (*GoSlurmMailer*).

.. _Goslmailer: https://github.com/CLIP-HPC/goslmailer

Reconfiguration of slurm.conf
-----------------------------

When changing the configuration files slurm.conf_ and cgroup.conf_, they must first be distributed to all compute and login nodes.
On the master node make the daemons reread the configuration files::

  scontrol reconfigure

From the scontrol_ man-page about the *reconfigure* option:

*  Instruct all Slurm daemons to re-read the configuration file.  This command does not restart the daemons.
   This mechanism would be used to modify configuration parameters (Epilog, Prolog, SlurmctldLogFile, SlurmdLogFile, etc.).   The
   Slurm  controller (slurmctld_)  forwards  the request all other daemons (slurmd_ daemon on each compute node). Running jobs continue execution.
*  Most configuration parameters can be changed by just running this command, however, Slurm
   daemons should be shutdown and **restarted** if any of these parameters are to be changed:

   - AuthType, BackupAddr, BackupController, ControlAddr, ControlMach, PluginDir, StateSaveLocation, SlurmctldPort or SlurmdPort.

*  The slurmctld_ daemon and all slurmd_ daemons must be **restarted** if **nodes are added to or removed from the cluster**.

Adding nodes
............

According to the scontrol_ man-page, when adding or removing nodes to slurm.conf_, it is necessary to **restart** slurmctld_.
However, it is also necessary to restart the slurmd_ daemon on all nodes, see bug_3973_:

1. Stop slurmctld_
2. Add/remove nodes in slurm.conf_
3. Restart slurmd_ on all nodes
4. Start slurmctld_

For a configless_ setup the slurmctld_ must be restarted first, in this case the order is:

1. Stop slurmctld_
2. Add/remove nodes in slurm.conf_
3. Start slurmctld_
4. Quickly restart slurmd_ on all nodes

It is also possible to add nodes to slurm.conf_ with a state of **future**::

  FUTURE
    Indicates the node is defined for future use and need not exist when the Slurm daemons are started.
    These nodes can be made available for use simply by updating the node state using the scontrol command rather than restarting the slurmctld daemon.
    After these nodes are made available, change their State in the slurm.conf file.
    Until these nodes are made available, they will not be seen using any Slurm commands or nor will any attempt be made to contact them. 

However, such *future* nodes must not be members of any partition.

.. _bug_3973: https://bugs.schedmd.com/show_bug.cgi?id=3973


.. _slurmd: https://slurm.schedmd.com/slurmd.html
.. _slurmctld: https://slurm.schedmd.com/slurmctld.html

Cgroup configuration
--------------------

*Control Groups* (Cgroups_ v1) provide a Linux kernel mechanism for aggregating/partitioning sets of tasks, and all their future children, into hierarchical groups with specialized behaviour.

Documentation about the usage of Cgroups_:

* `RHEL7 Resource Management Guide <https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7-Beta/html/Resource_Management_Guide/>`_.

* `RHEL8 Understanding control groups <https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/8/html/managing_monitoring_and_updating_the_kernel/setting-limits-for-applications_managing-monitoring-and-updating-the-kernel>`_.

To list current Cgroups_ use the command::

  lscgroup
  lscgroup -g cpu:/

To list processes that are not properly constrained by Slurm_ Cgroups_::

  ps --no-headers -eo pid,user,comm,cgroup | egrep -vw 'root|freezer:/slurm.*devices:/slurm.*cpuacct,cpu:/slurm.*memory:/slurm|cpuset:/slurm.*|dbus-daemon|munged|ntpd|gmond|polkitd|chrony|smmsp|rpcuser|rpc' 

Usage of Cgroups_ within Slurm_ is described in the Cgroups_Guide_.
Slurm_ provides Cgroups_ versions of a number of plugins:

* proctrack (process tracking)
* task (task management)
* jobacct_gather (job accounting statistics)

See also the cgroup.conf_ configuration file for the Cgroups_ support.

If you use *jobacct_gather*, change the default *ProctrackType* in slurm.conf_::

  ProctrackType=proctrack/linux

otherwise you'll get this warning in the slurmctld_ log::

  WARNING: We will use a much slower algorithm with proctrack/pgid, use Proctracktype=proctrack/linuxproc or some other proctrack when using jobacct_gather/linux

Notice: Linux kernel 2.6.38 or greater is strongly recommended, see the Cgroups_Guide_ *General Usage Notes*.

.. _Cgroups: https://www.kernel.org/doc/Documentation/cgroup-v1/cgroups.txt
.. _Cgroups_Guide: https://slurm.schedmd.com/cgroups.html
.. _cgroup.conf: https://slurm.schedmd.com/cgroup.conf.html

Getting started with Cgroups
............................

In this example we want to constrain jobs to the number of CPU cores as well as RAM memory requested by the job.

Configure slurm.conf_ to use Cgroups_ as well as the *affinity* plugin::

  TaskPlugin=affinity,cgroup

For a discussion see `bug 3853 <https://bugs.schedmd.com/show_bug.cgi?id=3853>`_.

You should probably also configure this (unless you have lots of short running jobs)::

  ProctrackType=proctrack/cgroup

see the section *ProctrackType* of slurm.conf_.

Create cgroup.conf_ file::

  cp /etc/slurm/cgroup.conf.example /etc/slurm/cgroup.conf

Edit the file to change these lines::

  ConstrainCores=yes
  ConstrainRAMSpace=yes
  ConstrainSwapSpace=yes

The cgroup.conf_ page defines:

* ConstrainCores=<yes|no>
    If configured to "yes" then constrain allowed cores to the subset of allocated resources. It uses the cpuset subsystem.
* ConstrainRAMSpace=<yes|no>
    If configured to "yes" then constrain the job's RAM usage.
    The default value is "no", in which case the job's RAM limit will be set to its  swap  space  limit.
    Also see AllowedSwapSpace, AllowedRAMSpace and ConstrainSwapSpace.
* ConstrainSwapSpace=<yes|no>
    If configured to "yes" then constrain the job's swap space usage.
    The default value is "no".
    Note that when set to "yes" and ConstrainRAMSpace is set to "no", AllowedRAMSpace is automatically set to 100% in order to limit the RAM+Swap amount to 100% of job's requirement plus the percent of allowed swap space.
    This amount is thus set to both RAM and RAM+Swap limits. This means that in that particular case, ConstrainRAMSpace is automatically enabled with the same limit than the one used to constrain swap space. Also see AllowedSwapSpace. 

You may also consider defining **MemSpecLimit** in slurm.conf_:

* **MemSpecLimit** Amount of memory, in megabytes, reserved for system use and not available for user allocations.
  If the task/cgroup plugin is configured and that plugin constrains memory allocations (i.e. TaskPlugin=task/cgroup in slurm.conf, plus ConstrainRAMSpace=yes in cgroup.conf), then Slurm compute node daemons (slurmd plus slurmstepd) will be allocated the specified memory limit.
  The daemons will not be killed if they exhaust the memory allocation (ie. the Out-Of-Memory Killer is disabled for the daemon's memory cgroup).
  If the task/cgroup plugin is not configured, the specified memory will only be unavailable for user allocations. 

See an interesting discussion in `bug 2713 <https://bugs.schedmd.com/show_bug.cgi?id=2713>`_.

If compute nodes mount Lustre or NFS file systems, it may be a good idea to configure cgroup.conf_ with::

  ConstrainKmemSpace=no

See the cgroup.conf_ man-page, bug_3874_ and 
`[slurm-dev] Interaction between cgroups and NFS <https://groups.google.com/forum/#!search/slurm-dev/slurm-devel/pOSFvh86_Vw/h2Nz6IjyAgAJ>`_.
This requires Slurm_ 17.02.5 or later, see NEWS_.
After distributing the cgroup.conf_ file to all nodes, make a ``scontrol reconfigure``.

.. _bug_3874: https://bugs.schedmd.com/show_bug.cgi?id=3874
.. _NEWS: https://github.com/SchedMD/slurm/blob/master/NEWS


Activating Cgroups
..................

Now propagate the updated files slurm.conf_ and cgroup.conf_ to all compute nodes and restart their slurmd_ service.

Cgroup bugs
...........

There may be some problems with Cgroups.

Jobs may crash with an error like::

  slurmstepd: error: xcgroup_instantiate: unable to create cgroup '/sys/fs/cgroup/memory/slurm/uid_207887' : No space left on device

The bug_3890_ explains this, it may be a kernel bug (CentOS 7 has kernel 3.10), see:

* https://github.com/torvalds/linux/commit/73f576c04b9410ed19660f74f97521bee6e1c546
* https://github.com/torvalds/linux/commit/24ee3cf89bef04e8bc23788aca4e029a3f0f06d9

**Workaround**: Reboot the node.

.. _bug_3890: https://bugs.schedmd.com/show_bug.cgi?id=3890

Node Health Check
-----------------

To insure the health status of Head/Master node and compute nodes, install the *LBNL Node Health Check* (NHC_) package from LBL_.
The NHC_ releases are in https://github.com/mej/nhc/releases/.

.. _NHC: https://github.com/mej/nhc
.. _LBL: https://www.lbl.gov/

It's simple to configure NHC_ Slurm integration, see the NHC_ page.
Add the following to slurm.conf_ on your *Head/Master* node **and** your compute nodes::

  HealthCheckProgram=/usr/sbin/nhc
  HealthCheckInterval=3600
  HealthCheckNodeState=ANY

This will execute NHC_ every 60 minutes on nodes in *ANY* states, see the slurm.conf_ documentation about ``Health*`` variables.
There are other criteria for when to execute NHC_ as defined by HealthCheckNodeState in slurm.conf_: ALLOC, ANY, CYCLE, IDLE, MIXED.

We add the following lines in the NHC_ configuration file ``/etc/nhc/nhc.conf`` for nodes in the domain *nifl.fysik.dtu.dk*::

  * || NHC_RM=slurm
  # Flag df to list only local filesystems (omit NFS mounts)
  * || DF_FLAGS="-Tkl"
  * || DFI_FLAGS="-Til"
  # Setting short hostname for compute nodes (default in our Slurm setup)
  *.nifl.fysik.dtu.dk || HOSTNAME=$HOSTNAME_S
  # Busy batch nodes may take a long time to run nhc
  *.nifl.fysik.dtu.dk  || TIMEOUT=120
  # Check OmniPath/Infiniband link
  x*.nifl.fysik.dtu.dk  || check_hw_ib 100

If you want an E-mail alert from NHC_ you must add a *crontab* entry to execute the ``nhc-wrapper`` script, see the NHC_ page section *Periodic Execution*.

For example, to execute the NHC_ check once per hour with a specified E-mail interval of 1 day, add this to the system's crontab::

  # Node Health Check
  3 * * * * /usr/sbin/nhc-wrapper -X 1d

NHC and GPU nodes
.................

The NHC_ has a check for Nvidia GPU health, namely ``check_nv_healthmon``.
Unfortunately, it seems that Nvidia no longer offers the tool nvidia-healthmon_ for this purpose.

Nvidia has a new *Data Center GPU Manager* (DCGM_) suite of tools which includes NVIDIA Validation Suite (NVVS_).
Download of DCGM_ requires membership of the Data Center GPU Manager (DCGM_) Program.
Install the RPM by::

  yum install datacenter-gpu-manager-1.7.1-1.x86_64.rpm

Run the NVVS_ tool::

  nvvs -g -l /tmp/nvvs.log

The (undocumented?) log file (-l) seems to be required.

See also https://docs.nvidia.com/datacenter/dcgm/latest/dcgm-user-guide/feature-overview.html#health-and-diagnostics

It does not seem obvious how to use NVVS_ as a fast running tool under NHC_.

Perhaps it may be useful in stead to check for the presence of the GPU devices with a check similar to this (for 4 GPU devices)::

  gpu* || check_file_test -c -r /dev/nvidia0 /dev/nvidia1 /dev/nvidia2 /dev/nvidia3

It seems that these device files do not get created automatically at reboot, but only if you run this (for example, in ``/etc/rc.local``)::

  /usr/bin/nvidia-smi
 
The physical presence of Nvidia devices can be tested by this command::

  # lspci | grep NVIDIA

.. _nvidia-healthmon: https://docs.nvidia.com/deploy/healthmon-user-guide/
.. _DCGM: https://developer.nvidia.com/dcgm
.. _NVVS: https://docs.nvidia.com/deploy/nvvs-user-guide/index.html

NHC bugs
........

It may be necessary to force the NHC_ configuration file ``/etc/nhc/nhc.conf`` to use the Slurm_ scheduler by adding this line near the top::

  * || NHC_RM=slurm

because NHC (version 1.4.2) may autodetect ``NHC_RM=pbs`` if the file ``/usr/bin/pbsnodes`` is present (see `issue 20 <https://github.com/mej/nhc/issues/20>`_).

Also, NHC 1.4.2 has a bug for Slurm_ multi-node jobs (see `issue 15 <https://github.com/mej/nhc/issues/15>`_), so you have to comment out any lines in ``nhc.conf`` calling::

  # check_ps_unauth_users

Both bugs should be fixed in NHC 1.4.3 (when it becomes available).

Reboot option
-------------

Nodes may occasionally have to be rebooted after firmware or kernel upgrades.

Reboot the nodes automatically as they become idle using the **RebootProgram** as configured in slurm.conf_, see the scontrol_ **reboot** option and explanation in the man-page::

  scontrol reboot [ASAP] [NodeList]

The ASAP flag is available from Slurm_ 17.02, see ``man scontrol`` for earlier versions.

Add this line to slurm.conf_::

  RebootProgram="/usr/sbin/reboot"

The path to ``reboot`` may be different on other OSes.

Notice: Command arguments to ``RebootProgram`` like::

  RebootProgram="/sbin/shutdown -r now"

seem to be ignored for Slurm_ 16.05 until 17.02.3, see bug_3612_.

.. _bug_3612: https://bugs.schedmd.com/show_bug.cgi?id=3612

Timeout options
---------------

A number of **Timeout** options may be configured in slurm.conf_.

In bug_3941_ is discussed the problem of nodes being drained due to the killing of jobs taking too long to complete.
To extend this timeout configure in slurm.conf_::

  UnkillableStepTimeout=120

Values above 127 should **not** be used, see bug_11103_.

This may also be accompanied by a custom command **UnkillableStepProgram**.
If this timeout is reached, the node will also be **drained** with reason *batch job complete failure*.

.. _bug_3941: https://bugs.schedmd.com/show_bug.cgi?id=3941
.. _bug_11103: https://bugs.schedmd.com/show_bug.cgi?id=11103

ReturnToService option
----------------------

The *ReturnToService* option in slurm.conf_ controls when a DOWN node will be returned to service, see slurm.conf_ and the FAQ 
`Why is a node shown in state DOWN when the node has registered for service? <https://slurm.schedmd.com/faq.html#return_to_service>`_.

MaxJobCount limit
-----------------

In slurm.conf_ is defined::

  MaxJobCount
    The maximum number of jobs Slurm can have in its active database at one time.
    Set the values of MaxJobCount and MinJobAge to insure the slurmctld daemon does not exhaust its memory or other resources.
    Once  this  limit  is  reached, requests to submit additional jobs will fail.
    The default value is 10000 jobs. 

If you exceed 10000 jobs in the queue users will get an error when submitting jobs::

  sbatch: error: Slurm temporarily unable to accept job, sleeping and retrying.
  sbatch: error: Batch job submission failed: Resource temporarily unavailable 

Add a higher value to slurm.conf_, for example::

  MaxJobCount=20000

Another parameter in slurm.conf_ may perhaps need modification with higher ``MaxJobCount``::

  MinJobAge
    The minimum age of a completed job before its record is purged from Slurm's active database.
    Set the values of MaxJobCount and to insure the slurmctld daemon does not exhaust its memory or other resources.
    The default value is 300 seconds. 

In addition, it may be a good idea to implement **MaxSubmitJobs** and **MaxJobs** resource_limits_ for user associations or QOSes, for example::

  sacctmgr modify user where name=<username> set MaxJobs=100 MaxSubmitJobs=500

.. _resource_limits: https://slurm.schedmd.com/resource_limits.html

Job arrays
----------

The job_arrays_ offer a mechanism for submitting and managing collections of similar jobs quickly and easily; job arrays with millions of tasks can be submitted in milliseconds (subject to configured size limits).

A slurm.conf_ configuration parameter controls the maximum job array size: 

* MaxArraySize. 

Be mindful about the value of MaxArraySize as job arrays offer an easy way for users to submit large numbers of jobs very quickly.

.. _job_arrays: https://slurm.schedmd.com/job_array.html

Requeueing of jobs
------------------

Jobs may be requeued explicitly by a system administrator, after node failure, or upon preemption by a higher priority job.
The following parameter in slurm.conf_ may be changed for the default ability for batch jobs to be requeued::

  JobRequeue=0

This function is:

* If JobRequeue is set to a value of 1, then batch job may be requeued unless explicitly disabled by the user.
* If JobRequeue is set to a value of 0, then batch job will not be requeued unless explicitly enabled by the user.
* The default value is 1. 

Use::

  sbatch --no-requeue or --requeue 

to change the default behavior for individual jobs.
 
Power monitoring and management
-------------------------------

Slurm can be configured to monitor the power and energy usage of compute nodes,
see the SLUG'18 presentation `Workload Scheduling and Power Management <https://slurm.schedmd.com/SLUG18/power_management.pdf>`_.
This paper also describes Slurm_ power management.
See also the `Slurm Power Management Guide <https://slurm.schedmd.com/power_mgmt.html>`_.

The Slurm configuration file for the **acct_gather plugins** such as *acct_gather_energy*, *acct_gather_profile* and *acct_gather_interconnect*
is described in acct_gather.conf_.

.. _acct_gather.conf: https://slurm.schedmd.com/acct_gather.conf.html

**Notice:** Please beware that the power monitoring may or may not cover entire compute node cabinets and other infrastructure!
For example, the RAPL_ method described below monitors CPUs and RAM only, 
and does not cover other power usage within the node such as GPUs, motherboard, fans, power supplies, PCIe network and storage adapters.

On most types of processors one may activate *Running Average Power Limit* (RAPL_) sensors for CPUs and RAM memory,
see these papers:

* The RAPL_ algorithm.
* `Correlating Hardware Performance Events to CPU and DRAM Power Consumption <https://ieeexplore.ieee.org/document/7549395>`_.
* `perf RAPL <https://en.wikipedia.org/wiki/Perf_(Linux)#RAPL>`_.
* `RAPL (Running Average Power Limit) driver <https://lwn.net/Articles/545745/>`_.
* `Running Average Power Limit – RAPL <https://01.org/blogs/2014/running-average-power-limit-%E2%80%93-rapl>`_.

With Slurm_ several *AcctGatherEnergyType* types are defined in the slurm.conf_ manual page.
RAPL_ data gathering can be enabled in Slurm_ by::

  # Power and energy monitoring
  AcctGatherEnergyType=acct_gather_energy/rapl
  AcctGatherNodeFreq=30

and do a ``scontrol reconfig``.
After a minute the power values become available::

  $ scontrol show node n123
  ...
    CurrentWatts=641 AveWatts=480

Please beware that the Slurm `bug 9956 <https://bugs.schedmd.com/show_bug.cgi?id=9956>`_ states:
*RAPL plugin: incorrect \*Watts and ConsumedEnergy values*.

A convenient script showpower_ is available for printing node power values as well as the total/average for sets of nodes with 1 line per node::

  Usage: showpower < -w node-list | -p partition(s) | -a | -h > [ -S sorting-variable ]
  where:
	-w node-list: Print this node-list
	-p partition(s): Print this partition
	-a: All nodes in the cluster
	-h: Print help information
	-S: Sort output by this column (e.g. CurrentWatts)

An example output is::

  $ showpower -w d[001-005]
  NodeName  #CPUs     CPU-  Current  Average       Cap ExtSensor ExtSensor
                      load    Watts    Watts     Watts     Watts    Joules
  d001         56     56.7      681      605      n/a        0      n/s
  d002         56     56.5      646      579      n/a        0      n/s
  d003         56     56.8      655      582      n/a        0      n/s
  d004         56     56.6      544      408      n/a        0      n/s
  d005         56     56.6      643      415      n/a        0      n/s
  
  NodeName  #CPUs     CPU-  Current  Average       Cap ExtSensor ExtSensor
                      load    Watts    Watts     Watts     Watts    Joules
  TOTAL       280    283.2     3169     2589        0        0        0
  Average      56     56.6      633      517        0        0        0


.. _showpower: https://github.com/OleHolmNielsen/Slurm_tools/tree/master/nodes
.. _RAPL: https://dl.acm.org/doi/10.1145/1840845.1840883
.. _turbostat: https://www.linux.org/docs/man8/turbostat.html

turbostat utility
.................

A CLI utility turbostat_ is provided by the *kernel-tools* package for reporting 
processor topology, frequency, idle power-state statistics, temperature, and power usage on Intel® 64 processors,
for example::

  $ turbostat --quiet --Summary

The turbostat_ reads the model-specific registers (MSRs) ``/dev/cpu/CPUNUM/msr``, see ``man 4 msr``.

Power saving configuration
---------------------------

Slurm_ provides an integrated power_save_ mechanism for powering down idle nodes.
Nodes that remain idle for a configurable period of time can be placed in a power saving mode, which can reduce power consumption or fully power down the node.
The nodes will be restored to normal operation once work is assigned to them. 

We describe the power_save_ configuration in the Slurm_cloud_bursting_ page section on :ref:`configuring-slurm-conf-for-power-saving`.

.. _power_save: https://slurm.schedmd.com/power_save.html
.. _Slurm_cloud_bursting: https://wiki.fysik.dtu.dk/Niflheim_system/Slurm_cloud_bursting

High throughput configuration or large clusters
-----------------------------------------------

The following document contains Slurm_ administrator information specifically for high throughput computing, namely the execution of many short jobs.
Getting optimal performance for high throughput computing does require some tuning and this document should help you off to a good start:

* https://slurm.schedmd.com/high_throughput.html

The following document contains Slurm administrator information specifically for clusters containing 1,024 nodes or more:

* https://slurm.schedmd.com/big_sys.html


Head/Master server configuration
================================

The following must be done on the Head/Master node.
Create the spool and log directories and make them owned by the slurm user::

  mkdir /var/spool/slurmctld /var/log/slurm
  chown slurm: /var/spool/slurmctld /var/log/slurm
  chmod 755 /var/spool/slurmctld /var/log/slurm

Create log files::

  touch /var/log/slurm/slurmctld.log 
  chown slurm: /var/log/slurm/slurmctld.log 

Create the (Linux default) accounting file::

  touch /var/log/slurm/slurm_jobacct.log /var/log/slurm/slurm_jobcomp.log
  chown slurm: /var/log/slurm/slurm_jobacct.log /var/log/slurm/slurm_jobcomp.log

**NOTICE:** If you plan to enable job accounting, it is mandatory to configure the database and accounting as explained in the :ref:`Slurm_accounting` page.

slurmctld daemon
----------------

Start and enable the slurmctld_ daemon::

  systemctl enable slurmctld.service
  systemctl start slurmctld.service
  systemctl status slurmctld.service

**Warning:** 
With Slurm 14.x and a compute node running RHEL 7 there is a bug `systemctl start/stop does not work on RHEL 7 <https://bugs.schedmd.com/show_bug.cgi?id=1182>`_.
This problem has apparently been resolved in Slurm 15.08.

Copy slurm.conf to all nodes
----------------------------

Finally copy ``/etc/slurm/slurm.conf`` to all compute nodes::

  scp -p /etc/slurm/slurm.conf nodeXXX:/etc/slurm/slurm.conf

It's convenient to use the ``pdsh`` command, see PDSH_.

.. _PDSH: https://wiki.fysik.dtu.dk/niflheim/SLURM#pdsh-parallel-distributed-shell

It is important to keep this file **identical** on both the *Head/Master* server and all Compute nodes.
Remember to include all of the *NodeName=* lines for all compute nodes.

Compute node configuration
==========================

The following must be done on each compute node.
Create the slurmd_ spool and log directories and make the correct ownership::

  mkdir /var/spool/slurmd /var/log/slurm
  chown slurm: /var/spool/slurmd  /var/log/slurm
  chmod 755 /var/spool/slurmd  /var/log/slurm

Create log files::

  touch /var/log/slurm/slurmd.log 
  chown slurm: /var/log/slurm/slurmd.log 

Executing the command::

  slurmd -C 

on each compute node will print its physical configuration (sockets, cores, real memory size, etc.), which must be added to the global slurm.conf_ file.
For example a node may be defined as::

  NodeName=test001 Boards=1 SocketsPerBoard=2 CoresPerSocket=2 ThreadsPerCore=1 RealMemory=8010 TmpDisk=32752 Feature=xeon

**Warning:** You should configure the *RealMemory* value slightly less than what is reported by ``slurmd -C``,
because kernel upgrades may give a slightly lower *RealMemory* value in the future and cause problems with the node's health status.

For recent Xeon and EPYC CPUs, the *Sub NUMA Cluster* (SNC_) BIOS setting has been shown to improve performance, see
`BIOS characterization for HPC with Intel Cascade Lake processors <https://www.dell.com/support/kbdoc/da-dk/000176921/bios-characterization-for-hpc-with-intel-cascade-lake-processors>`_.
This will cause each processor socket to have **two NUMA domains**, one for each of the memory controllers, so a dual-socket server will have 4 NUMA domains, for example::

  $ slurmd -C
  slurmd: Considering each NUMA node as a socket
  CPUs=40 Boards=1 SocketsPerBoard=4 CoresPerSocket=10 ThreadsPerCore=1 RealMemory=385380

**Note for Slurm 20.02**: The *Boards=1 SocketsPerBoard=2* configuration gives error messages, see bug_9241_ and bug_9233_.
Use *Sockets=* in stead::

  NodeName=test001 Sockets=2 CoresPerSocket=2 ThreadsPerCore=1 RealMemory=8010 TmpDisk=32752 Feature=xeon

This has been fixed in Slurm 20.02.4.

.. _bug_9241: https://bugs.schedmd.com/show_bug.cgi?id=9241
.. _bug_9233: https://bugs.schedmd.com/show_bug.cgi?id=9233

Here the ``TmpDisk`` is defined in slurm.conf_ as the size of the **TmpFS** file system (default: ``/tmp``).
It is possible to define another temporary file system in slurm.conf_, for example::

  TmpFS=/scratch

Start and enable the slurmd_ daemon::

  systemctl enable slurmd.service
  systemctl start slurmd.service
  systemctl status slurmd.service

.. _SNC: https://software.intel.com/content/www/us/en/develop/articles/intel-xeon-processor-scalable-family-technical-overview.html

Partition limits
----------------

If EnforcePartLimits is set to "ALL" then jobs which exceed a partition's size and/or limits will be rejected at submission time::

  EnforcePartLimits=ALL

NOTE: The partition limits being considered are its configured MaxMemPerCPU, MaxMemPerNode, MinNodes, MaxNodes,  MaxTime,  AllocNodes,  AllowAccounts,  AllowGroups, AllowQOS, and QOS usage threshold.


Job limits
----------

By default, Slurm_ will propagate all user limits from the submitting node (see ``ulimit -a``) to be effective also within batch jobs.

It is important to configure slurm.conf_ so that the *locked memory* limit isn't propagated to the batch jobs::

  PropagateResourceLimitsExcept=MEMLOCK

as explained in https://slurm.schedmd.com/faq.html#memlock.
A possible memory limit error with :ref:`OmniPath` was discussed in `Slurm bug 3363 <https://bugs.schedmd.com/show_bug.cgi?id=3363>`_.

In fact, if you have imposed any non-default limits in ``/etc/security/limits.conf`` or ``/etc/security/limits.d/\*.conf`` in the login nodes, you probably want to prohibit these from the batch jobs by configuring::

  PropagateResourceLimitsExcept=ALL

See the slurm.conf_ page for the list of all ``PropagateResourceLimitsExcept`` limits.

PAM module restrictions
-----------------------

On Compute nodes you may additionally install the ``slurm-pam_slurm`` RPM package to prevent rogue users from logging in.
A more important functions is the *containment* of SSH tasks, for example, by some MPI libraries **not** using Slurm_ for spawning tasks.
The pam_slurm_adopt_ module makes sure that child SSH tasks are controlled by Slurm on the job's master node.

SELinux may conflict with pam_slurm_adopt_, so it might need to be disabled by this command::

  setenforce 0

Disable SELinux permanently in ``/etc/selinux/config``::

  SELINUX=disabled

For further details, the pam_slurm_adopt_ module is described by its author in `Caller ID: Handling ssh-launched processes in Slurm  <https://tech.ryancox.net/2015/04/caller-id-handling-ssh-launched-processes-in-slurm.html>`_.
Features include:

* This module restricts access to compute nodes in a cluster where Slurm is in use.
  Access is granted to root, any user with an Slurm-launched job currently running on the node, or any user who has allocated resources on the node according to the Slurm.

Usage of pam_slurm_adopt_ is described in the source files pam_slurm_adopt_.
There is also a nice description in bug_4098_.
Documentation of pam_slurm_adopt_ is discussed in bug_3567_.

.. _bug_4098: https://bugs.schedmd.com/show_bug.cgi?id=4098
.. _bug_3567: https://bugs.schedmd.com/show_bug.cgi?id=3567

.. _pam_slurm_adopt: https://slurm.schedmd.com/pam_slurm_adopt.html
.. _pam: https://github.com/SchedMD/slurm/tree/master/contribs/pam

The PAM usage of, for example, ``/etc/pam.d/system-auth`` on CentOS/RHEL is configured through the authconfig_ command.

.. _pam_slurm: https://slurm.schedmd.com/faq.html#pam
.. _authconfig: https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/System-Level_Authentication_Guide/authconfig-addl-auth.html

Configure PrologFlags
.....................

**Warning: Do NOT** configure ``UsePAM=1`` in slurm.conf_ (this advice can be found on the net).
Please see bug_4098_ (comment 3).

You need to configure slurm.conf_ with::

  PrologFlags=contain

Then distribute the slurm.conf_ file to all nodes.
Reconfigure the slurmctld_ service::

  scontrol reconfigure

This can be done while the cluster is in production, see bug_4098_ (comment 3).

PAM configuration
.................

**Warnings:** 

* First make the ``PrologFlags=contain`` configuration described above.
* Do **NOT** configure ``UsePAM=1`` in slurm.conf_.
* Reconfiguration of the PAM setup should only be done on compute nodes that can't run jobs (for example, drained nodes).
* You should only configure this on Slurm_ 17.02.2 or later.

First make sure that you have installed this Slurm_ package::

  rpm -q slurm-pam_slurm

Create a new file in ``/etc/pam.d/`` where the line with ``pam_systemd.so`` has been removed::

  cd /etc/pam.d/
  grep -v pam_systemd.so < password-auth > password-auth-no-systemd

The reason is (quoting pam_slurm_adopt_) that:

* ``pam_systemd.so`` is known to not play nice with Slurm's usage of cgroups.
  It is recommended that you disable it or possibly add ``pam_slurm_adopt.so`` after ``pam_systemd.so``.

Insert some new lines in the file ``/etc/pam.d/sshd`` at this place::

  ...
  account    required     pam_nologin.so
  # - PAM config for Slurm - BEGIN
  account    sufficient   pam_slurm_adopt.so
  account    required     pam_access.so
  # - PAM config for Slurm - END
  account    include      password-auth
  ...

and also replace the line::

  session    include      password-auth

by::

  # - PAM config for Slurm - BEGIN
  session    include      password-auth-no-systemd
  # - PAM config for Slurm - END

Options to the ``pam_slurm_adopt.so`` module are documented in the pam_slurm_adopt_ page.

Now append these lines to ``/etc/security/access.conf`` (see ``man access.conf`` or access.conf_ for further possibilities)::

  + : root   : ALL
  - : ALL    : ALL

so that ``pam_access.so`` will:

* Allow access to the root user.
* Deny access to ALL other users.

.. _access.conf: https://linux.die.net/man/5/access.conf

This can be tested immediately by trying to make SSH logins to the node.
Normal user logins should be rejected with the message::

  Access denied by pam_slurm_adopt: you have no active jobs on this node
  Connection closed by <IP address>

slurmd systemd limits
---------------------

MPI jobs and other tasks using the Infiniband_ or :ref:`OmniPath` fabrics must have **unlimited locked memory**, see above.
Limits defined in ``/etc/security/limits.conf`` or ``/etc/security/limits.d/\*.conf`` are **not** effective for systemd_ services, see https://access.redhat.com/solutions/1257953,
so any limits must be defined in the service file, see ``man systemd.exec``.

.. _Infiniband: https://en.wikipedia.org/wiki/InfiniBand

For slurmd_ running under systemd_ the default limits are configured in ``/usr/lib/systemd/system/slurmd.service`` as::

  LimitNOFILE=51200
  LimitMEMLOCK=infinity
  LimitSTACK=infinity

If you want to modify/override these limits, create a new service file rather than editing the ``slurmd.service`` file.
For example, create a file ``/etc/systemd/system/slurmd.service.d/core_limit.conf`` with the contents::

  [Service]
  LimitCORE=0

and do::

  systemctl daemon-reload
  systemctl restart slurmd

This file could be distributed to all compute nodes from a central location.

The possible process limit parameters are documented in the systemd.exec_ page section on *Process Properties*.
The list is::

  LimitCPU=, LimitFSIZE=, LimitDATA=, LimitSTACK=, LimitCORE=, LimitRSS=, LimitNOFILE=, LimitAS=, LimitNPROC=, LimitMEMLOCK=, LimitLOCKS=, LimitSIGPENDING=, LimitMSGQUEUE=, LimitNICE=, LimitRTPRIO=, LimitRTTIME=

.. _systemd.exec: https://www.freedesktop.org/software/systemd/man/systemd.exec.html

To ensure that job tasks running under Slurm_ have the desired configuration, verify the ``slurmd`` daemon's limits by::

  cat /proc/$(pgrep -u 0 slurmd)/limits

If slurmd_ has a memory lock limited less than expected, it may be due to slurmd_ having been started at boot time by the old init-script ``/etc/init.d/slurm``
rather than by systemctl.
To remedy this problem see the section *Starting slurm daemons at boot time* above.

Setting job limits with PAM
---------------------------

By default jobs started by *slurmd* do not use PAM and therefore do not honor the ``/etc/security/limits.conf`` file.
This behavior may be changed by adding to ``slurm.conf`` (see the man-page)::

  UsePAM=1

Then you can create a file ``/etc/pam.d/slurm`` containing::

  auth            required        pam_localuser.so
  account         required        pam_unix.so
  session         required        pam_limits.so

Temporary job directories
-------------------------

Jobs may be storing temporary files in ``/tmp``, ``/scratch``, and ``/dev/shm/``.
These directories may be filled up, and no clean-up is done after the job exits.
There are several possible solutions:

* The auto_tmpdir_ SPANK_ plugin provides automated handling of temporary directories for jobs (see also `this page <https://docs.hpc.udel.edu/technical/slurm/caviness/auto_tmpdir>`_).

  A great advantage of this plugin that it actually works correctly with NFS home directories automounted by autofs_,
  in contrast to Slurm's job_container.conf_ plugin priot to 23.02 (see more below).

  You can build a customized RPM package for this plugin:

  * CMake_ version 3.6 (or greater) is required.
    Make sure the EPEL repo is enabled, then install this package::

      yum install epel-release
      yum install cmake3    # CentOS 7 and other EL7 systems
      dnf install cmake     # EL8 systems and newer

  * Download the source::

      git checkout git@github.com:University-of-Delaware-IT-RCI/auto_tmpdir.git 
      or:
      git clone https://github.com/University-of-Delaware-IT-RCI/auto_tmpdir.git

      cd auto_tmpdir
      mkdir builddir
      cd builddir

  * Configure the node local temporary directory as ``/scratch/slurm-<slurm_jobid>`` (choose whatever scratch disk is appropriate for your cluster installation)::

      cmake3 -DSLURM_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DAUTO_TMPDIR_DEFAULT_LOCAL_PREFIX=/scratch/slurm- ..
      make package

    Here the ``..`` just refers to the parent directory.
    The generated RPM package may be named similar to ``auto_tmpdir-1.0.1-21.08.8.el8.x86_64.rpm``.

  * **Note:** If you are **upgrading Slurm** to a new major version (like 21.08 to 22.05), you **must use a test node** to build the new auto_tmpdir_ RPM:

    1. Uninstall any preexisting RPM::

         yum remove auto_tmpdir

    2. Upgrade Slurm_ to the new version.

    3. Rebuild the auto_tmpdir_ RPM as shown above.

    4. Copy the auto_tmpdir_ RPM to where you keep the Slurm_ RPMs so that you can upgrade compute nodes with the ``slurm-*`` as well as ``auto_tmpdir`` simultaneously.

  * Install the ``auto_tmpdir`` RPM package on all slurmd_ compute nodes, as well as all submit/login nodes (see notes below).

  * Now you can create the file ``/etc/slurm/plugstack.conf`` (see the SPANK_ page) with contents::

      required    auto_tmpdir.so          mount=/tmp mount=/var/tmp

    Notes:

    * The ``/etc/slurm/plugstack.conf`` file name can be changed by the *PlugStackConfig* parameter in slurm.conf_.

    * If you use configless_ Slurm_ the ``/etc/slurm/plugstack.conf`` file is automatically distributed from the slurmctld_ host.

    * It is not required that ``plugstack.conf`` is identical or even installed on every node in the cluster, since Slurm_ does not check for that.
      Therefore you can have different configurations on different nodes (except when you use configless_ Slurm_).

    * If the ``plugstack.conf`` file is installed on a submit/login or compute node, it is **mandatory** that all plugins listed in the file are actually installed as well,
      otherwise user commands or slurmd_ will fail with errors.
      See a discussion in bug_14483_.

  * **Quickly restart** the slurmd_ service on **all compute nodes** to actually activate the ``/etc/slurm/plugstack.conf`` feature::

      systemctl restart slurmd

    This is required in order for new srun_ commands etc. to run correctly with the SPANK_ plugin.
    See the SPANK_ manual page::

      Note: Plugins loaded in slurmd context persist for the entire time slurmd is running, so if configuration is changed or plugins are updated, slurmd must be restarted for the changes to take effect. 

  * For information about Linux_namespaces_ currently mounted on the compute nodes use::

      lsns -t mnt

* The job_container.conf_ plugin which was introduced in Slurm_ 20.11.5.
  You should read `the tmpfs_jobcontainer FAQ <https://slurm.schedmd.com/faq.html#tmpfs_jobcontainer>`_ as well as bug_11183_ and bug_11135_ for further details.
  The job_container.conf_ plugin uses Linux_namespaces_.

  **WARNING:** 
  NFS automount and ``job_container/tmpfs`` do not play well together priot to 23.02:
  If a directory does not exist *when the tmpfs is created*, then that directory cannot be accessed by the job, see bug_14344_ and bug_12567_.
  The issue has been resolved in Slurm_ 23.02 according to bug_12567_.

  The configuration file ``/etc/slurm/job_container.conf`` must be created, and it is **important** to configure the new 23.02 option::

    Shared=true

  See the job_container.conf manual page.


.. _auto_tmpdir: https://github.com/University-of-Delaware-IT-RCI/auto_tmpdir 
.. _autofs: https://wiki.archlinux.org/title/autofs
.. _job_container.conf: https://slurm.schedmd.com/faq.html#tmpfs_jobcontainer
.. _bug_11183: https://bugs.schedmd.com/show_bug.cgi?id=11183
.. _bug_11135: https://bugs.schedmd.com/show_bug.cgi?id=11135
.. _bug_14344: https://bugs.schedmd.com/show_bug.cgi?id=14344
.. _bug_12567: https://bugs.schedmd.com/show_bug.cgi?id=12567
.. _bug_14483: https://bugs.schedmd.com/show_bug.cgi?id=14483
.. _Linux_namespaces: https://en.wikipedia.org/wiki/Linux_namespaces

* Another SPANK_ plugin is at https://github.com/hpc2n/spank-private-tmp.
  This plugin does not do any cleanup, so cleanup will have to be handled separately.

* A manual cleanup could be made (if needed) by a crontab job on the compute node, for example for the ``/scratch`` directory::

    # Remove files > 7 days old under /scratch/XXX (mindepth=2)
    find /scratch -depth -mindepth 2 -mtime +7 -exec rm -rf {} \;

.. _SPANK: https://slurm.schedmd.com/spank.html
.. _CMake: https://cmake.org/

Configure Prolog and Epilog scripts
===================================

It may be necessary to execute Prolog and/or Epilog scripts on the compute nodes when slurmd_ executes a task step (by default none are executed).
See also the `Prolog and Epilog Guide <https://slurm.schedmd.com/prolog_epilog.html>`_.

In the slurm.conf_ page this is described:

* **Prolog** 

  Fully qualified pathname of a program for the slurmd_ to execute whenever it is asked to run a job step from a new job allocation (e.g. ``/usr/local/slurm/prolog``).
  A glob pattern (See glob(7)) may also be used to specify more than one program to run (e.g. ``/etc/slurm/prolog.d/*``).
  The slurmd_ executes the prolog before starting the first job step. The prolog script or scripts may be used to purge files, enable user login, etc.

  By default there is no prolog.
  Any configured script is expected to complete execution quickly (in less time than **MessageTimeout**).

  If the prolog fails (returns a non-zero exit code), this will result in the node being set to a DRAIN state and the job being requeued in a held state, unless nohold_on_prolog_fail is configured in SchedulerParameters.
  See *Prolog and Epilog Scripts* for more information. 

* **TaskProlog**

  Fully qualified pathname of a program to be execute as the slurm job's owner prior to initiation of each task.
  Besides the normal environment variables, this has SLURM_TASK_PID available to identify the process ID of the task being started.
  Standard output from this program can be used to control the environment variables and output for the user program. 
  (further details in the slurm.conf_ page).

* **TaskEpilog** 

  Fully qualified pathname of a program to be execute as the slurm job's owner after termination of each task. See *TaskProlog* for execution order details. 

See also the items:

* PrologEpilogTimeout
* PrologFlags
* SrunEpilog

Prolog and epilog examples
--------------------------

An example script is shown in the FAQ https://slurm.schedmd.com/faq.html#task_prolog::

  #!/bin/sh
  #
  # Sample TaskProlog script that will print a batch job's
  # job ID and node list to the job's stdout  
  #

  if [ X"$SLURM_STEP_ID" = "X" -a X"$SLURM_PROCID" = "X"0 ]
  then
    echo "print =========================================="
    echo "print SLURM_JOB_ID = $SLURM_JOB_ID"
    echo "print SLURM_NODELIST = $SLURM_NODELIST"
    echo "print =========================================="
  fi

The script is supposed to output commands to be read by slurmd_:

* The task prolog is executed with the same environment as the user tasks to be initiated.
  The standard output of that program is read and processed as follows:

  - **export name=value** - sets an environment variable for the user task
  - **unset name** - clears an environment variable from the user task
  - **print ...** - writes to the task's standard output.


Configure partitions
====================

System partitions are configured in slurm.conf_, for example::

  PartitionName=xeon8 Nodes=a[070-080] Default=YES DefaultTime=50:00:00 MaxTime=168:00:00 State=UP

Partitions may overlap so that some nodes belong to several partitions.

Access to partitions is configured in slurm.conf_ using *AllowAccounts*, *AllowGroups*, or *AllowQos*.

If some partition (like big memory nodes) should have a higher priority, this is controlled in slurm.conf_ using the multifactor plugin, for example::

  PartitionName ... PriorityJobFactor=10
  PriorityWeightPartition=1000

Sharing nodes and cons_tres
=================================

By default nodes are allocated exclusively to jobs, but it is possible to permit multiple jobs and/or multiple users per node.
This is configured using **Consumable Resource Allocation Plugin** or cons_tres_ in slurm.conf_.
The cons_tres_ plugin has improved support for GPU nodes as compared to the older cons_res_,
and is described in the *Presentations from Slurm User Group Meeting, September 2019*,
see Slurm_publications_.

The required slurm.conf_ configuration is::

  SelectType=select/cons_tres 
  SelectTypeParameters=CR_CPU_MEMORY

In this configuration CPU and Memory are consumable resources.
It is **mandatory** to use ``OverSubscribe=NO`` for the partitions as stated in the cons_res_ page:

* All CR_s assume OverSubscribe=No or OverSubscribe=Force EXCEPT for CR_MEMORY which assumes OverSubscribe=Yes

Strange behaviour will result if you use the wrong *OverSubscribe* parameter.
The *OverSubscribe* parameter (default= **NO**) is defined in the section *OverSubscribe* in slurm.conf_.
See also the cons_res_share_ page.

.. _cons_res: https://slurm.schedmd.com/cons_res.html
.. _cons_res_share: https://slurm.schedmd.com/cons_res_share.html
.. _cons_tres: https://slurm.schedmd.com/slurm.conf.html

Upgrade cons_res to cons_tres
---------------------------------

The newer cons_tres_ plugin should be used in stead of cons_res_.
Upgrading from cons_res_ to cons_tres_ on a running system must be done very carefully, however, as discussed in bug_15470_.
The procedure is:

1. In slurm.conf_ change into ``SelectType=select/cons_tres``.
   The slurm.conf_ file must be distributed to all nodes (not needed with Configless_).

2. Then restart the slurmctld_ **as well as** all slurmd_ immediately::

     systemctl restart slurmctld
     clush -ba systemctl restart slurmd

Here we have used :ref:`ClusterShell` to run the command on all nodes.
One **must not** make a ``scontrol reconfig`` during this process!

.. _bug_15470: https://bugs.schedmd.com/show_bug.cgi?id=15470

Configure multiple nodes and their features
===========================================

Some *defaults* may be configured in slurm.conf_ for similar compute nodes, for example::

  NodeName=DEFAULT Boards=1 SocketsPerBoard=2 CoresPerSocket=2 ThreadsPerCore=1 RealMemory=8000 TmpDisk=32752 Weight=1
  NodeName=q001
  NodeName=q002
  ...

**Note for Slurm 20.02**: The *Boards=1 SocketsPerBoard=2* configuration gives error messages, see bug_9241_.
Use this in stead::

  NodeName=DEFAULT Sockets=2 CoresPerSocket=2 ThreadsPerCore=1 RealMemory=8000 TmpDisk=32752 Weight=1

.. _Torque: https://adaptivecomputing.com/cherry-services/torque-resource-manager/

Node **features**, similar to node **properties** used in the Torque_ resource manager are defined for each *NodeName* in slurm.conf_ by:

* Feature:

    A comma delimited list of arbitrary strings indicative of some characteristic associated with the node.
    There is no value associated with a feature at this time, a node either has a feature or it does not.
    If desired a feature may contain a numeric component indicating, for example, processor speed.
    By default a node has no features. 

Some examples are::

  NodeName=DEFAULT Sockets=2 CoresPerSocket=2 ThreadsPerCore=1 RealMemory=8000 TmpDisk=32752 Feature=xeon8,ethernet Weight=1
  NodeName=q001
  NodeName=q002

NodeSet configuration
---------------------

From Slurm 20.02 a new *NodeSet* configuration is available in slurm.conf_.

The nodeset configuration allows you to define a name for a specific set of nodes which can be used to simplify the partition configuration section, 
especially for heterogenous or condo-style systems. 
Each nodeset may be defined by an explicit list of nodes, and/or by filtering the nodes by a particular configured feature.

This can be used to simplify partitions in slurm.conf_, and some examples are::

  NodeSet=a_nodes Nodes=a[001-100]
  NodeSet=gpu_nodes Feature=GPU

Node weight
-----------

For clusters with heterogeneous node hardware it is useful to assign different **Weight** values to each type of node, see this slurm.conf_ parameter::

  Weight
    The priority of the node for scheduling purposes. All things being equal, jobs will be allocated the nodes with the lowest weight which satisfies their requirements. 

This enables prioritization based upon a number of hardware parameters such as GPUs, RAM memory size, CPU clock speed, CPU core number, CPU generation.
For example, GPU nodes should be avoided for non-GPU jobs.

A nice method was provided by Kilian Cavalotti of SRCC_ where a **weight mask** is used in slurm.conf_.
Each digit in the weight mask represents a hardware parameter of the node (a weight prefix of *1* is prepended in order to avoid octal conversion).
For example, the following weight mask example puts a higher weight on GPUs, then RAM memory, then number of cores, and finally the CPU generation::

  # (A weight prefix of "1" is prepended)
  #       #GRES           Memory          #Cores          CPU_generation
  #        none: 0         24 GB: 0        8: 0           Nehalem:      1
  #       1 GPU: 1         48 GB: 1        16: 1          Sandy Bridge: 2
  #       2 GPU: 2         64 GB: 2        24: 2          Ivy Bridge:   3
  #       3 GPU: 3        128 GB: 3        32: 3          Broadwell:    4
  #       4 GPU: 4        256 GB: 4        36: 4          Skylake:      5      
  # Example: Broadwell (=4) with 24 cores (=2), 128 GB memory (=3), and 0 GPUs (=0): Weight=10324

This example would be used to assign a **Weight** value in slurm.conf_ for the relevant nodes::

  NodeName=xxx Sockets=2 CoresPerSocket=12 ThreadsPerCore=1 RealMemory=128000 Weight=10324

A different prioritization of hardware can be selected with different columns and numbers in the mask,
but a fixed number is the result of the mask calculation for each type of node.

.. _SRCC: https://srcc.stanford.edu/

Generic resources (GRES) and GPUs
---------------------------------

The Generic resources (GRES_) are a comma delimited list of *generic resources* (GRES_) specifications for a node.
Such resources may be occupied by jobs, for example, GPU accelerators.
In this case you must also configure the gres.conf_ file.

An example with a **gpu** GRES_ may be a gres.conf_ file::

  Nodename=h[001-002] Name=gpu Type=K20Xm File=/dev/nvidia[0-3]

If GRES_ is used, you **must** also configure slurm.conf_, so define the named GRES_ in slurm.conf_::

  GresTypes=gpu

and append a list of GRES_ resources in the slurm.conf_ *NodeName* specifications::

  NodeName=h[001-002] Gres=gpu:K20Xm:4


.. _GRES: https://slurm.schedmd.com/gres.html
.. _gres.conf: https://slurm.schedmd.com/gres.conf.html

See also the examples in the gres.conf_ page.

Configure network topology
==========================

Slurm can be configured to support topology-aware resource allocation to optimize job performance, see the Topology_Guide_ and the topology.conf_ manual page.

Check consistency of ``/etc/slurm/topology.conf`` with nodelist in ``/etc/slurm/slurm.conf`` using the checktopology_ tool.

.. _Topology_Guide: https://slurm.schedmd.com/topology.html
.. _topology.conf: https://slurm.schedmd.com/topology.conf.html
.. _checktopology: https://github.com/OleHolmNielsen/Slurm_tools/tree/master/nodes

Configure firewall for Slurm daemons
====================================

The Slurm_ compute nodes must be allowed to connect to the Head/Master node's slurmctld_ daemon.
In the configuration file these ports are by default (see slurm.conf_)::

  SlurmctldPort=6817
  SlurmdPort=6818
  SchedulerPort=7321

CentOS7/RHEL7 firewall
----------------------

The CentOS7/RHEL7 default firewall service is firewalld_ and **not** the well-known *iptables* service.
The dynamic firewall daemon firewalld_ provides a dynamically managed firewall with support for network “zones” to assign a level of trust to a network and its associated connections and interfaces. 
See `Introduction to firewalld <https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Security_Guide/sec-Using_Firewalls.html>`_.

A nice introduction is `RHEL7: How to get started with Firewalld <https://www.certdepot.net/rhel7-get-started-firewalld/>`_.

.. _firewalld: https://fedoraproject.org/wiki/FirewallD

Install firewalld_ by::

  yum install firewalld firewall-config

Head/Master node
................

Open port 6817 (slurmctld_)::

  firewall-cmd --permanent --zone=public --add-port=6817/tcp 
  firewall-cmd --reload

Alternatively, completely whitelist the compute nodes' private subnet (here: 10.2.x.x)::

  firewall-cmd --permanent --direct --add-rule ipv4 filter INPUT_direct 0 -s 10.2.0.0/16 -j ACCEPT
  firewall-cmd --reload

The configuration is stored in the file ``/etc/firewalld/direct.xml``.

Database (slurmdbd) node
........................

The slurmdbd_ service by default listens to port 6819, see slurmdbd.conf_.

Open port 6819 (slurmdbd_)::

  firewall-cmd --permanent --zone=public --add-port=6819/tcp 
  firewall-cmd --reload

.. _slurmdbd: https://slurm.schedmd.com/slurmdbd.html
.. _slurmdbd.conf: https://slurm.schedmd.com/slurmdbd.conf.html

Compute node firewall must be off
.................................

Quoting Moe Jette from `[slurm-dev] No route to host: Which ports are used? <https://groups.google.com/forum/#!topic/slurm-devel/wOHcXopbaXw>`_::

  Other communications (say between srun and the spawned tasks) are intended to operate within a cluster and have no port restrictions.

The simplest solution is to ensure that the compute nodes must have **no firewall enabled**::

  systemctl stop firewalld
  systemctl disable firewalld

However, you may run a firewall service, as long as you ensure that **all ports** are open between the compute nodes.

Login node firewall
-------------------

A login node doesn't need any special firewall rules for Slurm_ because no such daemons should be running on login nodes.

**Warning:** The srun_ command only works if the login node can:

* Connect to the Head node port 6817.
* Resolve the DNS name of the compute nodes.
* Connect to the Compute nodes port 6818.

Therefore interactive batch jobs with srun_ seem to be impossible if your compute nodes are on an isolated private network relative to the Login node.

.. _srun: https://slurm.schedmd.com/srun.html

.. _firewall-between-slurmctld-and-slurmdbd:

Firewall between slurmctld and slurmdbd
=======================================

See advice from the Slurm_publications_ presentation *Technical: Field Notes Mark 2: Random Musings From Under A New Hat, Tim Wickberg, SchedMD* (2018).

SchedMD recommends to run slurmctld_ and slurmdbd_ daemons on **separate servers**, see the *My Preferred Deployment Pattern* slides in the presentation.

If you use this configuration, the firewall is an important issue.

See the *Related Networking Notes* slides in the presentation:

* This is almost always an issue with a firewall in between slurmctld and slurmdbd.
* slurmdbd opens a new connection to slurmctld to push changes.
* If you’ve firewalled that off, the update will not be propogated.

**Conclusion:** Open the firewall for **all ports** between slurmctld_ and slurmdbd_ servers.

Open firewall between servers
-----------------------------

On these servers, insert a firewalld_ direct_rule_ so that any incoming source IP packet (src) from a specific IP address (A.B.C.D) gets accepted, for example::

  firewall-cmd --permanent --direct --add-rule ipv4 filter INPUT_direct 0 -s A.B.C.D/32 -j ACCEPT

Then reload the firewall for any changes to take effect::

  firewall-cmd --reload

List the rules by::

  firewall-cmd  --permanent --direct --get-all-rules

.. _Slurm_publications: https://slurm.schedmd.com/publications.html
.. _firewalld: https://fedoraproject.org/wiki/FirewallD
.. _direct_rule: https://access.redhat.com/documentation/en-US/Red_Hat_Enterprise_Linux/7/html/Security_Guide/sec-Using_Firewalls.html#sec-Understanding_the_Direct_Interface


Checking the Slurm daemons
==========================

Check the configured daemons using the scontrol_ command::

  scontrol show daemons

To verify the basic cluster partition setup::

  scontrol show partition

To display the Slurm_ configuration::

  scontrol show config

To display the compute nodes::

  scontrol show nodes

One may also run the daemons interactively as described in Slurm_Quick_Start_ (*Starting the Daemons*).
You can use one window to execute *slurmctld -D -vvvvvv*, a second window to execute *slurmd -D -vvvvv*.

.. _scontrol: https://slurm.schedmd.com/scontrol.html

Configure ARP cache for large networks
======================================

If the number of network devices (cluster nodes plus switches etc.) approaches or exceeds 512, you must consider the Linux kernel's limited dynamic ARP-cache size. 
Please read the man-page *man 7 arp* about the kernel's ARP-cache.

The best solution to this ARP-cache trashing problem is to increase the kernel's ARP-cache garbage collection (gc) parameters by adding these lines to ``/etc/sysctl.conf``::

  # Don't allow the arp table to become bigger than(clusters containing 1024 nodes or more). this
  net.ipv4.neigh.default.gc_thresh3 = 4096
  # Tell the gc when to become aggressive with arp table cleaning.
  # Adjust this based on size of the LAN.
  net.ipv4.neigh.default.gc_thresh2 = 2048
  # Adjust where the gc will leave arp table alone
  net.ipv4.neigh.default.gc_thresh1 = 1024
  # Adjust to arp table gc to clean-up more often
  net.ipv4.neigh.default.gc_interval = 3600
  # ARP cache entry timeout
  net.ipv4.neigh.default.gc_stale_time = 3600

You may also consider increasing the SOMAXCONN limit::

  # Limit of socket listen() backlog, known in userspace as SOMAXCONN
  net.core.somaxconn = 1024

see `Large Cluster Administration Guide <https://slurm.schedmd.com/big_sys.html>`_.

Then reread this configuration file::

  /sbin/sysctl -p

Slurm plugins
=============

A Slurm_ plugin_ is a dynamically linked code object which is loaded explicitly at run time by the Slurm libraries. 
A plugin_ provides a customized implementation of a well-defined API connected to tasks such as authentication, interconnect fabric, and task scheduling. 

Plugins include:

* Job_Submit_Plugin_.

* Slurm scheduler plugins (schedplugins_) are Slurm plugins that implement the Slurm scheduler API.

* SPANK_ - *Slurm Plug-in Architecture for Node and job (K)control*.

* cli_filter_ Plugin API.

* The site_factor_ plugin_ is designed to provide the site a way to build a custom multifactor priority factor, and will only be loaded and operation alongside *PriorityType=priority/multifactor*.

.. _plugin: https://slurm.schedmd.com/plugins.html
.. _Job_Submit_Plugin: https://slurm.schedmd.com/job_submit_plugins.html
.. _site_factor: https://slurm.schedmd.com/site_factor.html
.. _schedplugins: https://slurm.schedmd.com/schedplugins.html
.. _SPANK: https://slurm.schedmd.com/spank.html
.. _cli_filter: https://slurm.schedmd.com/cli_filter_plugins.html

Job submit plugins
------------------

The Job_Submit_Plugin_ (a Lua_ plugin) will execute a Lua_ script named ``/etc/slurm/job_submit.lua`` on the slurmctld_ host.
Some clarification of the documentation is needed, however, see bug_14472_ and bug_14500_.

Sample Lua_ scripts can be copied from the Slurm_ source distribution in the directories ``contribs/lua/`` and ``etc/``:

* job_submit.lua.example_
* job_submit.lua_
* job_submit.license.lua_

We also provide a job submit plugin in https://github.com/OleHolmNielsen/Slurm_tools/tree/master/plugins

.. _job_submit.lua.example: https://github.com/SchedMD/slurm/blob/master/etc/job_submit.lua.example
.. _job_submit.lua: https://github.com/SchedMD/slurm/blob/master/contribs/lua/job_submit.lua
.. _job_submit.license.lua: https://github.com/SchedMD/slurm/blob/master/contribs/lua/job_submit.license.lua

Please note that job_submit.lua.example_ has an issue with use of ``log.user()`` in ``job_modify()`` prior to Slurm 23.02, see bug_14539_.

.. _bug_14539: https://bugs.schedmd.com/show_bug.cgi?id=14539

On the slurmctld_ server you may start with this example::

  cp ~/rpmbuild/BUILD/slurm-22.05.8/etc/job_submit.lua.example /etc/slurm/job_submit.lua

(replace the 22.05 version number) and read in the Lua_manual_ about Lua_ programming.
Install also the Lua_ package::

  yum install lua

Inspiration for writing you custom ``job_submit.lua`` script can be found in:

* https://funinit.wordpress.com/2018/06/07/how-to-use-job_submit_lua-with-slurm/
* https://github.com/edf-hpc/slurm-llnl-misc-plugins/blob/master/job_submit/job_submit.lua

It is **strongly recommended** to check your Lua_ code before using it with Slurm_!
Any error in the code might cause the slurmctld_ to crash!
If possible, verify the code on a test cluster before using it in a production cluster.

A good starting point is to make a syntax check with the luac_ compiler::

  luac -p /etc/slurm/job_submit.lua

Other Lua_ syntax checker tools can be found on the net, for example:

* https://code.google.com/archive/p/lua-checker/

.. _bug_14472: https://bugs.schedmd.com/show_bug.cgi?id=14472
.. _bug_14500: https://bugs.schedmd.com/show_bug.cgi?id=14500
.. _Lua: https://en.wikipedia.org/wiki/Lua_(programming_language)
.. _Lua_manual: https://www.lua.org/manual/
.. _luac: https://www.lua.org/manual/4.0/luac.html

Lua functions for the job_submit plugin
.......................................

When writing the Job_Submit_Plugin_ Lua_ script it is nice to have an overview of available functions and variables.
This is not well documented at present.

We have discovered the following functions (TODO: is there a list of all functions?)::

  slurm.log_info
  slurm.log_debug
  slurm.log_debug2
  slurm.log_debug3
  slurm.log_user

The function ``_get_job_req_field`` in job_submit_lua.c_ lists all available *job descriptor* fields in ``job_desc``, for example, the following may be useful::

  job_desc.partition
  job_desc.script
  job_desc.environment
  job_desc.gres
  job_desc.num_tasks
  job_desc.max_nodes
  job_desc.cpus_per_task
  job_desc.tres_per_node
  job_desc.tres_per_socket
  job_desc.tres_per_task
  job_desc.user_name

**NOTE:** If some field is **undefined** in the user's job script, for example ``max_nodes``, slurmctld_ sets an "invalid" value (see bug_15012_) which can be tested for in ``/etc/slurm/job_submit.lua``:

* Numeric values (a Lua double) if absent will be set to ``slurm.NO_VAL`` (32-bit, as defined in ``/usr/include/slurm/slurm.h``).

  For completeness, there are both 16, 32, and 64-bit integer values ``NO_VAL16, NO_VAL, NO_VAL64`` defined in ``slurm.h`` struct ``job_desc_msg_t``.

* String values (if absent) will be set to the nil_ Lua type.

.. _bug_15012:  https://bugs.schedmd.com/show_bug.cgi?id=15012.
.. _nil: https://www.lua.org/pil/2.1.html

Slurm_ error symbols ``ESLURM*`` and corresponding numeric values are defined in the file ``/usr/include/slurm/slurm_errno.h``, see also bug_14500_.
Note that only a few selected symbols ``ESLURM*`` are exposed to the Lua script, but from Slurm_ 23.02 all the error codes in ``/usr/include/slurm/slurm_errno.h`` are exposed.

Your ``/etc/slurm/job_submit.lua`` script can test for undefined values like in this example::

  slurm.ESLURM_INVALID_PARTITION_NAME=2000
  if (job_desc.partition == nil) then
    slurm.log_user("No partition specified, please specify partition")
    return slurm.ESLURM_INVALID_PARTITION_NAME
  end
  if (job_desc.max_nodes == slurm.NO_VAL) then
    slurm.log_user("No max_nodes specified, please specify a number of nodes")
    return slurm.ESLURM_INVALID_PARTITION_NAME
  end

.. _job_submit_lua.c: https://github.com/SchedMD/slurm/blob/master/src/plugins/job_submit/lua/job_submit_lua.c#L458

Configure Slurm for Lua JobSubmitPlugins
........................................

The Job_Submit_Plugin_ will only execute the Lua_ script named ``/etc/slurm/job_submit.lua`` on the slurmctld_ host, and it is not used by any other nodes.

Then configure slurm.conf_ with this parameter (undocumented prior to 22.05.3)::

  JobSubmitPlugins=lua

which will make Slurm_ use the ``/etc/slurm/job_submit.lua`` script.
Make sure to distribute slurm.conf_ to all nodes (or use a configless_ setup).

Then reconfigure ``slurmctld``::

  scontrol reconfig

If ``slurmctld`` gets an error when executing ``/etc/slurm/job_submit.lua``, it will use any previously cached script and ignore the file on disk henceforth
(see `comment 15 <https://bugs.schedmd.com/show_bug.cgi?id=14472#c15>`_ in bug_14472_).

**WARNING:**
If ``slurmctld`` does not have a cached script (because it was just restarted, for example) it may crash!
