.. _SLURM:

Slurm batch queueing system
===========================

.. _MoinMoin: https://moinmo.in/
.. _Sphinx: https://www.sphinx-doc.org/en/master/

These pages constitute a HowTo_ guide for setting up a Slurm workload manager software installation
based on RHEL_ 8 Linux and clones such as AlmaLinux_ and RockyLinux_.
It is strongly recommended to first read the official Slurm_documentation_ pages.

**These pages are for Slurm >= 23.02 and EL8/EL9 Linux.**
Older **archived versions** of these pages with information related to pre-23.02 Slurm and CentOS 7 Linux may be found in the archive_
or in the v24.06 release of these pages.

Much of the information should be relevant on other Linux versions as well.
The present pages are **not** official documents from SchedMD_ but only a user's contribution to the Slurm community.

The information has been subdivided into sub-pages for separate Slurm topics:

* :ref:`Slurm_operations` for daily management.   
* :ref:`Slurm_installation`.
* :ref:`Slurm_configuration` setting up the services.
* :ref:`Slurm_database` for storing user and job data.
* :ref:`Slurm_accounting` defining user accounts.
* :ref:`Slurm_scheduler` for prioritizing jobs.
* :ref:`Slurm_cloud_bursting` extending the local cluster with nodes in the Cloud.
* Some Slurm tools at https://github.com/OleHolmNielsen/Slurm_tools

.. _archive: https://wiki.fysik.dtu.dk/Niflheim_system_archive/
.. _HowTo: https://www.thefreedictionary.com/Howto
.. _AlmaLinux: https://www.almalinux.org
.. _RockyLinux: https://www.rockylinux.org
.. _RHEL: https://en.wikipedia.org/wiki/Red_Hat_Enterprise_Linux

Slurm documentation
===================

Documentation from SchedMD_ about Slurm_:

* The Slurm_homepage_.
* The Slurm_documentation_ pages.
* Quick_Start_User_Guide_.
* Slurm_Quick_Start_ admin guide.
* Commercial Support by SchedMD®: https://www.schedmd.com/support.php
* Slurm_docs_.
* Command_Summary_ (2-page sheet).
* Slurm_FAQ_.
* Slurm configuration file slurm.conf_
* Slurm NEWS_ (changes) and RELEASE_NOTES_ in the current version/branch.
  Select the relevant **Branch** version in the upper left corner of the page.
* Slurm_download_ with links to external tools under *Download Related Software*.
* Slurm_git_repo_ and Slurm_git_commits_.
* Subscribe to Slurm_mailing_lists_.
* The slurm_devel_archive_.
* Slurm_publications_ and presentations.
* Citing academic work involving Slurm from JSSPP 2023: `Architecture of the Slurm Workload Manager <https://doi.org/10.1007/978-3-031-43943-8_1>`_,
  see https://slurm.schedmd.com/faq.html#cite
* Youtube `videos about Slurm <https://www.youtube.com/c/SchedMDSlurm/videos>`_.
* Slurm_tutorials_.
* Slurm_man_pages_ overview of man-pages, configuration files, and daemons.
* Slurm_support_ tracking system.
* Slurm_contributions_ of code and patches.
* `Large Cluster Administration Guide <https://slurm.schedmd.com/big_sys.html>`_ (clusters containing 1024 nodes or more).
* `Slurm Troubleshooting Guide <https://slurm.schedmd.com/troubleshoot.html>`_.
* `Slurm Elastic Computing (Cloud Bursting) <https://slurm.schedmd.com/elastic_computing.html>`_ (Google Cloud, Amazon EC2 etc.)
* `Slurm containers guide <https://slurm.schedmd.com/containers.html>`_.
* `Support for Multi-core/Multi-thread Architectures <https://slurm.schedmd.com/mc_support.html>`_.

Documentation from other sources:

* `High Performance Computing Modern: Systems and Practices. Second Edition (2025) <https://www.sciencedirect.com/book/9780128230350/high-performance-computing>`_,
  `Chapter 7 - The Essential SLURM: Resource Management <https://www.sciencedirect.com/science/article/abs/pii/B9780128230350000079>`_.
* `LLNL Slurm User Manual <https://hpc.llnl.gov/banks-jobs/running-jobs/slurm-user-manual>`_.
* `CECI Slurm FAQ <https://www.ceci-hpc.be/slurm_faq.html>`_.
* `CECI Slurm Quick Start Tutorial <https://www.ceci-hpc.be/slurm_tutorial.html>`_.
* `Comparison between SLURM and Torque (PBS) commands <https://www.sdsc.edu/~hocks/FG/PBS.slurm.html>`_ `(cached copy) <https://ftp.fysik.dtu.dk/Slurm/PBS.slurm.html>`_.
* HPC @ NIH `Building pipelines using slurm dependencies <https://hpc.nih.gov/docs/job_dependencies.html>`_.

.. _Slurm_Quick_Start: https://slurm.schedmd.com/quickstart_admin.html
.. _Quick_Start_User_Guide: https://slurm.schedmd.com/quickstart.html
.. _Slurm_homepage: https://www.schedmd.com/
.. _Slurm_documentation: https://slurm.schedmd.com/documentation.html
.. _SchedMD: https://www.schedmd.com/
.. _Slurm_docs: https://slurm.schedmd.com/
.. _Command_Summary: https://slurm.schedmd.com/pdfs/summary.pdf
.. _Slurm_FAQ: https://slurm.schedmd.com/faq.html
.. _Slurm_download: https://slurm.schedmd.com/download.html
.. _Slurm_mailing_lists: https://lists.schedmd.com/mailman3/postorius/lists/
.. _slurm_devel_archive: https://groups.google.com/forum/#!forum/slurm-devel
.. _Slurm_publications: https://www.schedmd.com/publications/
.. _Slurm_tutorials: https://slurm.schedmd.com/tutorials.html
.. _Slurm_support: https://support.schedmd.com
.. _Slurm_contributions: https://github.com/SchedMD/slurm/blob/master/CONTRIBUTING.md
.. _Slurm_man_pages: https://slurm.schedmd.com/man_index.html
.. _slurm.conf: https://slurm.schedmd.com/slurm.conf.html
.. _scontrol: https://slurm.schedmd.com/scontrol.html
.. _sacctmgr: https://slurm.schedmd.com/sacctmgr.html
.. _slurmctld: https://slurm.schedmd.com/slurmctld.html
.. _slurmdbd: https://slurm.schedmd.com/slurmdbd.html
.. _slurmd: https://slurm.schedmd.com/slurmd.html
.. _EPEL: https://fedoraproject.org/wiki/EPEL
.. _Slurm_git_repo: https://github.com/SchedMD/slurm
.. _Slurm_git_commits: https://github.com/SchedMD/slurm/commits/
.. _NEWS: https://github.com/SchedMD/slurm/blob/slurm-24.11/NEWS
.. _RELEASE_NOTES: https://github.com/SchedMD/slurm/blob/slurm-24.11/RELEASE_NOTES

