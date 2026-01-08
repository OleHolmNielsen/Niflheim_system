.. _Slurm_operations:

================
Slurm operations
================

.. Contents:: 

Jump to our top-level Slurm page: :ref:`SLURM`

.. _Slurm: https://www.schedmd.com/
.. _Slurm_homepage: https://www.schedmd.com/
.. _Slurm_docs: https://slurm.schedmd.com/
.. _Slurm_Quick_Start: https://slurm.schedmd.com/quickstart_admin.html
.. _Command_Summary: https://slurm.schedmd.com/pdfs/summary.pdf
.. _Slurm_FAQ: https://slurm.schedmd.com/faq.html
.. _Slurm_download: https://slurm.schedmd.com/download.html
.. _Slurm_mailing_lists: https://lists.schedmd.com/cgi-bin/dada/mail.cgi/list
.. _slurm_devel_archive: https://groups.google.com/forum/#!forum/slurm-devel
.. _Slurm_publications: https://slurm.schedmd.com/publications.html
.. _Slurm_tutorials: https://slurm.schedmd.com/tutorials.html
.. _Slurm_bugs: https://support.schedmd.com
.. _Slurm_man_pages: https://slurm.schedmd.com/man_index.html
.. _slurm.conf: https://slurm.schedmd.com/slurm.conf.html
.. _scontrol: https://slurm.schedmd.com/scontrol.html
.. _sacctmgr: https://slurm.schedmd.com/sacctmgr.html
.. _slurmctld: https://slurm.schedmd.com/slurmctld.html
.. _slurmdbd: https://slurm.schedmd.com/slurmdbd.html
.. _slurmd: https://slurm.schedmd.com/slurmd.html
.. _EPEL: https://fedoraproject.org/wiki/EPEL

Testing basic functionality
===========================

We assume that you have carried out the above deployment along the lines of :ref:`Slurm_installation`, :ref:`Slurm_configuration`, :ref:`Slurm_database`, :ref:`Slurm_accounting` and :ref:`Slurm_scheduler`.

From the *Head* node try to submit an interactive job::

  srun -N1 /bin/hostname

If srun_ hangs, check the firewall settings described in :ref:`Slurm_configuration`.
Please note that interactive batch jobs from Login nodes seem to be impossible if your compute nodes are on an isolated private network relative to the Login node.

To display the job queue::

  scontrol show jobs

To submit a batch job script using sbatch_::

  sbatch -N1 <script-file>

For troubleshotting there exists a testsuite_ for Slurm_.

.. _sbatch: https://slurm.schedmd.com/sbatch.html
.. _srun: https://slurm.schedmd.com/srun.html
.. _testsuite: https://github.com/SchedMD/slurm/tree/master/testsuite

System information
------------------

Useful sysadmin commands:

* sinfo_ - view information about Slurm_ nodes and partitions.
* showpartitions_ - Print a Slurm_ cluster partition status overview with 1 line per partition.
* squeue_ - view information about jobs located in the Slurm_ scheduling queue
* scancel_ Used to signal jobs or job steps
* smap_ - graphically view information about Slurm_ jobs, partitions, and set configurations parameters
* sview_ - graphical user interface to view and modify Slurm_ state (requires gtk2)
* scontrol_ - view and modify Slurm_ configuration and state

.. _sinfo: https://slurm.schedmd.com/sinfo.html
.. _squeue: https://slurm.schedmd.com/squeue.html
.. _scancel: https://slurm.schedmd.com/scancel.html
.. _smap: https://slurm.schedmd.com/smap.html
.. _sview: https://slurm.schedmd.com/sview.html
.. _sview: https://slurm.schedmd.com/sview.html
.. _showpartitions: https://github.com/OleHolmNielsen/Slurm_tools/blob/master/partitions/

MPI setup
=========

MPI use under Slurm_ depends upon the type of MPI being used, see MPI_Users_Guide_.
The current versions of Slurm_ and OpenMPI_ support task launch using the srun_ command, see the MPI_Guide_OpenMPI_.

For OpenPMIx_ please see the :ref:`Optional_prerequisites` page.

It may be convenient to add this line to slurm.conf_::

  MpiDefault=pmi2

.. _MPI_Users_Guide: https://slurm.schedmd.com/mpi_guide.html
.. _MPI_Guide_OpenMPI: https://slurm.schedmd.com/mpi_guide.html#open_mpi
.. _OpenMPI: https://www.open-mpi.org/
.. _OpenPMIx: https://github.com/openpmix/openpmix

MPI locked memory
-----------------

MPI stacks running over Infiniband or :ref:`OmniPath` require the ability to allocate more locked memory than the default limit.
Unfortunately, user processes on login nodes may have a small memory limit (check it by ``ulimit -a``) which by default are propagated into Slurm_ jobs and hence cause fabric errors for MPI.
See the `memlock FAQ <https://slurm.schedmd.com/faq.html#memlock>`_.

This is fixed by adding to slurm.conf_::

  PropagateResourceLimitsExcept=MEMLOCK

You can view the running slurmd_ process limits by::

  cat "/proc/$(pgrep -u 0 slurmd)/limits"

CPU management
==============

It is important to understand how Slurm_ manages nodes, CPUs, tasks etc.
This is documented in the cpu_management_ page.

.. _cpu_management: https://slurm.schedmd.com/cpu_management.html

GPU accelerators
================

Configure Slurm_ for GPU_ accelerators as described in the :ref:`Slurm_configuration` page under the GRES_ section.

The *AutoDetect* configuration in gres.conf_ can be used to detect GPU hardware (currently Nvidia and AMD).

.. _GPU: https://en.wikipedia.org/wiki/Graphics_processing_unit
.. _gres.conf: https://slurm.schedmd.com/gres.conf.html
.. _GRES: https://slurm.schedmd.com/gres.html

You should set the job Default count of CPUs allocated per allocated GPU (**DefCpuPerGPU**) for each partition containing GPUs in the slurm.conf_ file, for example::

  PartitionName=xxx DefCpuPerGPU=4 ...

For accounting of GPU usage you must add to the **AccountingStorageTRES** in slurm.conf_, for example::

  AccountingStorageTRES=gres/gpu,gres/gpu:tesla

and restart slurmctld_ so that these new fields are added to the database.

Nvidia GPUs
-----------

It is possible to build Slurm_ packages which include the Nvidia NVML_ library for easy handling of GPU hardware.
NVML automatically detects GPUs, their type, cores, and NVLinks.
Quoting the GRES_ page::

  If AutoDetect=nvml is set in gres.conf, and the NVIDIA Management Library (NVML) is installed on the node and was found during Slurm configuration,
  configuration details will automatically be filled in for any system-detected NVIDIA GPU.
  This removes the need to explicitly configure GPUs in gres.conf, though the Gres= line in slurm.conf is still required in order to tell slurmctld how many GRES to expect. 

However, it is **not necessary** to include the NVML_ in your Slurm_ packages, 
since you can configure gres.conf_ manually for the GPU_ hardware in your nodes.
See the mailing list thread `Building Slurm RPMs with NVIDIA GPU support? <https://lists.schedmd.com/pipermail/slurm-users/2021-January/006697.html>`_.

.. _NVML: https://developer.nvidia.com/nvidia-management-library-nvml

Nvidia drivers
..............

Download Nvidia drivers from https://www.nvidia.com/Download/index.aspx and select the appropriate GPU_ version and host operating system.
You can also download and install Nvidia `UNIX drivers <https://www.nvidia.com/en-us/drivers/unix/>`_,
and the CUDA toolkit from https://developer.nvidia.com/cuda-downloads.

To verify the availability of GPU_ accelerators in a node run the nvidia-smi_ command::

  nvidia-smi -L

which is installed with the *xorg-x11-drv-nvidia* RPM package.

.. _nvidia-smi: https://docs.nvidia.com/deploy/nvidia-smi/

GPU monitoring tools
---------------------

There is a useful page `Top 3 Linux GPU Monitoring Command Line Tools <https://www.gpu-mart.com/blog/top-3-linux-gpu-monitoring-command-line-tools>`_
recommending the tools gpustat_, nvtop_, and nvitop_.
The NVIDIA tool nvidia-smi_ can of course also be used.

We recommend the gpustat_ tool which gives a 1-line status of each GPU in the system.
The installation on EL8 systems is a bit tricky, so use these commands::

  dnf install gcc python3-devel
  python3 -m pip install setuptools-scm
  python3 -m pip install gpustat 

Our Slurm_ monitoring tools psjob_ and psnode_ use gpustat_ on nodes with GPU GRES to print a GPU usage summary.

The *Slurm Profiling Service* sps_ is a lightweight job profiler which bridges the gap between numerical job stats and full-blown application profiling.
The sps_ is a SPANK_ plugin.
When built with GPU support it will also monitor any NVIDIA or AMD GPUs.
After the job finishes, a job Epilog_ task (see Prolog_and_Epilog_Guide_) calls a series of linked clean up scripts which visualise the data into both ASCII and PNG plots,
then writes the ASCII charts to the Slurm job log and compresses everything into a tarball.

.. _gpustat: https://github.com/wookayin/gpustat
.. _nvtop: https://github.com/Syllo/nvtop
.. _nvitop: https://github.com/XuehaiPan/nvitop
.. _psjob: https://github.com/OleHolmNielsen/Slurm_tools/blob/master/jobs/psjob
.. _psnode: https://github.com/OleHolmNielsen/Slurm_tools/blob/master/nodes/psnode
.. _sps: https://github.com/mhagdorn/sps
.. _SPANK: https://slurm.schedmd.com/spank.html
.. _Prolog_and_Epilog_Guide: https://slurm.schedmd.com/prolog_epilog.html
.. _Epilog: https://slurm.schedmd.com/slurm.conf.html#OPT_Epilog

Utilities for Slurm
===================

Here we list some useful third-party utilities that Slurm_ administrators or users may find useful:

* A comprehensive list of tools on the Slurm_download_ page.

* Slurm_ tools by Ole Holm Nielsen: https://github.com/OleHolmNielsen/Slurm_tools
  including:

  - pestat_ prints a node status list (1 host per line) with information about jobids, users and CPU loads.

* `SlurmCommander <https://github.com/CLIP-HPC/SlurmCommander>`_ is a simple, lightweight, no-dependencies text-based user interface (TUI) to your cluster.
  It ties together multiple slurm commands to provide you with a simple and efficient interaction point with slurm.

* `STUBL - Slurm Tools and UBiLities <https://github.com/ubccr/stubl>`_.

* `birc-aeh/slurm-utils <https://github.com/birc-aeh/slurm-utils>`_:
  *gnodes* gives a visual representation of your cluster. 
  *jobinfo* tries to collect information for a full job.

* `slurm_showq <https://github.com/fasrc/slurm_showq>`_ A *showq* style job summary utility for Slurm.

* `slurm-quota <https://github.com/rackslab/slurm-quota>`_ assigns CPU and GPU minute quotas to users and accounts on Slurm clusters,
  and to block Slurm_ job submissions and modifications when these quotas are reached.

.. _schedtop: https://svn.princeton.edu/schedtop/
.. _pestat: https://github.com/OleHolmNielsen/Slurm_tools/tree/master/pestat

Graphical monitoring tools
--------------------------

There exist a number of Open Source tools for graphical monitoring of Slurm:

* Slurm-web_ provides a web interface on top of Slurm_ with intuitive graphical views, clear insights and advanced visualizations to track your jobs and monitor status of HPC supercomputers in your organization.

* Open XDMoD_ is an open source tool to facilitate the management of high performance computing resources. 

* `Graphing sdiag with Graphite <https://giovannitorres.me/graphing-sdiag-with-graphite.html>`_ using Graphite_.
  See also `slurm-diamond-collector <https://github.com/fasrc/slurm-diamond-collector>`_.

* `Prometheus Slurm Exporter <https://github.com/vpenso/prometheus-slurm-exporter>`_ with a Grafana_ Slurm_dashboard_.

* `Slurmbrowser <https://source.uit.no/roy.dragseth/slurmbrowser/blob/master/README.md>`_ A really thin web layer above Slurm.
  This tool requires *Ganglia*.  Install first the RPMs ``python-virtualenv python2-bottle``.

.. _Slurm-web: https://slurm-web.com/
.. _XDMoD: https://open.xdmod.org/
.. _Graphite: https://graphite.readthedocs.org/en/latest/
.. _Grafana: https://grafana.com/
.. _Slurm_dashboard: https://grafana.com/dashboards/4323

Working with Compute nodes
==========================

.. _Slurm_power_saving:

Slurm power saving 
--------------------------

Slurm_ provides an integrated power saving mechanism for powering down idle nodes, 
and starting them again when jobs need to be scheduled, 
see the Slurm_Power_Saving_Guide_.

We provide some Slurm_power_saving_scripts_ which may be useful for power management using IPMI_ or with cloud services.

.. _Slurm_Power_Saving_Guide: https://slurm.schedmd.com/power_save.html
.. _Slurm_power_saving_scripts: https://github.com/OleHolmNielsen/Slurm_tools/tree/master/power_save
.. _IPMI: https://en.wikipedia.org/wiki/Intelligent_Platform_Management_Interface

Expanding and collapsing host lists
-----------------------------------

Slurm_ lists node/host lists in the compact format, for example ``node[001-123]``.
Sometimes you want to expand the host list, for example in scripts, to list all nodes individually.

You can use this command to output hostnames one line at a time::

  scontrol show hostnames node[001-123]

or rewrite the list into a single line with paste_::

  scontrol show hostnames node[001-123] | paste -s -d ,

.. _paste: https://en.wikipedia.org/wiki/Paste_(Unix)

To contract expanded hostlists::

  # scontrol show hostlistsorted h003,h002,h001
  h[001-003]
  # scontrol show hostlist h003,h002,h001
  h[003,002,001]

When the server does not have the *slurm* RPM installed,
or for more sophisticated host list processing,
some non-Slurm tools may be used as shown below.

The nodeset command
...................

The ClusterShell_tool_ 's nodeset_ command (see below) enables easy manipulation of node sets, as well as node groups, at the command line level. 
For example::

  $ nodeset --expand node[13-15,17-19]
  node13 node14 node15 node17 node18 node19

The hostlist command
....................

The python-hostlist_ tool is very convenient for expanding or compressing node lists.

To install this tool (make sure to download the latest release)::

  dnf install python3-devel
  wget https://www.nsc.liu.se/~kent/python-hostlist/python-hostlist-2.2.1.tar.gz
  rpmbuild -ta python-hostlist-2.2.1.tar.gz
  dnf install ~/rpmbuild/RPMS/noarch/python-hostlist-2.2.1-1.el8.x86_64
  
For usage see the python-hostlist_, but a useful example is::

  # hostlist --expand --sep " "  n[001-012]
  n001 n002 n003 n004 n005 n006 n007 n008 n009 n010 n011 n012

.. _python-hostlist: https://www.nsc.liu.se/~kent/python-hostlist/

The snodelist command
......................

The snodelist_ command is a tool for working with Slurm_ hostlists.
Rather than relying on ``scontrol show hostnames`` to expand a Slurm_ compact host list to a newline-delimited list.
Installation instructions are in the snodelist_ page.

.. _snodelist: https://github.com/University-of-Delaware-IT-RCI/snodelist

Hostlist expressions for left-hand and right-hand nodes or other node increments
..................................................................................

When managing separately the left-hand and right-hand nodes in a Lenovo compute tray,
or any other subset of compute nodes,
the ClusterShell_tool_ comes in handily for selecting subsets of nodes.
Let us assume that nodes are named numerically so that left-hand nodes have odd numbers,
whereas right-hand nodes have even numbers, for example the *left,right,left,right,...* nodes::

  e001,e002,...,e023,e024

The clush_ command can now perform commands separately::

  clush -bw e[001-023/2] echo I am a left-hand node
  clush -bw e[002-024/2] echo I am a right-hand node

Unfortunately, Slurm_ doesn't recognize this syntax of node number increments.
Here you can use the ClusterShell_tool_'s command nodeset_ to print Slurm_ compatible nodelists to be used as Slurm_ command arguments::

  $ nodeset -f e[001-024/2]
  e[001,003,005,007,009,011,013,015,017,019,021,023]
  $ nodeset -f e[002-024/2]
  e[002,004,006,008,010,012,014,016,018,020,022,024]

An example where we assign nodelists to variables::

  $ export nodelist=e[001-024]
  $ export left=`nodeset -f e[001-024/2]`
  $ export right=`nodeset -f e[002-024/2]`
  $ sinfo -n $left

SSH keys for password-less access to cluster nodes
--------------------------------------------------

Users may have a need for SSH access to Slurm_ compute nodes, for example,
if they have to use an MPI library which is using SSH in stead of Slurm_ to start MPI tasks.

However, it is a good idea to configure the slurm-pam-adopt_ module on the nodes to control and restrict SSH access, 
see `<Slurm_configuration#pam-module-restrictions>`_.

The SSH_ (*Secure Shell*) configuration files including server private/public keys are in the ``/etc/ssh/`` folder.

The file ``/etc/ssh/ssh_known_hosts`` containing the SSH *public keys* of all nodes should be created on the central server and distributed to all Slurm_ nodes.
The ssh-keyscan_ tool is very convenient for gathering SSH *public keys* of the cluster nodes, some examples are::

  ssh-keyscan -t ssh-ed25519 node001 node002                   # Scan nodes node001+node002 for key type ssh-ed25519
  scontrol show hostnames node[001-022] | ssh-keyscan -f - 2>/dev/null | sort # Scan nodes node[001-022], pipe comments to /dev/null, and sort the output
  sinfo -Nho %N | uniq | ssh-keyscan -f - 2>/dev/null | sort          # Scan all Slurm nodes (uniq suppresses duplicates)

Remember to set the SELinux context correctly for the files in ``/etc/ssh``::

  chcon system_u:object_r:etc_t:s0 /etc/ssh/ssh_known_hosts

When all SSH *public keys* of the Slurm_ nodes are available in ``/etc/ssh/ssh_known_hosts``, each individual user can configure a password-less SSH login.
First the user must generate personal SSH keys (placed in the ``$HOME/.ssh/`` folder) using the ssh-keygen_ tool.

Each user may use the convenient tool authorized_keys_ for generating SSH keys and adding them to the ``$HOME/.ssh/authorized_keys`` file.

For external computers the personal SSH_authorized_keys_
(preferably protected with a *passphrase* or *Multi-Factor Authentication*) should be used.

For the servers running the slurmctld_ and slurmdbd_ services it is strongly recommended **not** to permit login by normal users because they have no business on those servers!
To restrict which users can login to the management hosts, append this line to the SSH server ``/etc/ssh/sshd_config`` file::

  AllowUsers root 

You can add more trusted system managers to this line if needed.
Then restart the SSH service::

  systemctl restart sshd

.. _slurm-pam-adopt: https://slurm.schedmd.com/pam_slurm_adopt.html

Host-based authentication
.........................

Another way to enable password-less SSH login is to configure login nodes and compute nodes in the cluster to allow Host-based_Authentication_.
Please beware that:

* For security reasons it is strongly recommended **not** to include the Slurm_ slurmctld_ and slurmdbd_ servers in the Host-based_Authentication_
  because normal users have no business on those servers!
* For security reasons the **root** user is not allowed to use Host-based_Authentication_.
  You can add root's public key to the ``/root/.ssh/authorized_keys`` file on all compute nodes for easy SSH access.
* Furthermore, personal computers and other computers outside the cluster **MUST NOT be trusted** by the cluster nodes!
  For external computers the personal SSH_authorized_keys_ (preferably protected with a *passphrase* or Multi_Factor_Authentication_) should be used.
* You need to understand that Host-based_Authentication_ is a *bad idea in general*,
  but that it is a good and secure solution within a single Linux cluster's security perimeter, see for example:

  * `Implementing ssh hostbased authentication <https://hea-www.harvard.edu/~fine/Tech/ssh-host-based.html>`_.
  * The mailing list thread at https://lists.schedmd.com/pipermail/slurm-users/2020-June/005578.html

* It is recommended to configure the slurm-pam-adopt_ module on the nodes to control and restrict SSH access, see :ref:`pam-module-restrictions`.

Here are the steps for configuring Host-based_Authentication_:

1. First populate all SSH keys in the file ``/etc/ssh/ssh_known_hosts`` as shown above.

2. Configure **only** these lines in the SSH client configuration ``/etc/ssh/ssh_config`` on all nodes::

     HostbasedAuthentication yes
     EnableSSHKeysign yes

   These lines do not work inside *Host* or *Match* statements, but must be defined at the global level.

   You may also configure *PreferredAuthentications* (order of authentication methods) so that the *hostbased* method is preferred
   for the nodes in the cluster's domainname (replace by your DNS domain).
   Furthermore *GSSAPI* and *ForwardX11Trusted* may be configured::

     Host *.<domainname>
       PreferredAuthentications gssapi-keyex,gssapi-with-mic,hostbased,publickey,keyboard-interactive,password
       GSSAPIAuthentication yes
       ForwardX11Trusted yes

   The ssh_config_ manual page explains the configuration keywords.

   The *GSSAPI* (Generic Security Service Application Program Interface (GSS-API) Authentication and Key Exchange for the Secure Shell (SSH) Protocol) is defined in rfc4462_.

3. Add these lines to the SSH server ``/etc/ssh/sshd_config`` file on all nodes::

     HostbasedAuthentication yes
     UseDNS yes

   and restart the SSH service::

     systemctl restart sshd

4. Populate the file ``/etc/ssh/shosts.equiv`` for **every** node in the cluster listed in ``/etc/ssh/ssh_known_hosts`` with 1 line per node **including** the full DNS domainname, for example::

     node001.<domainname>
     node002.<domainname>
     ...

   Wildcard hostnames are not possible, so you must list **all** hosts one per line.
   To list all cluster nodes::

     sinfo -Nho %N | uniq | awk '{print $1 ".domainname"}' > /etc/ssh/shosts.equiv

   where you must substitute your own *domainname*.

Remember to set the SELinux context correctly for the files in ``/etc/ssh``::

  chcon system_u:object_r:etc_t:s0 /etc/ssh/sshd_config /etc/ssh/ssh_config /etc/ssh/shosts.equiv /etc/ssh/ssh_known_hosts

A normal (non-root) user should now be able to login from a node to itself, for example::

  testnode$ ssh -v testnode

and the verbose output should inform you::

  debug1: Authentication succeeded (hostbased).

.. _SSH: https://en.wikipedia.org/wiki/Secure_Shell
.. _ssh-keyscan: https://linux.die.net/man/1/ssh-keyscan
.. _ssh-keygen: https://www.ssh.com/ssh/keygen/
.. _ssh_config: https://linux.die.net/man/5/ssh_config
.. _SSH_authorized_keys: https://www.ssh.com/ssh/authorized_keys/openssh
.. _Host-based_Authentication: https://en.wikibooks.org/wiki/OpenSSH/Cookbook/Host-based_Authentication
.. _authorized_keys: https://github.com/OleHolmNielsen/Slurm_tools/tree/master/SSH
.. _rfc4462: https://www.ietf.org/rfc/rfc4462.txt
.. _Multi_Factor_Authentication: https://www.redhat.com/sysadmin/mfa-linux

.. _clustershell:

ClusterShell
------------

The ClusterShell_tool_ is an event-driven open source Python library, designed to run local or distant commands in parallel on server farms or on large Linux clusters.
It provides a light and unified command execution Python framework to help administer GNU/Linux or BSD clusters.
There is a ClusterShell_manual_ and a ClusterShell_configuration_ guide,
and you can view the ClusterShell_source_.

.. _ClusterShell_manual: https://clustershell.readthedocs.io/en/latest/
.. _ClusterShell_configuration: https://clustershell.readthedocs.io/en/latest/config.html
.. _ClusterShell_tool: https://clustershell.readthedocs.io/en/latest/intro.html
.. _ClusterShell_source: https://github.com/cea-hpc/clustershell

Install the ClusterShell_tool_ from the EPEL_ repository::

  dnf install epel-release
  dnf install clustershell

Copy the example file for Slurm.conf_::

  cp /etc/clustershell/groups.conf.d/slurm.conf.example /etc/clustershell/groups.conf.d/slurm.conf

You should define *slurm* as the default group in ``/etc/clustershell/groups.conf``::

  [Main]
  # Default group source
  default: slurm

.. _zero-padding: https://clustershell.readthedocs.io/en/latest/tools/nodeset.html#zero-padding
.. _issue_293: https://github.com/cea-hpc/clustershell/issues/293

Using ClusterShell
....................

You can list all node groups including hostnames and node counts using this ClusterShell_tool_ command::

  cluset -LLL

Simple usage of clush_::

  clush -w node[001-003] date

For convenience you can use bash_command_completion_ for the clush_ command::

  clush -w node[TAB][TAB] [-b|-B]
  clush -g group[TAB][TAB] [-b|-B]

(note that additional arguments ``-b`` or ``-B`` must come last, they are missing in clush_PR_563_).

For a Slurm_ partition::

  clush -g <partition-name> date

If option *-b* or *--dshbak* is specified, clush_ waits for command completion while displaying a progress indicator and then displays gathered output results::

  clush -g <partition-name> -b date

To execute a command only on nodes with a specified Slurm_ state (here: ``drained``)::

  clush -w@slurmstate:drained date
  clush -bw@slurmstate:down 'uname -r; dmidecode -s bios-version'

To execute a command only on nodes running a particular Slurm_ JobID (here: 123456)::

  clush -w@sj:123456 <command>

To execute a command only on nodes running jobs for a particular username (requires the above mentioned *slurmuser* configuration)::

  clush -w@su:username <command>

If you want to run commands on hosts **not** under Slurm, select a group source defined in /etc/clustershell/groups (see ``man clush``)::

  clush -s GROUPSOURCE or --groupsource=GROUPSOURCE <other arguments>

For example::

  clush -s local -g testcluster <command>

The nodeset_ command enables easy manipulation of node sets, as well as node groups, at the command line level. 
For example::

  $ nodeset --expand node[13-15,17-19]
  node13 node14 node15 node17 node18 node19

.. _clush: https://clustershell.readthedocs.io/en/latest/tools/clush.html
.. _nodeset: https://clustershell.readthedocs.io/en/latest/tools/nodeset.html
.. _bash-completion: https://github.com/scop/bash-completion
.. _bash_command_completion: https://www.gnu.org/software/gnuastro/manual/html_node/Bash-TAB-completion-tutorial.html
.. _clush_PR_563: https://github.com/cea-hpc/clustershell/pull/563

Skipping powered_down nodes
.............................

The clush_ command will obviously be unable to run commands on nodes that have been powered down by :ref:`Slurm_power_saving`.
It is therefore desirable to configure the ClusterShell_tool_ to skip powered down nodes.

A **very useful** new state filtering was added to the sinfo_ command starting from Slurm_ 25.05::

  The state can be prefixed with '~' which will invert the result of match.

We can use this to specify the state ``~POWERED_DOWN`` (meaning *not powered down*),
so that nodes that have been powered down by :ref:`Slurm_power_saving` will be skipped by clush_.
See also ticket_21428_.

An example usage of this is::

  clush -bw@slurmstate:~POWERED_DOWN uname -r

You can even configure the ``clush -a`` command to include **only** the ``~POWERED_DOWN`` nodes by editing the file
``/etc/clustershell/groups.conf.d/slurm.conf`` to add to the ``all:`` line a specific state by ``-t``::

  [slurmpart,sp]
  map: sinfo -h -o "%N" -p $GROUP
  all: sinfo -h -o "%N" -t ~POWERED_DOWN
  ...

Note that nodes with states such as ``Down`` (for example, crashed nodes) will not be skipped!

.. _ticket_21428: https://support.schedmd.com/show_bug.cgi?id=21428

Copying files with ClusterShell
...............................

When ClusterShell_tool_ has been set up, it's very simply to copy files and folders to nodes, see the clush_ manual page.
Example::

  clush -bw node[001-099] --copy /etc/slurm/slurm.conf --dest /etc/slurm/

Listing nodes
-------------

Use sinfo_ to list nodes that are responding (for example, to be used in clush_ scripts)::

  sinfo -r -h -o '%n'
  sinfo --responding --noheader --format='%n'

List reasons nodes are in the down, drained, fail or failing state::

  sinfo -R
  sinfo --list-reasons
  sinfo -lRN

List of nodes with features and status::

  sinfo --format="%25N %.40f %.6a %.10A"

Use scontrol_ to list node properties::

  scontrol -o show nodes <Nodename>

.. _sinfo: https://slurm.schedmd.com/sinfo.html

Listing node resources used
---------------------------

Use sinfo_ to see what resources are used/remaining on a per node basis::

  sinfo -Nle -o '%n %C %t'

The flag ``-p <partition>`` may be added.
Nodes states listed with \* means that the node is **not responding**.

Note the *STATE* column:

* State of the nodes. Possible states include: allocated, completing, down, drained, draining, fail, failing, future, idle, maint, mixed, perfctrs, power_down, power_up, reserved, and unknown plus Their abbreviated forms: alloc, comp, down, drain, drng, fail, failg, futr, idle, maint, mix, npc, pow_dn, pow_up, resv, and unk respectively.

  Note that the suffix "*" identifies nodes that are presently not responding. 

Listing node events
---------------------------

Use sacctmgr_ to list node events that are stored in the Slurm_ database,
like ``boot``, ``down`` or ``drained`` nodes (but **not** ``draining`` nodes)::

  sacctmgr show event where nodes=XXX

Optional flags can be used to modify the database search or the output fields,
see SPECIFICATIONS_FOR_EVENTS_ and SHOW-EVENT-FORMAT-OPTIONS_.
Default period of start of events is 00:00:00 of the previous day.
For example::

  sacctmgr show event where nodes=XXX All_Time
  sacctmgr show event where nodes=XXX Start=now-4weeks
  sacctmgr show event where nodes=XXX States=boot
  sacctmgr show event where nodes=XXX Format=NodeName,TimeStart,Duration,State,Reason
  sacctmgr show event where nodes=XXX Format=NodeName,TimeStart,Duration,State,Reason%-40

.. _SPECIFICATIONS_FOR_EVENTS: https://slurm.schedmd.com/sacctmgr.html#SECTION_SPECIFICATIONS-FOR-EVENTS
.. _SHOW-EVENT-FORMAT-OPTIONS: https://slurm.schedmd.com/sacctmgr.html#SECTION_LIST/SHOW-EVENT-FORMAT-OPTIONS

Resume an offline node
----------------------

A node may get stuck in an offline mode for several reasons.
For example, you may see this::

  # scontrol show node q007

  NodeName=q007 Arch=x86_64 CoresPerSocket=2
  ...
   State=DOWN ThreadsPerCore=1 TmpDisk=32752 Weight=1 Owner=N/A
  ...
   Reason=NO NETWORK ADDRESS FOUND [slurm@2015-12-08T09:25:32]

Nodes states listed with \* means that the node is **not responding**.

It is very difficult to find documentation on how to clear such an offline state.
The solution is to use the scontrol_ command (section *SPECIFICATIONS FOR UPDATE COMMAND, NODES*)::

  scontrol update nodename=a001 state=down reason="undraining"
  scontrol update nodename=a001 state=resume

See also `How to "undrain" slurm nodes in drain state <https://stackoverflow.com/questions/29535118/how-to-undrain-slurm-nodes-in-drain-state>`_
where it is recommended to avoid the *down* state (1st command above).

Slurm trigger information
-------------------------

Triggers include events such as:

* a node failing
* daemon stops or restarts
* a job reaching its time limit
* a job terminating. 

These events can cause actions such as the execution of an arbitrary script. 
Typical uses include notifying system administrators of node failures and gracefully terminating a job when it's time limit is approaching. 
A hostlist expression for the nodelist or job ID is passed as an argument to the program. 

* strigger_ - Used set, get or clear Slurm_ trigger information

An example script using this is notify_nodes_down_.
To set up the trigger as the *slurm* user::

   slurm# strigger --set --node --down --program=/usr/local/bin/notify_nodes_down

To display enabled triggers::

  strigger --get

.. _notify_nodes_down: https://github.com/OleHolmNielsen/Slurm_tools/tree/master/triggers
.. _strigger: https://slurm.schedmd.com/strigger.html

Add and remove nodes
--------------------

Nodes can be added or removed by modifying the slurm.conf_ file and distributing it to all nodes.
If you use the topology.conf_ configuration, that file must also be updated and distributed to all nodes.
If you run a :ref:`configless-slurm-setup` setup then the configuration files are served automatically to nodes by the slurmctld_.

Starting in Slurm_ 22.05, nodes can be dynamically added and removed from Slurm, see dynamic_nodes_.

.. _dynamic_nodes: https://slurm.schedmd.com/dynamic_nodes.html

If nodes must initially be unavailable for starting jobs, define them in slurm.conf_ with a *State* and optionally a *Reason* parameter::

  NodeName=xxx ... State=DRAIN Reason="Not yet ready"
  NodeName=xxx ... State=FUTURE

For convenience the command::

  slurmd -C

can be used on each compute node to print its physical configuration (sockets, cores, real memory size, etc.) for inclusion into slurm.conf_.

An entire new partition may also be made unavailable using a *State* not equal to *UP*::

  PartitionName=xxx ... State=INACTIVE
  PartitionName=xxx ... State=DRAIN

.. _topology.conf: https://slurm.schedmd.com/topology.conf.html

However, the slurmctld_ daemon must then be restarted::

  systemctl restart slurmctld

As stated in the scontrol_ page under the *reconfigure* option):

* The slurmctld_ daemon must be restarted if nodes are added to or removed from the cluster. 

Furthermore, the slurmd_ service on all compute nodes must also be restarted in order to pick up the changes in slurm.conf_, for example::

  clush -ba systemctl restart slurmd

See advice from the Slurm_publications_ talk *Technical: Field Notes Mark 2: Random Musings From Under A New Hat, Tim Wickberg, SchedMD* (2018) on the *Safe procedure*:

1. Stop slurmctld
2. Change configs
3. Restart all slurmd processes
4. Start slurmctld

Less-Safe, but usually okay, procedure:

1. Change configs
2. Restart slurmctld
3. Restart all slurmd processes really quickly

See also https://thread.gmane.org/gmane.comp.distributed.slurm.devel/3039 (comment by Moe Jette).

Rebooting nodes
---------------

Slurm_ can reboot nodes by::

  scontrol reboot [ASAP] [NodeList]
    Reboot  all nodes in the system when they become idle using the RebootProgram as configured in Slurm's slurm.conf file.
    The option "ASAP" prevents initiation of additional jobs so the node can be rebooted and returned to service "As Soon As Possible" (i.e. ASAP).
    Accepts an option list of nodes to reboot.
    By default all nodes are rebooted. 

**NOTE**: The reboot request will be ignored for hosts in the following states: ``FUTURE, POWER_DOWN, POWERED_DOWN, POWERING_DOWN, REBOOT_ISSUED, REBOOT_REQUESTED``,
see ticket_18505_.
Currently, no warning is issued in such cases.
From Slurm_ 24.08 an error message will be printed by ``scontrol reboot`` when a node reboot request is ignored due to the current node state.

.. _ticket_18505: https://support.schedmd.com/show_bug.cgi?id=18505

Compute node OS and firmware updates
------------------------------------

Regarding the question of methods for Slurm_ compute node OS and firmware updates, 
we have for a long time used rolling updates while the cluster is in full production, 
so that we do not waste any resources.

When entire partitions are upgraded in this way, there is no risk of starting new jobs on nodes with differing states of OS and firmware, 
while running jobs continue on the not-yet-updated nodes.

The basic idea (which was provided by Niels Carl Hansen, ncwh -at- cscaa.dk) is to run a crontab_ script ``update.sh`` whenever a node is rebooted.  
Use scontrol to reboot the nodes as they become idle, thereby performing the updates that you want.  
Remove the crontab_ job as part of the ``update.sh`` script.

The ``update.sh`` script and instructions for usage are in:
https://github.com/OleHolmNielsen/Slurm_tools/tree/master/nodes

.. _crontab: https://linux.die.net/man/5/crontab

Shared Memory cleanup
---------------------

Certain jobs allocate Shared Memory resources but do not release them before job completion.
For example, the shared memory segments may hit the system limit (typically 4096), see the system limit by::

  $ sysctl kernel.shmmni
  kernel.shmmni = 4096

Error messages such as this one may occur::

  getshmem_C in getshmem.c: cannot create shared segment 8
  No space left on device 

See also ticket_7232_.

Information on the inter-process communication facilities::

  ipcs -a

Users and root can clean up unused data by::

  ipcrm -a

.. _ticket_7232: https://support.schedmd.com/show_bug.cgi?id=7232

.. _resource_reservation:

Resource Reservation
====================

Compute nodes can be reserved for a number of purposes.
Read the reservations_ guide.

.. _reservations: https://slurm.schedmd.com/reservations.html

For example, to **reserve a set of nodes** for a testing purpose with a duration of 720 hours::

  scontrol create reservation starttime=now duration=720:00:00 ReservationName=Test1 Flags=MAGNETIC nodes=x[049-096] user=user1,user2

Ignore currently running jobs when creating the reservation by adding this flag::

  flags=ignore_jobs

**Magnetic reservations** were introduced in Slurm_ 20.02, see the scontrol_ man-page::

  Flags=MAGNETIC  # This flag allows jobs to be considered for this reservation even if they didn't request it.

Jobs will be eligible to run in such reservations even if they did not specify ``--reservation``.

To **reserve nodes for maintenance** for 72 hours::

  scontrol create reservation starttime=2017-06-19T12:00:00 duration=72:00:00 ReservationName=Maintenance flags=maint,ignore_jobs nodes=x[145-168] user=root

A specification of **nodes=ALL** will reserve all nodes.

If you want to reserve an **entire partition**, it is recommended to **not** specify nodes, but a partition in stead::

  scontrol create reservation starttime=2017-06-19T12:00:00 duration=72:00:00 ReservationName=Maintenance flags=maint,ignore_jobs partitionname=xeon16 user=root

To list all reservations::

  scontrol show reservations

and also previous reservations some weeks back in time::

  scontrol show reservations start=now-5weeks

Batch jobs submitted for the reservation must explicitly refer to it, for example::

  sbatch --reservation=Test1 -N4 my.script

One may also specify explicitly some nodes::

  sbatch --reservation=Test1 -N2 --nodelist=x188,x140 my.script

Working with jobs
=================

Tutorial pages about Slurm_ job management:

* `Convenient Slurm Commands <https://rc.fas.harvard.edu/resources/documentation/convenient-slurm-commands/>`_

Interactive jobs
----------------

Using srun_ users can launch interactive jobs on compute nodes through Slurm.
See the FAQ `How can I get shell prompts in interactive mode? <https://slurm.schedmd.com/faq.html#prompt>`_::

  srun --pty bash -i [additional options]

If you need to run MPI tasks, see MPI_Guide_OpenMPI_.
It is required to invoke srun_ with pmi2 or pmix support as shown above in the MPI section, for example::

  srun --pty --mpi=pmi2 bash -i [additional options]

Job arrays
----------

Slurm_ job_arrays_ offer a mechanism for submitting and managing collections of similar jobs quickly and easily.
Please note that job_arrays_ are subject to the **same limits**
(such as MaxSubmitJobs_) as ordinary batch jobs.

It is important to understand that job arrays, only at the moment when an individual job starts running, become independent jobs (similar to non-array jobs) 
and are assigned their own unique JobIDs. 

To see the relationship between job arrays and JobIDs, this is a useful command for a specified ArrayJobID::

  $squeue  -j 3394902 -O ArrayJobID,JobArrayID,JobID,State
  ARRAY_JOB_ID        JOBID               JOBID               STATE               
  3394902             3394902_[34-91]     3394902             PENDING             
  3394902             3394902_30          3394932             RUNNING             
  3394902             3394902_28          3394930             RUNNING            

.. _job_arrays: https://slurm.schedmd.com/job_array.html
.. _MaxSubmitJobs: https://slurm.schedmd.com/sacctmgr.html#OPT_MaxSubmitJobs

Useful commands
---------------

See the overview of Slurm_man_pages_ as well as the individual command man-pages.

.. list-table::
  :widths: 4 4

  * - **Command**
    - **Function**
  * - squeue_
    - List jobs
  * - squeue_ --start
    - List starting times of jobs
  * - sbatch_ <options> --wrap="some-command"
    - Submit a job running just ``some-command`` (without script file)
  * - scontrol_ show job xxx
    - Get job details
  * - scontrol_ --details show job xxx
    - Get more job details
  * - scontrol_ suspend xxx
    - Suspend a job (root only)
  * - scontrol_ resume xxx
    - Resume a job (root only)
  * - scontrol_ hold xxx
    - Hold a job 
  * - scontrol_ uhold xxx
    - User-Hold a job 
  * - scontrol_ release xxx
    - Release a held job 
  * - scontrol_ update jobid=10208 nice=-10000
    - Increase a job's priority (Slurm managers only)
  * - scontrol_ update jobid=10208 nice=5000
    - Decrease a job's priority (users and managers)
  * - scontrol_ top 10208
    - Move the job to the top of the user's queue
  * - scontrol_ update jobid=10208 priority=50000
    - Set a job's priority value
  * - scontrol_ hold jobid=10208; scontrol_ release jobid=10208
    - Reset a job's explicit priority=xxx value
  * - scontrol_ update jobid=1163 EndTime=2022-04-27T08:30:00
    - Modify a job's End time
  * - scontrol_ update jobid=1163 timelimit=12:00:00
    - Modify a job's time limit
  * - scontrol_ update jobid=1163 qos=high
    - Set the job QOS to *high* (QOS list: ``sacctmgr show qos``)
  * - scontrol_ listpids <jobid> (on node running a job)
    - Print  a  listing  of  the  process  IDs in a job step
  * - scontrol_ write batch_script job_id optional_filename
    - Write the batch script for a given job_id to a file or to stdout
  * - scontrol_ show config
    - Prints the Slurm configuration and running parameters
  * - scontrol_ write config optional_filename
    - Write the current Slurm configuration to a file
  * - scancel_ job xxx
    - Kill a job
  * - sjobexitmod_ -l jobid
    - Display job exit codes
  * - sstat_
    - Display various status information of a running job/step
  * - scontrol_ show assoc_mgr
    - Displays the slurmctld_'s internal cache for users, associations and/or qos such as GrpTRESRunMins, GrpTRESMins etc.
  * - scontrol_ -o show assoc_mgr users=xxx accounts=yyy flags=assoc 
    - Display the association limits and current values for user xxx in account yyy as a one-liner.
  * - sacctmgr_ show user -s xxx
    - Display information about user xxx from the Slurm database
  * - sacctmgr_ add user xxx Account=zzzz
    - Add user xxx to the non-default account zzzz, see the accounting_ page.
  * - sacctmgr_ modify qos normal set priority=50
    - Modify the the QOS_ named *normal* to set a new priority value.
  * - sacctmgr_ modify user where name=xxx set MaxSubmitJobs=NN
    - Update user's maximum number of submitted jobs to NN.  NN=0 blocks submissions, NN=-1 removes the limit.
  * - sacctmgr_ -nP list associations user=xxx format=fairshare
    - Print the fairshare number of user xxx.
  * - sacctmgr_ show event
    - Display information about events like downed or draining nodes on clusters.
  * - sshare_ -lU -u xxx
    - Print the various fairshare values of user xxx.

.. _sjobexitmod: https://slurm.schedmd.com/job_exit_code.html
.. _sstat: https://slurm.schedmd.com/sstat.html
.. _sshare: https://slurm.schedmd.com/sshare.html
.. _accounting: https://slurm.schedmd.com/accounting.html
.. _QOS: https://slurm.schedmd.com/qos.html

squeue usage
............

The squeue_ command has a huge number of parameters for listing jobs.
Here are some suggestions for usage of squeue_:
sbatch <options> --wrap="some-command"
* The long display gives more details::

    squeue -l  # is equivalent to:
    squeue -o "%.18i %.9P %.8j %.8u %.8T %.10M %.9l %.6D %R"

* Add columns for job priority (%Q) and CPU count (%C) and make some columns wider::

    squeue -o "%.18i %.9P %.8j %.8u %.10T %.9Q %.10M %.9l %.6D %.6C %R"

* Set the output format by an environment variable::

    export SQUEUE_FORMAT="%.18i %.9P %.8j %.8u %.10T %.9Q %.10M %.9l %.6D %.6C %R"

  or using the new output format::

    export SQUEUE_FORMAT2="JobID:8,Partition:11,QOS:7,Name:10 ,UserName:9,Account:9,State:8,PriorityLong:9,ReasonList:16 ,TimeUsed:12 ,SubmitTime:19 ,TimeLimit:10 ,tres-alloc: "

* List of pending jobs in the same order considered for scheduling by Slurm_ (see squeue_ man-page under --priority)::

    squeue --priority  --sort=-p,i --states=PD

Slurm debugging
===============

Change the debug level of the slurmctld_ daemon.::

  scontrol setdebug LEVEL

where LEVEL may be: "quiet", "fatal", "error", "info", "verbose", "debug", "debug2", "debug3", "debug4", or "debug5".
See the `scontrol OPTIONS <https://slurm.schedmd.com/scontrol.html#lbAE>`_ section.
For example::

  scontrol setdebug debug2

This value is temporary and will be overwritten whenever the slurmctld daemon reads the slurm.conf configuration file (e.g. when the daemon is restarted or scontrol reconfigure is executed). 

Add or remove DebugFlags of the slurmctld daemon::

  scontrol setdebugflags [+|-]FLAG

For example::

  scontrol setdebugflags +backfill 

See slurm.conf_ `PARAMETERS <https://slurm.schedmd.com/slurm.conf.html#lbAD>`_ section for the full list of supported DebugFlags. 
NOTE: Changing the value of some DebugFlags will have no effect without restarting the slurmctld daemon, which would set DebugFlags based upon the contents of the slurm.conf configuration file. 
