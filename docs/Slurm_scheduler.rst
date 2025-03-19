.. _Slurm_scheduler:

===================
Slurm job scheduler
===================

.. Contents::

Jump to our top-level Slurm page: :ref:`SLURM`

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
.. _scontrol: https://slurm.schedmd.com/scontrol.html

Prerequisites
=============

Before configuring the Multifactor_Priority_Plugin_ scheduler, you must first configure :ref:`Slurm_accounting`.

Scheduler configuration
=======================

The **SchedulerType** configuration parameter controls how queued jobs are executed, see the Scheduling_Configuration_Guide_.

**SchedulerType** options are sched/backfill, which performs backfill scheduling, and sched/builtin, which attempts to schedule jobs in a strict priority order within each partition/queue.

There is also a **SchedulerParameters** configuration parameter which can specify a wide range of parameters as described below. 
This first set of parameters applies to all scheduling configurations. 
See the slurm.conf_ man page for more details:

* default_queue_depth=# - Specifies the number of jobs to consider for scheduling on each event that may result in a job being scheduled. Default value is 100 jobs. Since this happens frequently, a relatively small number is generally best.
* defer - Do not attempt to schedule jobs individually at submit time. Can be useful for high-throughput computing.
* max_switch_wait=# - Specifies the maximum time a job can wait for desired number of leaf switches. Default value is 300 seconds.
* partition_job_depth=# - Specifies how many jobs are tested in any single partition, default value is 0 (no limit).
* sched_interval=# - Specifies how frequently, in seconds, the main scheduling loop will execute and test all pending jobs. The default value is 60 seconds.

Backfill scheduler
------------------

We use the backfill scheduler in slurm.conf_::

  SchedulerType=sched/backfill
  SchedulerParameters=kill_invalid_depend,defer,bf_continue

but there are some *backfill* parameters that should be considered (see slurm.conf_), for example::

  ...bf_interval=60,bf_max_job_start=20,bf_resolution=600,bf_window=11000

The importance of ``bf_window`` is explained as:

* The default value is 1440 minutes (one day).
  A value **at least as long as the highest allowed time limit** is generally advisable to prevent job starvation.
  In order to limit the amount of data managed by the backfill scheduler, if the value of ``bf_window`` is increased, then it is generally advisable to also increase ``bf_resolution``. 

So you must configure ``bf_window`` according to the longest possible ``MaxTime`` in all partitions in slurm.conf_::

  PartitionName= ... MaxTime=XXX

.. _Scheduling_Configuration_Guide: https://slurm.schedmd.com/sched_config.html

scontrol top command
--------------------

The scontrol_ **top job_list** command is documented as::

  Move the specified job IDs to the top of the queue of jobs belonging to the identical user ID, partition name, account, and QOS.
  The job_list argument is a comma separated ordered list of job IDs.
  Any job not matching all of those fields will not be effected.
  Only jobs submitted to a single partition will be effected.
  This operation changes the order of jobs by adjusting job nice values.
  The net effect on that user's throughput will be negligible to slightly negative.
  This operation is disabled by default for non-privileged (non-operator, admin, SlurmUser, or root) users.
  This operation may be enabled for non-privileged users by the system administrator by including the option "enable_user_top" in the SchedulerParameters configuration parameter. 

While scontrol_ **top job_list** may be useful for the superuser to help with user requests, it is **not recommended** to configure *SchedulerParameters=enable_user_top*.
The Slurm_ 17.11 news page (https://slurm.schedmd.com/news.html) highlights this change::

  Regular user use of "scontrol top" command is now disabled.
  Use the configuration parameter "SchedulerParameters=enable_user_top" to enable that functionality.
  The configuration parameter "SchedulerParameters=disable_user_top" will be silently ignored. 

There does not seem to be any documentation of why the scontrol_ **top job_list** is unwarranted, but we have made the following observations of a bad side effect:

* A user requests a high priority for a job, and the superuser grants a negative nice value with *scontrol update jobid=10208 nice=-10000*.
* The user can now assign a negative nice value to his *other jobs* with *scontrol top jobid=10209,10210*, thereby jumping ahead of normal jobs in the queue.


Preemption of jobs by high-priority jobs
----------------------------------------

Slurm supports job preemption_, the act of stopping one or more "low-priority" jobs to let a "high-priority" job run. 
Job preemption_ is implemented as a variation of Slurm's Gang Scheduling logic. 
When a high-priority job has been allocated resources that have already been allocated to one or more low priority jobs, the low priority job(s) are preempted. 
The low priority job(s) can resume once the high priority job completes. 
Alternately, the low priority job(s) can be requeued and started using other resources if so configured in newer versions of Slurm. 

Preemption is configured in slurm.conf_.

.. _preemption: https://slurm.schedmd.com/preempt.html

Multifactor Priority Plugin scheduler
=====================================

A sophisticated Multifactor_Priority_Plugin_ provides a very versatile facility for ordering the queue of jobs waiting to be scheduled.
See the **PriorityXXX** parameters in the slurm.conf_ file.

Multifactor configuration
-------------------------

The Fairshare is configured with **PriorityX** parameters in the **Configuration** section of the Multifactor_Priority_Plugin_ page,
also documented in the slurm.conf_ page:

* PriorityType
* PriorityDecayHalfLife
* PriorityCalcPeriod
* PriorityUsageResetPeriod
* PriorityFavorSmall
* PriorityMaxAge
* PriorityWeightAge
* PriorityWeightFairshare
* PriorityWeightJobSize
* PriorityWeightPartition
* PriorityWeightQOS
* PriorityWeightTRES 

An example slurm.conf_ fairshare configuration may be::

  PriorityType=priority/multifactor
  PriorityDecayHalfLife=7-0
  PriorityFavorSmall=NO
  PriorityMaxAge=10-0
  PriorityWeightAge=100000
  PriorityWeightFairshare=1000000
  PriorityWeightJobSize=100000
  PriorityWeightPartition=100000
  PriorityWeightQOS=100000

  PropagateResourceLimitsExcept=MEMLOCK
  PriorityFlags=ACCRUE_ALWAYS,FAIR_TREE
  AccountingStorageEnforce=associations,limits,qos,safe

PriorityWeightXXX values are all 32-bit integers.
The final Job Priority is a 32-bit integer.

**IMPORTANT:** Set PriorityWeight values high to generate wide range of job priorities.

Fairshare
---------

See the page explaining the Classic_fairshare_ algorithm.
A good page on `Fairshare and Job Accounting <https://docs.rc.fas.harvard.edu/kb/fairshare/>`_ explains many of the concepts used.

Several Fairshare Algorithms may be used:

* Fair Tree Fairshare Algorithm fair_tree_.

* Depth-Oblivious Fair-share Factor priority_multifactor3_.

.. _fair_tree: https://slurm.schedmd.com/fair_tree.html
.. _priority_multifactor3: https://slurm.schedmd.com/priority_multifactor3.html
.. _Classic_fairshare: https://slurm.schedmd.com/classic_fair_share.html
.. _Multifactor_Priority_Plugin: https://slurm.schedmd.com/priority_multifactor.html

The Fairshare algorithm is explained in the Multifactor_Priority_Plugin_ page.
Note the meaning of the special value **fairshare=parent**:

* If all users in an account are configured with::

    FairShare=parent

  the result is that all the jobs drawing from that account will get the same fairshare priority, based on the accounts total usage.
  **No additional fairness is added based on users individual usage.**

Use one of these commands to print the fairshare number of user xxx::

  sacctmgr -nP list associations user=xxx format=fairshare
  sshare -lU -u xxx

or get a complete fairshare listing with::

  sacctmgr -sor show associations format=user,account,fairshare

Quality of Service (QOS)
------------------------

One can specify a *Quality of Service* (QOS_) for each job submitted to Slurm. 
A description and example are in the QOS_ page.
Example QOS_ configurations are::

  sacctmgr modify qos normal set priority=50
  sacctmgr add qos high
  sacctmgr modify qos high set priority=100

Examples::

  sacctmgr show qos format=name,priority
  sacctmgr show qos format="name,priority,flags,usagefactor,maxtres,maxwall,maxtrespu%30"

To enforce user jobs to have a QOS_ you must (at least) have::

  AccountingStorageEnforce=qos

see the slurm.conf_ and Resource_Limits_ documents.
The ``AccountingStorageEnforce`` options include:

* ``associations`` - This will prevent users from running jobs if their association is not in the database. This option will prevent users from accessing invalid accounts.
* ``limits`` - This will enforce limits set to associations.
  By setting this option, the ``associations`` option is also set.
* ``qos`` - This will require all jobs to specify (either overtly or by default) a valid qos (*Quality of Service*).
  QOS values are defined for each association in the database.
  By setting this option, the ``associations`` option is also set. 
* ``safe`` - limits and associations will automatically be set. 

The *Quality of Service (QOS) Factor* is defined in the Multifactor_Priority_Plugin_ page as::

  Each QOS can be assigned an integer priority. The larger the number, the greater the job priority will be for jobs that request this QOS. This priority value is then normalized to the highest priority of all the QOS's to become the QOS factor.

A non-zero weight must be defined in slurm.conf_, for example::

  PriorityWeightQOS=100000

.. _QOS: https://slurm.schedmd.com/qos.html

Resource Limits
---------------

To enable any limit enforcement you must at least have::

  AccountingStorageEnforce=limits

in your slurm.conf_, otherwise, even if you have limits set, they will not be enforced. 
Other options for *AccountingStorageEnforce* and the explanation for each are found on the Resource_Limits_ document.

.. _Resource_Limits: https://slurm.schedmd.com/resource_limits.html

Limiting (throttling) jobs in the queue
---------------------------------------

It is desirable to prevent individual users from flooding the queue with jobs, in case there are idle nodes available, because it may block future jobs by other users.
Note:

* With the MAUI_ scheduler (an alternative product to Slurm_) this is called Throttling_Policies_.

With Slurm_ it appears that the only way to achieve user job throttling is the following:

* Using the *GrpTRESRunMins* parameter defined in the Resource_Limits_ document.  See also the TRES_ definition.

* The *GrpTRESRunMins* limits can be applied to associations (accounts or users) as well as QOS_.
  Set the limit by::

    sacctmgr modify association where name=XXX set GrpTRESRunMin=cpu=1000000   # For an account/user asociation
    sacctmgr modify qos where name=some_QOS set GrpTRESRunMin=cpu=1000000      # For a QOS
    sacctmgr modify qos where name=some_QOS set MaxTRESPU=cpu=1000             # QOS Max TRES per user

.. _MAUI: https://docs.adaptivecomputing.com/maui/
.. _Throttling_Policies: https://docs.adaptivecomputing.com/maui/throttling306.php
.. _TRES: https://slurm.schedmd.com/tres.html

Partition factor priority
-------------------------

If some partition XXX (for example big memory nodes) should have a higher priority, this is explained in Multifactor_Priority_Plugin_ by::

  (PriorityWeightPartition) * (partition_factor) +

The *Partition factor* is controlled in slurm.conf_, for example::

  PartitionName=XXX ... PriorityJobFactor=10
  PriorityWeightPartition=1000

Scheduling commands
===================

View scheduling information for the Multifactor_Priority_Plugin_ by the commands:

* sprio_ - view the factors that comprise a job's scheduling priority::

    sprio     # List job priorities
    sprio -l  # List job priorities including username etc.
    sprio -w  # List weight factors used by the multifactor scheduler


* sshare_ - Tool for listing the shares of associations to a cluster::

    sshare
    sshare -l    # Long listing with additional information
    sshare -a    # Listing with also user information

* sdiag_ - Scheduling diagnostic tool for Slurm

.. _sprio: https://slurm.schedmd.com/sprio.html
.. _sshare: https://slurm.schedmd.com/sshare.html
.. _sdiag: https://slurm.schedmd.com/sdiag.html
.. _sched_config: https://slurm.schedmd.com/sched_config.html
