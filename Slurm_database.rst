.. _Slurm_database:

==============
Slurm database
==============

.. Contents::

This page describes Slurm_ database configuration on RHEL 7 or 8 (and clones) servers.

Jump to our top-level Slurm page: :ref:`SLURM`

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
.. _slurmdbd: https://slurm.schedmd.com/slurmdbd.html
.. _slurmctld: https://slurm.schedmd.com/slurmctld.html
.. _slurm.conf: https://slurm.schedmd.com/slurm.conf.html
.. _slurmdbd.conf: https://slurm.schedmd.com/slurmdbd.conf.html
.. _MySQL: https://www.mysql.com/

Database documentation
======================

See the accounting_ page and the Slurm_tutorials_ with *Slurm Database Usage*.
Please note this statement::

  MySQL or MariaDB is the preferred database.
  To enable this database support one only needs to have the development package for the database they wish to use on the system.
  Slurm uses the InnoDB storage engine in MySQL to make rollback possible.
  This must be available on your MySQL installation or rollback will not work. 

.. _accounting: https://slurm.schedmd.com/accounting.html
.. _InnoDB: https://en.wikipedia.org/wiki/InnoDB

The following configuration is relevant only for the **Database node** (which may be the Head/Master node), but **not** the compute nodes.

Hardware optimization
=====================

SchedMD_ recommends to have a separate database server, if possible.
It may be on the same server as slurmctld_, but this may impact performance.

Furthermore, running the database on a separate server gives you a **strong advantage** because you can upgrade slurmdbd_ without affecting slurmctld_ and thereby the cluster operations!

You should consider optimizing the database performance 
by mounting the MariaDB_ (MySQL_) database directory on a dedicated high-speed file system::

  /var/lib/mysql

Whether this is required depends on the number and frequency of jobs expected.
A high-speed file system could be placed on a separate SSD SAS/SATA disk drive,
or even better on a PCIe_ NVMe_ disk drive.

Such disks must be qualified for high-volume random small read/write operations relevant for databases,
and should be built with the *Non-Volatile Memory Express* (NVMe_) storage interface standard for reliability and performance.
It seems that NVMe_ support was added to Linux kernel 3.3 (and later), so it should work with RHEL7/CentOS7.
A new scalable block layer for high-performance SSD storage was added in kernel 3.13.

A disk size of 200 GB or 400 GB should be sufficient.
Consider installing 2 disk drives and run them in a RAID-1 mirrored configuration.
Example hardware could be the Intel *SSD P3700 series* or the Kingston *E1000 series*.

.. _PCIe: https://en.wikipedia.org/wiki/PCI_Express
.. _NVMe: https://en.wikipedia.org/wiki/NVM_Express

Install slurmdbd package
========================

Install the slurm database RPM on the database-only (slurmdbd service) node::

  export VER=22.05.8-1  # Use the latest version
  yum install slurm-$VER*rpm slurm-devel-$VER*rpm slurm-slurmdbd-$VER*rpm

Explicitly enable the service::

  systemctl enable slurmdbd

Set up MariaDB database
=======================

The accounting_ page has a section named *MySQL Configuration* which should be studied first.
Please note:

* CentOS7/RHEL7 has replaced MySQL_ by the MariaDB_ database version 5.5.
* CentOS8/RHEL8 contains the MariaDB_ database version 10.3.

.. _MariaDB: https://mariadb.org/

Make sure the MariaDB_ packages were installed **before** you built the Slurm_ RPMs::

  rpm -q mariadb-server mariadb-devel
  rpm -ql slurm-slurmdbd | grep accounting_storage_mysql.so     # Must show location of this file

Otherwise you must install MariaDB_ packages::

  yum install mariadb-server mariadb-devel

and rebuild all RPMs with mysql support as shown in :ref:`Slurm_installation`::

  rpmbuild -ta slurm-$VER.tar.bz2 --with mysql

If you will use Ansible_ to manage the database, Ansible_ needs this Python package::

  yum install MySQL-python    # CentOS7/RHEL7
  dnf install python3-mysql   # CentOS8/RHEL8

.. _Ansible: https://www.ansible.com/
 
Now start the MariaDB_ service::

  systemctl start mariadb
  systemctl enable mariadb
  systemctl status mariadb

Make sure to configure the MariaDB_ database's **root password** as instructed at first invocation of the *mariadb* service, or run this command::

  /usr/bin/mysql_secure_installation

Select a suitable *slurm* user's **database password**.
Now follow the accounting_ page instructions (using -p to enter the database password)::

  # mysql -p
  grant all on slurm_acct_db.* TO 'slurm'@'localhost' identified by 'some_pass' with grant option;  ### WARNING: change the some_pass
  SHOW VARIABLES LIKE 'have_innodb';
  create database slurm_acct_db;
  quit;

**WARNING:** Use the *slurm* database user's password **in stead of** ``some_pass``.

Optional: If you would like to grant read-only (SELECT) access to the database, set up a *readonly* user with access from '%' (meaning any host)::

  # mysql -p
  grant select on slurm_acct_db.* TO 'readonly'@'%' identified by 'some_pass';

For remote hosts you of course have to open the database server's firewall on port 3306
as described in :ref:`firewall-between-slurmctld-and-slurmdbd`.

You can verify the database grants for the *slurm* user::

  # mysql -p -u slurm
  show grants;
  quit;

Regarding InnoDB_, by default, MariaDB_ uses the XtraDB_ storage engine, a performance enhanced fork of the InnoDB_ storage engine.

.. _XtraDB: https://mariadb.com/kb/en/mariadb/about-xtradb/

This will grant user 'slurm' access to do what it needs to do on the local host or the storage host system. 
This must be done before the slurmdbd_ will work properly. After you grant permission to the user 'slurm' in mysql then you can start slurmdbd_ and the other Slurm daemons. 
You start slurmdbd_ by typing its pathname '/usr/sbin/slurmdbd' or '/etc/init.d/slurmdbd start'. 
You can verify that slurmdbd_ is running by typing ``ps aux | grep slurmdbd``.

If the slurmdbd_ is not running you can use the -v option when you start slurmdbd_ to get more detailed information. 
Starting the slurmdbd_ in daemon mode with the ``-D -vvv`` option can also help in debugging so you don't have to go to the log to find the problem.

MySQL configuration
-------------------

In the accounting_ page section *Slurm Accounting Configuration Before Build* some advice about MySQL_ configuration is given:

* NOTE: Before running the slurmdbd_ for the first time, review the current setting for MySQL's innodb_buffer_pool_size_.
  Consider setting this value large enough to handle the size of the database.
  This helps when converting large tables over to the new database schema and when purging old records.
  Setting ``innodb_lock_wait_timeout`` and ``innodb_log_file_size`` to larger values than the default is also recommended.

The following is recommended for ``/etc/my.cnf``, but on CentOS 7 you should create a new file ``/etc/my.cnf.d/innodb.cnf`` containing::

  [mysqld]
  innodb_buffer_pool_size=1024M
  innodb_log_file_size=64M
  innodb_lock_wait_timeout=900

The innodb_buffer_pool_size_ might be even larger, like 50%-80% of the server's RAM size.

To implement this change you have to shut down the database and move/remove logfiles::

  systemctl stop mariadb
  mv /var/lib/mysql/ib_logfile? /tmp/
  systemctl start mariadb

You can check the current setting in MySQL like so::

  # mysql -p
  SHOW VARIABLES LIKE 'innodb_buffer_pool_size';
  SHOW VARIABLES LIKE 'innodb_log_file_size';
  SHOW VARIABLES LIKE 'innodb_lock_wait_timeout';
  quit;

See also Bug_2457_:

* The innodb_buffer_pool_size_ can have a huge impact - we'd recommend setting this as high as half the RAM available on the slurmdbd server.

.. _Bug_2457: https://bugs.schedmd.com/show_bug.cgi?id=2457
.. _innodb_buffer_pool_size: https://mariadb.com/kb/en/innodb-buffer-pool/

Slurm database tables
=====================

To view the status of the tables in the *slurm_acct_db* database::

  # mysqlshow -p --status slurm_acct_db

It is possible to display the contents of the *slurm_acct_db* database::

   # mysql -p -u slurm slurm_acct_db
   Enter password: 

To show tables in the database::

  MariaDB [slurm_acct_db]> show tables;
  +----------------------------------+
  | Tables_in_slurm_acct_db |
  +----------------------------------+
  | acct_coord_table |
  | acct_table |
  | clus_res_table |
  | cluster_table |
  | convert_version_table |
  | federation_table |
  | niflheim_assoc_table |
  | niflheim_assoc_usage_day_table |
  | niflheim_assoc_usage_hour_table |
  | niflheim_assoc_usage_month_table |
  | niflheim_event_table |
  | niflheim_job_table |
  | niflheim_last_ran_table |
  | niflheim_resv_table |
  | niflheim_step_table |
  | niflheim_suspend_table |
  | niflheim_usage_day_table |
  | niflheim_usage_hour_table |
  | niflheim_usage_month_table |
  | niflheim_wckey_table |
  | niflheim_wckey_usage_day_table |
  | niflheim_wckey_usage_hour_table |
  | niflheim_wckey_usage_month_table |
  | qos_table |
  | res_table |
  | table_defs_table |
  | tres_table |
  | txn_table |
  | user_table |
  +----------------------------------+

where *niflheim* refers to the name of our cluster.

The contents of a table can be described like in this example::

  MariaDB [slurm_acct_db]> describe user_table;
  +---------------+---------------------+------+-----+---------+-------+
  | Field | Type | Null | Key | Default | Extra |
  +---------------+---------------------+------+-----+---------+-------+
  | creation_time | bigint(20) unsigned | NO | | NULL | |
  | mod_time | bigint(20) unsigned | NO | | 0 | |
  | deleted | tinyint(4) | YES | | 0 | |
  | name | tinytext | NO | PRI | NULL | |
  | admin_level | smallint(6) | NO | | 1 | |
  +---------------+---------------------+------+-----+---------+-------+

An element in the user_table can be printed::

  MariaDB [slurm_acct_db]> select * from user_table where name="xxx";
  +---------------+------------+---------+------+-------------+
  | creation_time | mod_time | deleted | name | admin_level |
  +---------------+------------+---------+------+-------------+
  | 1477321893 | 1477321893 | 0 | xxx | 1 |
  +---------------+------------+---------+------+-------------+


SlurmDBD Configuration
======================

While the slurmdbd_ will work with a flat text file for recording job completions and such this configuration will not allow "associations" between a user and account. 
A database allows such a configuration.

MySQL_ or MariaDB_ is the preferred database. 
To enable this database support one only needs to have the development package for the database they wish to use on the system. 
Slurm uses the InnoDB_ storage engine in MySQL_ to make rollback possible. 
This must be available on your MySQL_ installation or rollback will not work. 

slurmdbd_ requires its own configuration file called slurmdbd.conf_. 
Start by copying the example file from the *slurmdbd.conf* man-page.

The file ``/etc/slurm/slurmdbd.conf`` should be only on the computer where slurmdbd_ executes and should only be readable by the user which executes slurmdbd_ (e.g. "slurm"). 
It must be protected from unauthorized access since it contains a database login name and password::
See the slurmdbd.conf_ man-page for a more complete description of the configuration parameters. 

**NOTICE:** The ``/etc/slurm/slurm.conf`` file is **not needed or used** in the slurmdbd_ server.
The only file required is ``/etc/slurm/slurmdbd.conf``.
However, user commands such as sinfo_ and sacctmgr_ will need access to slurm.conf_,
and if ``/etc/slurm/slurm.conf`` does not exist, then they will use the configless_ mode (if configured).

Set up files and permissions::

  chown slurm: /etc/slurm/slurmdbd.conf
  chmod 600 /etc/slurm/slurmdbd.conf
  mkdir /var/log/slurm
  touch /var/log/slurm/slurmdbd.log
  chown slurm: /var/log/slurm/slurmdbd.log

Configure some of the slurmdbd.conf_ variables::

  LogFile=/var/log/slurm/slurmdbd.log
  DbdHost=XXXX    # Replace by the slurmdbd server hostname (for example, slurmdbd.my.domain)
  DbdPort=6819    # The default value
  SlurmUser=slurm
  StorageHost=localhost
  StoragePass=some_pass    # The above defined database password, change it for your site!
  StorageLoc=slurm_acct_db

.. _configless: https://slurm.schedmd.com/configless_slurm.html

Setting database purge parameters
---------------------------------

A database with very many job records (maybe of the order of a million) is causing widespread problems when **upgrading** Slurm_ and the database.
See the mailing list thread 
`[slurm-users] Extreme long db upgrade 16.05.6 -> 17.11.3 <https://lists.schedmd.com/pipermail/slurm-users/2018-February/000612.html>`_.

In order to solve this problem, it seems necessary to **purge job records** from the Slurm_ database.
In slurmdbd.conf_ you may define a number of purge parameters such as::

  PurgeEventAfter
  PurgeJobAfter
  PurgeResvAfter
  PurgeStepAfter
  PurgeUsageAfter

The values of these parameters depend on the number of jobs in the database, which differs a lot between sites.
There does not seem to be any heuristics for determining good values, so some testing will be required.

From the high_throughput_ page:
You might also consider setting the *Purge* options in your slurmdbd.conf_ to clear out old Data. 
A typical configuration might look like this::

  PurgeEventAfter=12months
  PurgeJobAfter=12months
  PurgeResvAfter=2months
  PurgeStepAfter=2months
  PurgeSuspendAfter=1month
  PurgeTXNAfter=12months
  PurgeUsageAfter=12months

The purge operation is done at the start of each time interval (see bug_4295_), which means on the 1st day of the month in this example.
Monthly, daily or even hourly purge operations would occur when using different time units for the same interval::

  PurgeStepAfter=2months
  PurgeStepAfter=60days
  PurgeStepAfter=1440hours

A monthly purge operation can be a huge amount of work for a database depending on its size, and you certainly want to cut down the amount of work required during the purges.
If you did not use purges before, it is probably a good idea to try out a series of daily purges starting with::

  PurgeEventAfter=2000days
  PurgeJobAfter=2000days
  PurgeResvAfter=2000days
  PurgeStepAfter=2000days
  PurgeSuspendAfter=2000days

If this works well over a few days, decrease the purge interval 2000days little by little and try again (1800, 1500, etc) until you after many iterations come down to the desired final purge intervals. 

Logging of purge events can be configured in slurmdbd.conf_ using::

  DebugLevel=verbose
  DebugFlags=DB_ARCHIVE


.. _high_throughput: https://slurm.schedmd.com/high_throughput.html
.. _bug_4295: https://bugs.schedmd.com/show_bug.cgi?id=4295



.. _systemd: https://en.wikipedia.org/wiki/Systemd
.. _MUNGE: https://dun.github.io/munge/

slurmdbd hostname configuration
-------------------------------

The slurmdbd_ hostname **must** be configured correctly.
The default value may be *localhost*, meaning that **no other hosts** can inquire the slurmdbd_ service (you may or may not want this limitation).

We recommend to explicitly set the slurmdbd_ hostname (for example, ``slurmdbd.my.domain``) in these files:

* On the slurmdbd_ server configure ``DbdHost`` in slurmdbd.conf_ as documented above::

    DbdHost=slurmdbd.my.domain

* On the slurmctld_ server configure ``AccountingStorageHost`` in slurm.conf_ so that slurmctld_ knows the slurmdbd_ server's hostname::

    AccountingStorageHost=slurmdbd.my.domain

After restarting the slurmctld_ and slurmdbd_ services, verify the setup by::

  scontrol show config | grep AccountingStorageHost

If other nodes than the slurmdbd_ node must be able to connect to the slurmdbd_ service, you must open the firewall to specific hosts
as described in :ref:`firewall-between-slurmctld-and-slurmdbd`.
 
Setting MaxQueryTimeRange
-------------------------

It may be a good idea to limit normal users from inquiring the database for too long periods of time.
The slurmdbd.conf_ parameter is used for this, for example for a maximum of 60 days::

  MaxQueryTimeRange=60-0

Start the slurmdbd service
==========================

First try to run *slurmdbd* manually to see the log::

  slurmdbd -D -vvv

Terminate the process by Control-C when the testing is OK.

Start the slurmdbd_ service::

  systemctl enable slurmdbd
  systemctl start slurmdbd
  systemctl status slurmdbd

If you get this error in ``/var/log/slurm/slurmdbd.log``::

  error: Couldn't find the specified plugin name for accounting_storage/mysql looking at all files

then the file ``/usr/lib64/slurm/accounting_storage_mysql.so`` is missing because you forgot to install the *mariadb-devel* RPM before building Slurm_ RPMs.
You must install the *mariadb-devel* RPM and rebuild and reinstall Slurm_ RPMs as shown above.

.. _backup-and-restore-of-database:

Backup and restore of database
==============================

In order to backup the entire database to a different location (for disaster recovery or migration), the Slurm_ database must be backed up regularly.

You may want to read the page `Mysqldump with Modern MySQL <https://serversforhackers.com/c/mysqldump-with-modern-mysql>`_.

See also the discussion in Bug_10295_ about dumping **only** the *slurm_acct_db* database, and using ``--single-transaction`` for InnoDB_ tables.
You can show the tables by::

  # mysqlshow -p --status slurm_acct_db

For compressing the (large) database dumps, install the *gzip*, *bzip2*, and perhaps the *lbzip2* package::

  yum install gzip bzip2
  yum install lbzip2    # From EPEL

.. _Bug_10295: https://bugs.schedmd.com/show_bug.cgi?id=10295
.. _mysqldump: https://mariadb.com/kb/en/mysqldump/

Backup script with crontab
--------------------------

Make a *slurm_acct_db* database using mysqldump_, for example, 
using this example script ``/root/mysqlbackup``
(**Note:** Insert the correct **root DATABASE-PASSWORD** in ``PWD``)::

   #!/bin/sh
   # MySQL Backup Script for slurm_acct_db database
   HOST=localhost
   BACKUPFILE=/root/mysql_dump.gz
   USER=root
   PWD='DATABASE-PASSWORD'
   DUMP_ARGS="--single-transaction"
   DATABASES="-B slurm_acct_db"
   /usr/bin/mysqldump --host=$HOST --user=$USER --password=$PWD $DUMP_ARGS $DATABASES | gzip > $BACKUPFILE

Write permission to $BACKUPFILE is required.
The script is also available in https://github.com/OleHolmNielsen/Slurm_tools/blob/master/database/.

Make regular database dumps, for example by a *crontab* job::

  # MySQL database backup
  30 7 * * * /root/mysqlbackup

Backup script with logrotate
----------------------------

It is preferable to make daily database backup dumps and keep a number of backup versions.
The logrotate_ tool is ideal for this purpose.

This logrotate_ file ``/etc/logrotate.d/slurm_acct_db_backup`` will make 8 backup versions in ``/var/log/mariadb/`` (insert the correct **root DATABASE-PASSWORD**)::

  /var/log/mariadb/slurm_acct_db_backup.bz2 {
  daily
  dateext
  dateyesterday
  rotate 8
  nocompress
  create 640 root adm
  postrotate
  # Dump ONLY the Slurm database slurm_acct_db
  # Strongly recommended: --single-transaction
  # Use bzip2 for compression.
  # Alternatively use lbzip2 from the EPEL repo: lbzip2 --force -n 5
  /usr/bin/mysqldump --user=root --password=<DATABASE-PASSWORD> --single-transaction -B slurm_acct_db | bzip2 > /var/log/mariadb/slurm_acct_db_backup.bz2
  endscript
  }

You must first create an empty backup file::

  touch /var/log/mariadb/slurm_acct_db_backup.bz2 

The script is also available in https://github.com/OleHolmNielsen/Slurm_tools/tree/master/database.

Using ``mysqldump --single-transaction`` is recommended by SchedMD to avoid race conditions when slurmdbd is being run while taking the MySQL dump, see
https://bugs.schedmd.com/show_bug.cgi?id=10295#c18

Testing the script::

  logrotate -dv /etc/logrotate.d/slurm_acct_db_backup

**Note:** SELinux_ enforces that logrotate_ only create files in the ``/var/log/`` folder and below.
If logrotate_ tries to create files in other locations it will get *permission denied* errors,
and errors will be present in ``/var/log/audit/audit.log``.
See the logrotate_selinux_ manual page and this Red Hat solution: https://access.redhat.com/solutions/39006

.. _logrotate: https://linux.die.net/man/8/logrotate
.. _SELinux: https://en.wikipedia.org/wiki/Security-Enhanced_Linux
.. _logrotate_selinux: https://linux.die.net/man/8/logrotate_selinux

Backup of MariaDB 10.1 and later
--------------------------------

In MariaDB_ 10.1 and later, Mariabackup_ is the recommended backup method to use instead of Percona XtraBackup, see `this page <https://mariadb.com/kb/en/backing-up-and-restoring-databases-percona-xtrabackup/>`_.

The mysqldump_for_MariaDB_ utility still exists for MariaDB_.

.. _Mariabackup: https://mariadb.com/kb/en/mariabackup/
.. _mysqldump_for_MariaDB: https://mariadb.com/kb/en/mysqldump/

Restore of a database backup
----------------------------

The database contents must be loaded from the backup.
To restore a MySQL_ database see for example
`How do I restore a MySQL .dump file? <https://stackoverflow.com/questions/105776/how-do-i-restore-a-mysql-dump-file>`_.

As user *root* read in the above created backup file::

  mysql -u root -p < /root/mysql_dump

or if the dump file has been compressed::

  zcat  /root/mysql_dump.gz  | mysql -u root -p
  bzcat /root/mysql_dump.bz2 | mysql -u root -p     # For bzip2 compressed files

The MariaDB_/MySQL_ *password* will be asked for.
Reading in the database dump may take **many minutes** depending on the size of the dump file, the storage system speed, and the CPU performance.

Upgrade of MySQL/MariaDB
------------------------

If you restore a database dump onto a different server running a **newer MySQL or MariaDB version** 
there are some extra steps:

* Consult the Upgrading_MariaDB_ page with detailed instructions for upgrading between MariaDB_ versions or from MySQL_.

You should run the mysql_upgrade_ command whenever **major (or even minor) version upgrades** are made::

  mysql_upgrade -p

It may be necessary to force an upgrade if you have restored a database dump made on an earlier version of MariaDB_,
say, when migrating from CentOS7/RHEL7 to CentOS8/RHEL8::

  mysql_upgrade -p --force

It may be necessary to restart the *mysqld* service or reboot the server after this upgrade (??).

When migrating a database from CentOS/RHEL 7 (EL7) to RHEL 8 (and EL8 clones) you should read 
`Upgrading from MariaDB 5.5 to MariaDB 10.0 <https://mariadb.com/kb/en/upgrading-from-mariadb-55-to-mariadb-100/>`_
since there are some incompatible changes between 5.5 and 10.

.. _mysql_upgrade: https://mariadb.com/kb/en/library/mysql_upgrade/
.. _Upgrading_MariaDB: https://mariadb.com/kb/en/upgrading/

.. _MariaDB_10.2.1_modifications:

Slurm database modifications required for MariaDB 10.2.1 and above
..................................................................

In MariaDB_ 10.2.1 and above there are some **important changes** which have been discussed in bug_13562_.
Several Slurm database tables must be altered while the slurmdbd_ is stopped.
Please note that EL7 contains MariaDB_ version 5.5, and EL8 contains MariaDB_ version 10.3,
so this point is **important**, for example, when upgrading from EL7 to EL8!
This has been resolved from Slurm 22.05.7.

We have discussed the procedure for MariaDB_ 10.2.1 and above in details in bug_15168_.
A future version of Slurm may perform these changes automatically.

The procedure for Slurm database modifications must be followed
when MariaDB_ has been upgraded from **older versions than 10.2.1** to 10.2.1 or newer,
or when a Slurm database has been restored from a dump from an older MariaDB_ version.

The following steps should be made:

1. Make sure slurmdbd_ is stopped::

     systemctl stop slurmdbd

2. Configure MariaDB 10.3 for Slurm in the usual way (see above in the present page).

3. Login to the *slurm_acct_db* database::

     # mysql -p -u slurm slurm_acct_db
     Enter password: 

4. Drop (delete) the following database table and show tables in the database::

     drop table table_defs_table;
     show tables;
     quit;

5. Set a high debug level in slurmdbd.conf_::

     DebugLevel=debug4

6. Start the slurmdbd_ service and look for a number of table creation lines in ``/var/log/slurm/slurmdbd.log`` such as::

     debug4: Table "niflheim_assoc_table" doesn't exist, adding

   You can also repeat the ``show tables;`` command from above to verify that ``table_defs_table`` exists once again.

7. At the end, set a normal debug level in slurmdbd.conf_::

     DebugLevel=verbose

   and restart slurmdbd_::

     systemctl start slurmdbd

.. _bug_13562: https://bugs.schedmd.com/show_bug.cgi?id=13562#c21
.. _bug_15168: https://bugs.schedmd.com/show_bug.cgi?id=15168

Configure database accounting in slurm.conf
===========================================

Finally, when you have made sure that the slurmdbd_ service is working correctly, you must configure slurm.conf_ to use slurmdbd_.

In slurm.conf_ you must configure accounting so that the database will be used through the slurmdbd_ database daemon::

  AccountingStorageType=accounting_storage/slurmdbd

Migrate the slurmdbd service to another server
==============================================

It is recommended to run the slurmdbd_ database server on a separate host from the slurmctld_'s server, see documents in Slurm_publications_:

* *Technical: Field Notes From the Frontlines of Slurm Support, Tim Wickberg, SchedMD* (2017) slides on *High-Availability*.
* *Technical: Field Notes Mark 2: Random Musings From Under A New Hat, Tim Wickberg, SchedMD* (2018) slides on *My Preferred Deployment Pattern*:
  
  - SchedMD recommends to run slurmctld_ and slurmdbd_ daemons on **separate servers**.

However, many sites run both services successfully on the same server.
If you decide to migrate the slurmdbd_ service to another server, here is a tested procedure which works on a running production cluster.

It is important to understand that the slurmctld_ service can run without problems even when the slurmdbd_ database is not responding,
since slurmctld_ just caches all state information in the *StateSaveLocation* directory::

  $ scontrol show config | grep StateSaveLocation
  StateSaveLocation       = /var/spool/slurmctld

Therefore we can take down slurmdbd_ for a number of minutes or hours without problems.
The outstanding messages in the *StateSaveLocation* are currently capped at *3xNodes + MaxJobCount*.

Configure a slurmdbd server
---------------------------

Install a new Slurm_ server as described in :ref:`Slurm_installation`.
You must make sure that these prerequisites are satisfied:

* The Munge_ service is running correctly.
* The user passwd database contains all Slurm_ users.

Install **the same Slurm version** on the new server as on the old server! 
This ensures that the database migration will be as fast as possible.
Any upgrading should be done at a later date according to the instructions in :ref:`upgrading-slurm`.

Make sure to open the firewall **completely** as described in :ref:`firewall-between-slurmctld-and-slurmdbd`.

Configure the MariaDB_/MySQL and the slurmdbd_ services as described above.

Testing the database restore
............................

Take a database dump file and restore it into the MariaDB_/MySQL_ database (see above :ref:`backup-and-restore-of-database`).
Use the time_command_ to get an estimate of the time this will take.

Configure the server's hostname ( for example ``db2``) in slurmdbd.conf_:

  DbdHost=<hostname>

Start the slurmdbd_ service manually to see if any errors occur::

  slurmdbd -D -vvvv

and wait for the output::

     slurmdbd: debug2: Everything rolled up

and do a *Control-C*.

.. _time_command: https://man7.org/linux/man-pages/man1/time.1.html

Database migration procedure
----------------------------

Let us denote the slurmdbd_ servers as:

* ``db1`` is the current slurmdbd_ and MariaDB_ database server.
  This could be the same as the slurmctld_ server, or it could be a dedicated server.

* ``db2`` is the designated new slurmdbd_ and MariaDB_ database server.

db1: stop slurmdbd
..................

On the ``db1`` server:

1. Stop and disable slurmdbd_ and make sure the status is down::

     systemctl disable slurmdbd
     systemctl stop slurmdbd
     systemctl status slurmdbd

2. Run the MySQL_ database dump described  above :ref:`backup-and-restore-of-database`.

   Copy the database dump to the ``db2`` server.
   Make a long-term copy of the database dump.

3. Stop any crontab jobs that run MySQL_ database dumps.

db2: restore database and start slurmdbd
........................................

On the ``db2`` server:

1. Make sure the slurmdbd_ service is stopped and that no crontab jobs will run database dumps.

2. Load the database dump from ``db1`` into MariaDB_ as shown above :ref:`backup-and-restore-of-database`.

   If the MariaDB_ version on ``db2`` than on ``db1`` then **you must remember** to perform the MariaDB_ update actions shown above.

3. Start the slurmdbd_ service manually to see if any errors occur::

     slurmdbd -D -vvvv 

   and wait for the output::
 
     slurmdbd: debug2: Everything rolled up

   and do a *Control-C*.

4. Start and enable slurmdbd_ and make sure the status is up::

     systemctl enable slurmdbd
     systemctl start slurmdbd
     systemctl status slurmdbd

Now the new slurmdbd_ service should be up and running on the ``db2`` server in a stable state.

slurmctld server: reconfigure AccountingStorageHost
...................................................

On the slurmctld_ server:

Now it's time to reconfigure slurmctld_ for the new ``db2`` slurmdbd_ server.

1. Stop the slurmctld_::

     systemctl stop slurmctld

2. Edit slurm.conf_ to configure the new slurmdbd_ server (``db2``)::

     AccountingStorageHost=db2

3. Just for safety, but not required:
   Make a backup copy of the *StateSaveLocation* ``/var/spool/slurmctld`` directory::

     tar czf $HOME/var.spool.slurmctld.tar.gz /var/spool/slurmctld

4. Start the slurmctld_::

     systemctl start slurmctld

5. Check the slurmctld_ log file, for example::

     grep slurmdbd: /var/log/slurm/slurmctld.log

6. Test that your Slurm_ cluster's functionality has now been completely restored (use squeue_, sinfo_ etc.).

.. _sinfo: https://slurm.schedmd.com/sinfo.html
.. _squeue: https://slurm.schedmd.com/squeue.html

db2: Enable database backups
............................

On the ``db2`` server:

1. Make a crontab job for doing database dumps as in :ref:`backup-and-restore-of-database`.

2. Make sure the ``db2`` server and the database dumps are backed up daily/regularly to your site's backup service.
