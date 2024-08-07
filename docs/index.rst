System technical documentation for Niflheim
#################################################

This page contains system technical documentation for Niflheim
describing the installation of Slurm batch queue system, software modules, and networking topics.
(Note: This information is probably not useful for end users of Niflheim,
please refer to the page_for_users_ in stead).

**These pages are for Slurm >= 23.02 and EL8/EL9 Linux.**
Older **archived versions** of these pages with information related to pre-23.02 Slurm and CentOS 7 Linux may be found in the archive_
or in the v24.06 release of these pages.

.. _page_for_users: https://wiki.fysik.dtu.dk/Niflheim_users/Niflheim_Getting_Started/
.. _Ansible_configuration: https://wiki.fysik.dtu.dk/ITwiki/Ansible_configuration/
.. _Hardware_documentation: https://wiki.fysik.dtu.dk/ITwiki/Hardware/
.. _archive: https://wiki.fysik.dtu.dk/Niflheim_system_archive/

.. toctree::
   :maxdepth: 1
   :caption: Slurm documentation

   Slurm batch queueing system: Overview <SLURM>
   Slurm_operations
   Slurm_installation
   Slurm_configuration
   Slurm_database
   Slurm_accounting
   Slurm_scheduler
   Slurm_cloud_bursting

.. toctree::
   :maxdepth: 1
   :caption: Software documentation

   EasyBuild_modules

* Ansible_configuration_ of Linux servers and desktops

Hardware documentation
=========================

* Hardware_documentation_ of servers and other devices

.. toctree::
   :maxdepth: 1
   :caption: Networking documentation

   OmniPath
   OmniPath_switches
   System_administration
   MultipleEthernetCards
