.. _Slurm_accounting:

================
Slurm accounting
================

.. Contents::

Jump to our top-level Slurm page: :ref:`SLURM`

The following configuration is relevant for the Head/Master node only.

.. _Slurm_Quick_Start: https://slurm.schedmd.com/quickstart_admin.html
.. _Slurm: https://www.schedmd.com/
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
.. _sacct: https://slurm.schedmd.com/sacct.html
.. _sstat: https://slurm.schedmd.com/sstat.html
.. _sreport: https://slurm.schedmd.com/sreport.html
.. _sacctmgr: https://slurm.schedmd.com/sacctmgr.html
.. _slurmctld: https://slurm.schedmd.com/slurmctld.html
.. _slurmdbd: https://slurm.schedmd.com/slurmdbd.html
.. _sbatch: https://slurm.schedmd.com/sbatch.html

Accounting setup in Slurm
=========================

See the accounting_ page and the Slurm_tutorials_ with *Slurm Database Usage*.

Before setting up accounting, you need to set up the :ref:`Slurm_database`.

There must be a uniform user and group name space (including UIDs and GIDs) across the cluster,
see the Slurm_Quick_Start_ Administrator Guide.

.. _accounting: https://slurm.schedmd.com/accounting.html


Database Configuration
======================

See the section *Database Configuration* in the accounting_ page.
The command to manage accounts is:

* sacctmgr_ - View and modify Slurm account information

Show the current accounting configuration by::

  sacctmgr show configuration

.. _sacctmgr: https://slurm.schedmd.com/sacctmgr.html

Associations
------------

Accounting records are maintained based upon what we refer to as an Association_, which is a **fundamental concept** consisting of four elements:

* cluster, 
* account,
* user names,
* an optional partition name. 

Use the sacctmgr_ command to create and manage the Association_ records.

Display all Association_ records by::

  sacctmgr show associations

possibly organized as a tree::

  sacctmgr show associations tree

**NOTE:** There is an order to set up accounting associations. 
You must define clusters before you add accounts, and you must add accounts before you can add users.

.. _Association: https://slurm.schedmd.com/slurm_ug_2012/SUG_Oct2012_DBD.pdf

Create a cluster
----------------

Add the cluster name to slurm.conf_::

  AccountingStorageType=accounting_storage/slurmdbd
  AccountingStoreJobComment=YES
  ClusterName=niflheim

See the *Database Configuration* section of the accounting_ page::

  sacctmgr add cluster niflheim
  sacctmgr create cluster niflheim  # The add and create commands are identical

See also https://bugs.schedmd.com/show_bug.cgi?id=1050

To display the clusters defined in the Slurm database::

  sacctmgr show clusters
  sacctmgr -nP show clusters format=cluster   # No headers and just the cluster names

You can delete a cluster *xxx* by::

  sacctmgr delete cluster xxx

Create accounts and users
=========================

See the *Database Configuration* section of the accounting_ page.

Create accounts
---------------

**Definition of accounts**:

* Not formally defined in the Slurm_ documentation, but see the accounting_ page for examples.
* An account is similar to a UNIX group.
* An account may contain multiple users, or just a single user.
* Accounts may be organized as a hierarchical tree.
* A user may belong to multiple accounts, but **must** have a *DefaultAccount* (see sacctmgr_).

Create a hierarchical organization list using sacctmgr_, for example, with departments and external users::

  sacctmgr add account dtu Description="DTU departments" Organization=dtu

  sacctmgr add account fysik Description="Physics department" Organization=fysik parent=dtu
  sacctmgr add account deptx Description="X department" Organization=deptx parent=dtu
  
  sacctmgr add account external Description="External groups" Organization=external
  sacctmgr add account companyx Description="Company X" Organization=companyx parent=external

You may also create subgroups within the departments::

  sacctmgr add account camd Description="CAMD section" Organization=camd parent=fysik

If you wish to assign different resources within the departmental subgroups, you could use the UNIX GID_ group name to differentiate between faculty, staff and students, for example.
Use the GID_ names from the ``/etc/group`` file to create new accounts within the same *Organization* name, for example, for the *CAMD section* students with GID_ group name *camdstud*::

  sacctmgr add account camdstud Description="CAMD students" Organization=camd parent=camd

.. _GID: https://en.wikipedia.org/wiki/Group_identifier

Display the accounts created::

  sacctmgr show account
  sacctmgr show account -s   # Show also associations in the accounts

In addition, the sshare_ command displays the account hierarchy::

  sshare

When either adding or modifying an account, the following sacctmgr_ options are available:

* **Cluster=** Only add this account to these clusters. The account is added to all defined clusters by default.
* **Description=** Description of the account. (Default is account name)
* **Name=** Name of account. Note the name must be unique and can not represent different bank accounts at different points in the account hierarchy
* **Organization=** Organization of the account. (Default is parent account unless parent account is root then organization is set to the account name.)
* **Parent=** Make this account a child of this other account (already added).

Create users
------------

Create a Slurm_ user named xxx with a specific default account (**required**) yyy::

  sacctmgr create user name=xxx DefaultAccount=yyy

If desired users may also be **added to additional accounts** (see accounting_), for example::

  sacctmgr add user xxx Account=zzzz

The fairshare and other settings for the non-default account may be configured::

  sacctmgr modify user where name=xxx account=zzzz set fairshare=0

A non-default account name may be specified in the user's batch jobs, for example with sbatch_::

  sbatch -A <account> or --account=<account>

List users by::

  sacctmgr show user
  sacctmgr show user -s
  sacctmgr show account -s xxx

When either adding or modifying a user, the following sacctmgr_ options are available:

* **Account=** Account(s) to add user to (see also ``DefaultAccount``).
* **AdminLevel=** This field is used to allow a user to add accounting privileges to this user. Valid options are:

  * None
  * Operator: can add, modify, and remove any database object (user, account, etc), and add other operators.
    On a SlurmDBD_ served slurmctld_ these users can:

    - View information that is blocked to regular uses by a ``PrivateData`` flag (see slurm.conf_).
    - Create/Alter/Delete Reservations

  * Admin: These users have the same level of privileges as an operator in the database.
    They can also alter anything on a served slurmctld_ as if they were the slurm user or root.

* **Cluster=** Only add to accounts on these clusters (default is all clusters)
* **DefaultAccount=** Default account for the user, used when no account is specified when a job is submitted. (**Required on creation**)
* **DefaultWCKey=** Default WCkey_ for the user, used when no WCkey_ is specified when a job is submitted. (Only used when tracking WCkey_.)
* **Name=** User name
* **Partition=** Name of Slurm partition this association applies to.

Modifying user UIDs or group GIDs
.................................

If a user's UID is modified in ``/etc/passwd``, 
or the user's GID is modified in ``/etc/passwd``, 
the Slurm_ design requires you to restart slurmctld_, see bug_3575_.

.. _bug_3575: https://bugs.schedmd.com/show_bug.cgi?id=3575

User fairshare
--------------

By default users are created with a **raw fairshare** value of 1, see the sshare_ command::

  sshare -l -a

.. _sshare: https://slurm.schedmd.com/sshare.html

One may alternatively create or modify users with a non-default fairshare value, see Resource_Limits_ or sacctmgr_:

* **Fairshare=** Integer value used for determining priority.
  Essentially this is the amount of claim this association and it's children have to the above system.
  Can also be the string **parent**, when used on a user this means that the parent association is used for fairshare.
  If Fairshare=parent is set on an account, that account's children will be effectively re-parented for fairshare calculations to the first parent of their parent that is not Fairshare=parent.
  Limits remain the same, only it's fairshare value is affected. 

**NOTE:** The Fairshare=parent definition is inconsistent, since the Multifactor_Priority_Plugin_ says something different:

* If all users in an account is configured with **FairShare=parent** the result is that all the jobs drawing from that account will get the same fairshare priority, based on the accounts total usage.
  **No additional fairness is added based on users individual usage.**

.. _Multifactor_Priority_Plugin: https://slurm.schedmd.com/priority_multifactor.html

For example::

    sacctmgr create user name=xxx DefaultAccount=yyy fairshare=2

Use one of these commands to print the fairshare number of user xxx::

  sacctmgr -nP list associations user=xxx format=fairshare
  sshare -lU -u xxx

Modifying and removing entities
-------------------------------

For example, to permit user xxx to execute jobs on all clusters with a default account of *fysik* execute::

  sacctmgr add user xxx DefaultAccount=fysik

You can modify the database items using SQL-like  *where* and *set*, for example::

  sacctmgr modify account where cluster=niflheim name=fysik set Description="DTU Physics"

The following has been copied from the accounting_ page:

When modifying entities, you can specify many different options in SQL-like fashion, using key words like where and set. 
A typical execute line has the following form::

  sacctmgr modify <entity> set <options> where <options>

For example::

  sacctmgr modify user set default=none where default=test

will change all users with a default account of "test" to account "none". 
Once an entity has been added, modified or removed, the change is sent to the appropriate Slurm daemons and will be available for use instantly.

Removing entities using an execute line similar to the modify example above, but without the set options. 
For example, remove all users with a default account "test" using the following execute line::

  sacctmgr remove user where default=test

To remove a user from an account::

  sacctmgr remove user brian where account=physics

Note: In most cases, removed entities are preserved, but flagged as deleted. If an entity has existed for less than 1 day, the entity will be removed completely. This is meant to clean up after typographic errors.

Resource Limits
---------------

To enable any limit enforcement you must at least have::

  AccountingStorageEnforce=limits

in your slurm.conf_, otherwise, even if you have limits set, they will not be enforced. 
Other options for *AccountingStorageEnforce* and the explanation for each are found on the Resource_Limits_ document.

Now you can impose user limits, for example::

  sacctmgr modify user xxx set GrpTRES=cpu=1000 GrpTRESRunMin=cpu=2000000

Notice: The keyword ``cpu`` **must** be in lower-case, see bug_4226_ (resolved in Slurm_ 17.02.8).

.. _Resource_Limits: https://slurm.schedmd.com/resource_limits.html
.. _bug_4226: https://bugs.schedmd.com/show_bug.cgi?id=4226

Trackable RESources (TRES)
--------------------------

A TRES_ is a resource that can be tracked for usage or used to enforce limits against.

.. _TRES: https://slurm.schedmd.com/tres.html

Quality of Service (QOS)
------------------------

A Quality of Service (QOS_) configuration should have at least the default QOS_ value of **normal**.
Additional QOS'es may be defined, for example a *high* QOS_ with some priority and limits (see sacctmgr_)::

  sacctmgr add qos high priority=10 MaxTRESPerUser=cpu=256

Update the QOS_ like this::

  sacctmgr update qos high set priority=10 MaxTRESPerUser=cpu=400

The *Quality of Service (QOS) Factor* is defined in the Multifactor_Priority_Plugin_ page as::

  Each QOS can be assigned an integer priority. The larger the number, the greater the job priority will be for jobs that request this QOS. This priority value is then normalized to the highest priority of all the QOS's to become the QOS factor.

View defined QOS'es with various degrees of detail by::

  sacctmgr show qos
  sacctmgr show qos format=name
  sacctmgr --noheader show qos format=name

A user may be allowed to use a certain QOS_ like in these examples::

  sacctmgr -i modify user where name=XXXX set QOS=normal,high
  sacctmgr -i modify user where name=XXXX set QOS+=high

The user *DefaultQOS* (see sacctmgr_) may be set by::

  sacctmgr -i modify user where name=XXXX set DefaultQOS=normal

Users must submit jobs to non-default QOS_ with sbatch_ like::

  sbatch --qos=high ...

.. _QOS: https://slurm.schedmd.com/qos.html

Workload Characterization Key (WCKey)
-------------------------------------

A WCKey_ is an orthogonal way to do accounting against possibly unrelated accounts. 
This can be useful where users from different accounts are all working on the same project. 

.. _WCKey: https://slurm.schedmd.com/wckey.html

Enforce accounting
==================

The account and user associations created above only take effect after you enable::

  AccountingStorageEnforce

in your slurm.conf_. 
Options for *AccountingStorageEnforce* and the explanation for each are found in slurm.conf_ and the Resource_Limits_ document.

When *AccountingStorageEnforce* is changed, a restart of the slurmctld_ daemon is required (not just a ``scontrol reconfig``). 

Accounting information
======================

Inquire accounting information using these commands:

* sacct_ - Displays accounting data for all jobs and job steps in the Slurm job accounting log or Slurm database.
* sstat_ - Display various status information of a running job/step.
* sreport_ - Generate reports from the slurm accounting data.
* scontrol_ show assoc_mgr - displays the current contents of the slurmctld_ 's internal cache for users, associations and/or qos.


.. _sacct: https://slurm.schedmd.com/sacct.html
.. _sstat: https://slurm.schedmd.com/sstat.html
.. _sreport: https://slurm.schedmd.com/sreport.html
.. _scontrol: https://slurm.schedmd.com/scontrol.html

Useful commands
---------------

See the sacctmgr_ man page `ENTITIES <https://slurm.schedmd.com/sacctmgr.html#lbAH>`_ section.

* Show the current accounting configuration by::

    sacctmgr show configuration

* Show the Slurm database statistics::

    sacctmgr show stats

* Show the Slurm entity (e.g., accounts) problems::

    sacctmgr show problem

* Report current jobs that have been orphanded on the local cluster and are now runaway::

    sacctmgr show RunawayJobs

* List of database transactions that have occurred during a given time period::

    sacctmgr show transaction

* Show user fairshare etc. information::

    sacctmgr show associations format=account,user,fairshare,GrpTRES,GrpTRESRunMin

  and as a tree::

    sacctmgr show associations tree

Parseable output
................

The commands sacctmgr_, sacct_, and sreport_ will truncate long columns in the printed output.
For usage in scripts one should use the **parseable** output flags -p or -P:

* -p, --parsable 
  Output will be '|' delimited with a '|' at the end.

* -P, --parsable2
  Output will be '|' delimited without a '|' at the end.

For the sacct_ command, the default delimiter '|' can be changed with this option:

* --delimiter=characters
  ASCII  characters used to separate the fields when specifying the -p or -P options. The default delimiter is  a  '|'.
  This option is ignored if -p or -P options are not specified.

To display nicely formatted columns this example may be useful::

  sacct --parsable2 | column -s '|' -t | less -S

Slurm job efficiency report (seff)
----------------------------------

The ``/usr/bin/seff`` command takes a jobid and reports on the efficiency of that job's cpu and memory utilization (requires Slurm_ 15.08 or later).
The ``slurm-contribs`` RPM (Slurm_ 17.02 and later, previously ``slurm-seff``) also comes with an ``/usr/bin/smail`` utility that allows for Slurm end-of-job emails to include a ``seff`` report, see bug_1611_.
This allows users to become aware if they are wasting resources.

.. _bug_1611: https://bugs.schedmd.com/show_bug.cgi?id=1611

Note: You may like to copy the updated ``smail`` from https://github.com/OleHolmNielsen/Slurm_tools/tree/master/smail to add the cluster name to mail headers.
This is included in Slurm_ 17.11, but make sure to get the bugfix in 17.11.6.

The ``smail`` utility is invoked automatically to process end-of-job notifications if you add the following to slurm.conf_::

  MailProg=/usr/bin/smail

User job scripts may also use this line as the last line::

  seff $SLURM_JOBID 

GPU accounting
--------------

By default Slurm_ does not record GPU accounting information.
For accounting of GPU usage you must add to the **AccountingStorageTRES** in slurm.conf_, for example::

  AccountingStorageTRES=gres/gpu,gres/gpu:tesla

and restart slurmctld_ so that these new fields are added to the database.
After editing slurm.conf_ you must distribute this file to all nodes (possibly using Configless Slurm), and do a ``scontrol reconfig``.
See also the discussion in bug_12031_.

Then you can inquire GPU resource usage with the sacct_ **AllocTRES** output, for example for a specific jobid 12345::

  sacct -j 12345 -p -X --format=jobid,user,alloctres

.. _bug_12031: https://bugs.schedmd.com/show_bug.cgi?id=12031

Nvidia accounting
.................

For Nvidia_GPUs_ there is an accounting tool built into the nvidia-smi_ command, which gets installed along with the Nvidia_drivers_.
View command options with::

  nvidia-smi -h

View the GPU accounting command options::

  nvidia-smi --help-query-accounted-apps

To enable GPU accounting, execute this code in the system ``/etc/rc.local`` startup file::

  if test -x /usr/bin/nvidia-smi
  then
	echo Starting NVIDIA driver
	/usr/bin/nvidia-smi
	echo Start the nvidia-persistenced daemon
	/usr/bin/nvidia-persistenced --verbose
	echo Enable NVIDIA accounting
	/usr/bin/nvidia-smi --accounting-mode=1
	/usr/bin/nvidia-smi --query --display=ACCOUNTING
  fi

Now you can report GPU accounting information by, for example::

  nvidia-smi --query-accounted-apps=gpu_name,pid,time,gpu_util,mem_util,max_memory_usage --format=csv
  nvidia-smi --query --display=ACCOUNTING

.. _Nvidia_GPUs: https://en.wikipedia.org/wiki/List_of_Nvidia_graphics_processing_units
.. _nvidia-smi: https://developer.nvidia.com/nvidia-system-management-interface
.. _Nvidia_drivers: https://www.nvidia.com/en-us/drivers/unix/

Accounting reports
==================

Use sreport_ to generate reports from the slurm accounting data, for example::

  sreport cluster UserUtilizationByAccount
  sreport cluster AccountUtilizationByUser 

The accounting timings will by default be displayed in units of TRES_ Minutes.

Selection of date ranges::

  sreport ... Start=02/01 End=02/25
  sreport ... Start=`date -d "last month" +%D` End=`date -d "this month" +%D`

Change the date/time format in report header for readability (formats in "man strftime")::

  env SLURM_TIME_FORMAT="%d-%b-%Y_%R" sreport ...

Show accounting indented as a tree::

  sreport cluster AccountUtilizationByUser tree

Show top user accounting::

  sreport user top start=0101 end=0201 TopCount=50 -t hourper --tres=cpu,gpu

Specify the accounting time format (default is *Minutes*) from sreport_::

  sreport -t hourper ...

Report specified TRES_ accounting (default is *cpu*)::

  sreport --tres cpu,gpu ...

Print parseable output from sreport_::

  sreport -p ...

for further processing with scripts.

Cluster utilization report::

  sreport -t hourper cluster Utilization

Other accounting report tools
-----------------------------

* **Monthly accounting report**

  The slurmreportmonth_ tool will generate monthly accounting statistics from Slurm using the sreport command. The script calculates last month's dates by default.

  Specific accounts and start/end dates may be specified. In addition, the report may be E-mailed and/or copied to some report directory (for example, on a web server).

* **Top user and group accounting report**.

  The slurmacct_ tools is an alternative to the sreport command with some advantages over the sreport command:

  *  Partition specific accounting is possible.
  *  Average CPU count (job parallelism) is printed.
  *  Average waiting time in the queue is printed (answer to "My jobs wait for too long").
  *  User full name is printed (useful to managers).

.. _slurmreportmonth: https://github.com/OleHolmNielsen/Slurm_tools/tree/master/slurmreportmonth
.. _slurmacct: https://github.com/OleHolmNielsen/Slurm_tools/tree/master/slurmacct
