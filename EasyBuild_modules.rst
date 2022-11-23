.. _EasyBuild_modules:

==========================================
EasyBuild software for environment modules 
==========================================

On the Niflheim_ cluster (based on EL7 and EL8 Linux) we use the Lmod_ and EasyBuild_ environment modules for software packages.

.. _Niflheim: https://wiki.fysik.dtu.dk/Niflheim_Getting_started/niflheim/

.. Contents::

EasyBuild documentation
=======================

Documentation:

* EasyBuild_ homepage.
* Documentation_ page.
* Release_notes_.
* `Demos <http://easybuild.readthedocs.io/en/latest/demos/>`_.
* Paper: `Modern Scientific Software Management Using EasyBuild and Lmod <http://dl.acm.org/citation.cfm?id=2691141>`_.
* `EasyBuild mailinglist <https://lists.ugent.be/wws/info/easybuild>`_.
* Github_information_ and Integration_with_GitHub_.
* EasyBuild_ channel on Youtube_.
* News on Twitter_.
* Slack: Contact the EasyBuild community via Slack: https://easybuild.slack.com, self-request an invite via https://easybuild-slack.herokuapp.com.
* IRC: An IRC channel #easybuild has been set up on the FreeNode network.
  Just connect your IRC client to the chat.freenode.net server, and join the #easybuild channel.
* `Generating container recipes & images <http://easybuild.readthedocs.io/en/develop/Containers.html>`_.

Software provided by EasyBuild_:

* `List of supported software <http://easybuild.readthedocs.io/en/latest/version-specific/Supported_software.html>`_.

.. _EasyBuild: https://github.com/hpcugent/easybuild
.. _Documentation: https://easybuild.readthedocs.io/en/latest/
.. _Release_notes: http://easybuild.readthedocs.io/en/latest/Release_notes.html
.. _Github_information: https://easybuilders.github.io/easybuild/
.. _Integration_with_GitHub: https://easybuild.readthedocs.io/en/latest/Integration_with_GitHub.html
.. _Twitter: https://twitter.com/easy_build
.. _Youtube: https://www.youtube.com/easybuilders

Some talks about EasyBuild_:

* http://hpcugent.github.io/easybuild/files/SC15_BoF_Getting_Scientific_Software_Installed_EasyBuild_lightning_talk.pdf
* http://hpcugent.github.io/easybuild/files/SC15_BoF_Getting_Scientific_Software_Installed_Lmod_lightning_talk.pdf

Lmod
====

EasyBuild_ works with older Tcl/C based module tools, but Lmod_ is `recommended <https://easybuild.readthedocs.io/en/latest/Installation.html#dependencies>`_.
From the Lmod_ homepage:

* Lmod_ is a Lua_ based module system that easily handles the MODULEPATH Hierarchical problem.
  Environment Modules provide a convenient way to dynamically change the users' environment through modulefiles.
  This includes easily adding or removing directories to the PATH environment variable.
  Modulefiles for Library packages provide environment variables that specify where the library and header files can be found.

.. _Lmod: https://www.tacc.utexas.edu/research-development/tacc-projects/lmod
.. _Lua: https://www.lua.org/about.html

Lmod_ documentation:

* `Lmod: A New Environment Module System <http://lmod.readthedocs.org>`_.

* Lmod_User_Guide_ (PDF version lmod.pdf_)

* The `lmod-users mailing list <https://lists.sourceforge.net/lists/listinfo/lmod-users>`_.

.. _Lmod_User_Guide: http://lmod.readthedocs.io/en/latest/010_user.html
.. _lmod.pdf: https://media.readthedocs.org/pdf/lmod/latest/lmod.pdf

Install Lmod
------------

You must install Lmod_ on every node in your cluster.
It is most convenient to install an Lmod_ RPM package on all nodes.

If you don't have **root** permissions on the system, you can install Lmod_ as described in `Installing Lmod without root permissions <http://easybuild.readthedocs.io/en/latest/Installing-Lmod-without-root-permissions.html>`_.

There are no official Lmod_ RPM packages available from the authors, so for CentOS/RHEL Linux you need to install Lmod_ from the EPEL_ repository.
First you install the newest version of *epel-release* RPM for EL7, for example::

  CentOS7: yum install epel-release
  RHEL7:  yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm

On EL8 systems install EPEL_ and enable the Powertools::

  dnf config-manager --set-enabled powertools
  dnf install epel-release

.. _EPEL: https://fedoraproject.org/wiki/EPEL

To download the Lmod_ and prerequisite Lua_ packages directly (for compute nodes) get them from https://dl.fedoraproject.org/pub/epel/7/x86_64/l/.
The Lua_ packages required are::

  yum install lua-bitop lua-filesystem lua-json lua-lpeg lua-posix lua-term

Then install Lmod_ and prerequisite Lua_ packages::

  yum install Lmod

The minimum Lmod_ version is 6.5.1 as of EasyBuild_ 3.7.

Using Lmod
----------

See the document `How to Transition to Lmod (or how to test Lmod without installing it for all) <https://lmod.readthedocs.io/en/latest/045_transition.html>`_.

The Lmod_ RPM package installs several shell initialization scripts in ``/etc/profile.d/``.
For *bash* the shell initialization process involves some steps:

1. ``/etc/profile.d/z00_lmod.sh`` is called when the shell is started.
2. This initializes module support by calling the script ``/usr/share/lmod/lmod/init/sh``.
3. This defines a shell function ``module()``.
4. The shell function ``module()`` calls the main Lua_ program for Lmod_: ``/usr/share/lmod/lmod/libexec/lmod``.

Now the ``module`` "commands" (functions) can be used::

  module list
  ml

To view the ``module`` command::

  type module

To list all defined shell functions::

  compgen -A function

Lmod spider cache
-----------------

It is now very important that sites with large modulefile installations build system spider cache files. 
There is a shell script called ``update_lmod_system_cache_files`` that builds a system cache file.
See the Spider_cache_ page.

If you work on different CPU architectures, it may be convenient to turm off Lmod_'s caching feature by::

  export LMOD_IGNORE_CACHE=1

.. _Spider_cache: https://lmod.readthedocs.io/en/latest/130_spider_cache.html

Installing EasyBuild
====================

EasyBuild_ itself should be used only by a dedicated account for building software modules.

We have a created a user+group named *modules* with a home-directory on **a shared filesystem** to be mounted by NFS on the compute nodes: ``/home/opt/modules``.
For example::

    root# groupadd -g 983 modules
    root# useradd -m -c "Modules user" -d /home/opt/modules -u 983 -g modules -s /bin/bash modules

EasyBuild prerequisites
-----------------------

Prerequisite modules are listed in Dependencies_.

.. _Dependencies: https://easybuild.readthedocs.io/en/latest/Installation.html#dependencies

For RHEL/CentOS 7 these packages seem to suffice::

  yum install tar gzip bzip2 unzip xz make patch python3 python3-setuptools gcc-c++ Lmod 

Modules such as UCX require some OS dependencies::

  yum install libibverbs-devel rdma-core-devel

Old versions of EasyBuild might also require the *python-mock* RPM, see https://github.com/easybuilders/easybuild-framework/issues/2712

Some optional packages from EPEL_ may perhaps be needed, see `Dependencies <https://easybuild.readthedocs.io/en/latest/Installation.html#dependencies>`_::

  yum install GitPython pysvn graphviz 

Bootstrapping
-------------

.. _Configuration: http://easybuild.readthedocs.io/en/latest/Configuration.html

Now you should login or do::

  su - modules

to become the **non-root** user.

The steps required for a normal (**non-root**) user are:

* Read the Installation_ page, especially the *Bootstrapping procedure* section.

  You may want to use `Installing EasyBuild with EasyBuild <https://docs.easybuild.io/en/latest/Installation.html#installing-easybuild-with-easybuild>`_ to build an EasyBuild_ module.
  This is assumed in the sections below.

.. _Installation: https://easybuild.readthedocs.io/en/latest/Installation.html

* If multiple module tools are available on the system, it may be necessary to configure the use of Lmod_ (see the Configuration_ page)::

    export EASYBUILD_MODULES_TOOL=Lmod

* Define the top-level directory for your modules, for example::

    export EASYBUILD_PREFIX=/home/opt/modules

  If your environment is inhomogeneous with different OS versions and/or CPU architectures, you could create separate subdirectories for each.
  For example, you may have both CentOS 7 (el7) and CentOS 6 (el6)::

    export EASYBUILD_PREFIX=/home/opt/modules/el7/x86_64
    export EASYBUILD_PREFIX=/home/opt/modules/el6/i686

  Obviously, you would need to select somehow the appropriate top-level directory for each computer.

* If you work on a PC, it is recommended to use a $EASYBUILD_PREFIX directory on the PC's local hard disk for performance reasons.
  An SSD disk will obviously speed up the tasks.

* Update $MODULEPATH and check the basic functionality::

    module use $EASYBUILD_PREFIX/modules/all
    module list
    eb --version

* You may run some tests (which take a long time)::

    export TEST_EASYBUILD_MODULES_TOOL=Lmod
    python -m test.framework.suite

Environment variables
---------------------

All EasyBuild_ long option names can be passed as environment variables. 
Variable name is EASYBUILD_<LONGNAME> eg. --some-opt is same as setting EASYBUILD_SOME_OPT in the environment.

Examples::

  export EASYBUILD_TMPDIR=/scratch/$USER

To use the shared memory for building::

  export EASYBUILD_BUILDPATH=/dev/shm

Updating EasyBuild
------------------

If a new version of EasyBuild_ should be installed, consult the Updating_ page.

The simplest way may be the `new command <https://github.com/hpcugent/easybuild-framework/pull/1861>`_ in version 2.9.0 and later::

  eb --install-latest-eb-release


The standard upgrading method is to download the bootstrap script and execute it as in the normal installation explained above.
Then reload the *EasyBuild* module as shown above.

.. _Updating: https://easybuild.readthedocs.io/en/latest/Installation.html#updating-an-existing-easybuild-installation

Using EasyBuild for module building
===================================

The following is only for module builders!

Add the following to the normal user's ``.bashrc`` file::

  # EasyBuild setup
  export EASYBUILD_MODULES_TOOL=Lmod
  export EASYBUILD_PREFIX=/home/opt/modules   # Example directory
  module use $EASYBUILD_PREFIX/modules/all
  module load EasyBuild

**Notice:** Except for the last line, the modules environment can be set up for all users using ``/etc/profile.d/`` files as shown below.

Read the Concepts_and_Terminology_ and command_line_ pages.
See also the command help::

  eb --help

To get verbose output from the ``eb`` command set this variable::

  export EB_VERBOSE=1 

Of particular interest is:

* The toolchains_: EasyBuild_ employs so-called **compiler toolchains** or simply toolchains_ for short, which are a major concept in handling the build and installation processes.
  List available toolchains_::

     eb --list-toolchains

  See also http://easybuild.readthedocs.io/en/latest/eb_list_toolchains.html

* The easyblocks_: The implementation of a particular software build and install procedure is done in a Python module, which is aptly referred to as an **easyblock**.
  A list of easyblocks_ can be obtained with::

    eb --list-easyblocks

* Searching_for_easyconfigs_, for example::

    eb -S ^GCC-4.6

.. _command_line: https://easybuild.readthedocs.io/en/latest/Using_the_EasyBuild_command_line.html
.. _toolchains: https://easybuild.readthedocs.io/en/latest/Using_the_EasyBuild_command_line.html#list-of-known-toolchains-list-toolchains
.. _easyblocks: https://easybuild.readthedocs.io/en/latest/Using_the_EasyBuild_command_line.html#list-of-available-easyblocks-list-easyblocks
.. _Concepts_and_Terminology: https://easybuild.readthedocs.io/en/latest/Concepts_and_Terminology.html
.. _Searching_for_easyconfigs: https://easybuild.readthedocs.io/en/latest/Using_the_EasyBuild_command_line.html#searching-for-easyconfigs-search-s
.. _easyconfig: https://easybuild.readthedocs.io/en/latest/Concepts_and_Terminology.html#easyconfig-files

Building in a RAM disk
----------------------

Building may be a lot faster if the (temporary) software build directory is located in a RAM disk in stead of a hard disk or on a remote server.
Define this variable in your ``.bashrc`` file::

  export EASYBUILD_BUILDPATH=/dev/shm                  # CentOS/RHEL  
  export EASYBUILD_BUILDPATH=/dev/shm/$USER            # Debian based
  export EASYBUILD_BUILDPATH=/run/user/$UID/eb_build   # CentOS/RHEL  

Beware of the file system sizes:

* The ``/dev/shm`` defaults to 50% of the system RAM memory.  It can be changed by specifying an explicit size in the ``/etc/fstab`` file (see tmpfs filesystems in ``man 8 mount``).

* The ``/run/user/$UID`` defaults to only 10% of the system RAM memory as defined in ``/etc/systemd/logind.conf`` (see ``man 5 logind.conf``).  

You may also configure a larger directory for temporary files::

  export EASYBUILD_TMPDIR=/scratch/$USER

See the Configuration_ page.

Maybe a larger stack size is needed also::

  ulimit -s 2000240 

Global setup of modules for all users
=====================================

**Notice:** Normal users of the modules **do not** need to load the *EasyBuild* module - this is only for module builders.

If desired the system administrator can set up shell initialization scripts so that all users automatically have the EasyBuild_ modules set up,
see:

* `Providing A Standard Set Of Modules for all Users <http://lmod.readthedocs.io/en/latest/070_standard_modules.html>`_.
* Mailing list thread https://lists.ugent.be/wws/arc/easybuild/2016-10/msg00052.html

On CentOS systems the shell initialization scripts are in ``/etc/profile.d/``.
The Lmod_ RPM has installed several scripts here.
See also the Lmod_User_Guide_.

To set up the EasyBuild_ environment create in ``/etc/profile.d/`` the file :download:`z01_EasyBuild.sh <attachments/z01_EasyBuild.sh>`::

  if [ -z "$__Init_Default_Modules" ]; then
   export __Init_Default_Modules=1
   export EASYBUILD_MODULES_TOOL=Lmod
   export EASYBUILD_PREFIX=/home/modules
   module use $EASYBUILD_PREFIX/modules/all
  else
   module refresh
  fi

and for *tcsh* :download:`z01_EasyBuild.csh <attachments/z01_EasyBuild.csh>`::

  if ( ! $?__Init_Default_Modules )  then
    setenv __Init_Default_Modules 1
    setenv EASYBUILD_MODULES_TOOL Lmod
    setenv EASYBUILD_PREFIX /home/modules
    module use $EASYBUILD_PREFIX/modules/all
  else
    module refresh
  endif

Obviously, the *EASYBUILD_PREFIX* location of modules is just an example - every site will use a different location, so configure this variable accordingly.

Setting the CPU hardware architecture
-------------------------------------

By default, EasyBuild optimizes builds for the CPU architecture of the build host, by instructing the compiler to generate instructions for the highest instruction set supported by the process architecture of the build host processor.
This is done by including specific compiler flags in $CFLAGS, $CXXFLAGS, $FFLAGS, $F90FLAGS, etc.

See `Controlling compiler optimization flags <https://easybuild.readthedocs.io/en/latest/Controlling_compiler_optimization_flags.html>`_.


Some compilers will generate code for the CPU hardware on which it is executed, and this code may not run on older CPUs.
This leaves sysadmins and users with two choices:

1. Build modules on the oldest available CPU.  This should run on newer CPUs, but performance will suffer because newer hardware isn't utilized well.

2. Build separate module trees for each generation of CPUs, assuring that optimized code is generated.
   Centrally built modules can be NFS mounted so that only the CPU-specific module tree is made available.

More complicated setups are suggested in the mailing list thread https://lists.ugent.be/wws/arc/easybuild/2016-09/msg00052.html

Determining the current CPU architecture
........................................

It is surprisingly difficult to determine the CPU hardware architecture of any given system for selecting hardware-optimized modules.
A useful list of CPU-architectures is in the Safe_CFLAGS_ page.

We have found the following solutions:

* **Recommended:** Ask the GCC compiler for the native architecture, for example::

    # module load GCC
    # gcc -march=native -Q --help=target | grep march | awk '{print $2}'
    haswell

  GCC version 4.9 or newer should be used in order to reveal processor codenames, since older GCC versions will output less informative names such as *core2*.
  Intel's *Skylake* processor is only recognized by GCC version 6 or newer.

  The output may be the Intel CPU codenames such as *broadwell, haswell* etc. 
  See the CPU-specific Safe_CFLAGS_.

* Use the command ``lscpu`` to display the *Model name* (or look into ``/proc/cpuinfo``).

As a convenience to normal users, the sysadmin may provide in ``/etc/profile.d/`` the scripts ``cpu_arch.sh``::

  export CPU_ARCH="broadwell"

and ``cpu_arch.csh``::

  setenv CPU_ARCH "broadwell"

(for the example of *broadwell* CPUs) where the current host CPU-architecture has been determined by any of the above methods.
Obviously, this may have to be set differently for different types of compute nodes.

Using the ``$CPU_ARCH`` variable users can easily select the correct CPU-architecture.
For example, users may choose to select CPU-specific module trees::

  export EASYBUILD_PREFIX=$HOME/$CPU_ARCH
  module use $EASYBUILD_PREFIX/modules/all

.. _Safe_CFLAGS: https://wiki.gentoo.org/wiki/Safe_CFLAGS

Automounting the CPU architecture dependent modules directory
-------------------------------------------------------------

If the modules file tree is located on a shared NFS server, NFS clients can automount_ different module trees for different CPU architectures.

We prefer to provide a ``/home/modules`` NFS file system alongside with user home-directories such as ``/home/group1``.
This is configured in ``/etc/auto.master`` with a line::

  /home /etc/auto.home --timeout=60

The file ``/etc/auto.home`` may then contain, along with user home directories, a CPU architecture specific NFS mount for ``/home/modules``, for example::

  modules -rsize=8192,wsize=8192,tcp,vers=3 nfsserver:/u/modules/broadwell

When you have multiple CPU architectures it becomes complicated to maintain consistent ``/etc/auto.home`` automount_ files across the different architectures.
In stead you can define a CPU architecture variable in the autofs_ configuration file ``/etc/sysconfig/autofs``::

  OPTIONS="-DCPU_ARCH=broadwell"

and use that variable in ``/etc/auto.home``::

  modules -rsize=8192,wsize=8192,tcp,vers=3 nfsserver:/u/modules/$CPU_ARCH

Restart the autofs_ service::

  systemctl restart autofs

Now the ``/etc/auto.home`` file is independent of CPU architecture, since this is in stead defined in ``/etc/sysconfig/autofs``.


.. _automount: https://linuxconfig.org/how-to-configure-the-autofs-daemon-on-centos-7-rhel-7
.. _autofs: https://linuxconfig.org/how-to-configure-the-autofs-daemon-on-centos-7-rhel-7

Install common packages
=======================

See the `List of supported software <http://easybuild.readthedocs.io/en/latest/version-specific/Supported_software.html>`_.

Some examples:

* *Atomic Simulation Environment* (ASE)::

    eb -S '^ASE*'

You can do a *dry-run* overview (typically combined with --robot, in the form of -Dr) using one of these flags:

* eb --dry-run: Print build overview incl. dependencies (full paths) (def False)
* eb -D, --dry-run-short: Print build overview incl. dependencies (short paths) (def False)
* eb -x, --extended-dry-run: Print build environment and (expected) build procedure that will be performed (def False)

Notes:

* The ASE module requires the *openssl-devel* and *libibverbs-devel* (Infiniband) RPMs (to be installed by the *root* user)::

     root# yum install openssl-devel libibverbs-devel libX11-devel

* If you build the Tk package, there is a TK_bug_ requiring you to preinstall the *libX11-devel* library::

     root# yum install libX11-devel

.. _Tk_bug: https://github.com/hpcugent/easybuild-easyconfigs/issues/2261

Uninstall a module
==================

There is no automatic way to uninstall a module.
Please see the discussion of `Uninstall software <https://github.com/hpcugent/easybuild-framework/issues/590>`_.
The reason is that if you remove some modules, there is (currently) no way to find out if other modules depend upon it.

The unsafe way to remove a module may be to locate the module file in your $MODULEPATH.
Examine the module's *root* directory and remove the files belonging to the module.
Finally remove the module file itself.

Probably the best approach to renew your module list is to **generate a complete module tree from scratch**.
In this way you can select a new and smaller set of modules to build.

Marking a module as the Default version
=======================================

When installing a new version of a module, it may be necessary to mark the previous module as the default module during a testing period.

This is achieved with Lmod_ by as described in https://lmod.readthedocs.io/en/latest/060_locating.html#marking-a-version-as-default by going to the module directory and creating a soft-link named ``default``::

  cd /home/modules/modules/all/<module>
  ln -s <old-version>.lua default

Hiding modules
==============

As the number of modules keep growing, the list from ``module avail`` may become confusingly large for users.
Also, old and obsolete modules makes it difficult to find relevant modules.
As stated above, there is no safe way to uninstall a module.

During the module building process one may use::

  eb xxx.eb --hide-deps=zlib,Szip

see https://easybuild.readthedocs.io/en/latest/Manipulating_dependencies.html#installing-dependencies-as-hidden-modules-using-hide-deps

Hidden modules may also be defined in this (undocumented?) environment variable::

  EASYBUILD_HIDE_DEPS=zlib,Szip

Hiding modules with Lmod
------------------------

The Lmod_ command can be configured to hide modules from the ``module avail`` command.
Quoting https://github.com/TACC/Lmod/blob/master/Transition_to_Lmod7.txt::

  In the MODULERC file you can now do:
     #%Module
     hide-version     mpich/3.2-dbg

You can hide a module by specifying its full name.  
From Lmod_ version 7 you can also set a default module. 

The user file ``~/.modulerc`` takes precedence over the system ``modulerc``.
List the system-wide modulerc file by::

  module --config 2>&1 | grep MODULERCFILE

For example, create a user ``~/.modulerc`` or a system file ``/usr/share/lmod/etc/rc`` defining hidden modules::

  #%Module
  hide-version zlib/1.2.8

We have written a convenient script :download:`make-modulerc <attachments/make-modulerc>` for generating the modulerc file from a simple list of modules that we want to be hidden.

Lmod isVisibleHook
..................

It's also possible to hide modules by configuring Lmod, see the ``isVisibleHook`` function in https://lmod.readthedocs.io/en/latest/170_hooks.html

Using toolchains
================

A specific package may (should) be based upon on of the standard toolchains_.
Here we discuss the ones of interest to us.

To list all available toolchains::

  eb --list-toolchains

foss toolchain
--------------

The foss_ toolchain provides *GCC, OpenMPI, OpenBLAS/LAPACK, ScaLAPACK(/BLACS), FFTW*.

The foss_ toolchain was introduced in an effort to promote some toolchains as *common toolchains*, where the hope was that several sites would pick up these toolchains so we could benefit from each others efforts even more (the same was done with the intel toolchain which was a renaming of 'ictce'). 
We revisit these toolchains under the <year>(a|b) versioning scheme every 6 months.  (`Quote <https://lists.ugent.be/wws/arc/easybuild/2016-09/msg00015.html>`_).

.. _foss: https://github.com/hpcugent/easybuild-framework/blob/master/easybuild/toolchains/foss.py

Search for available foss_ toolchains::

  eb -S ^foss 

To build one of the foss_ toolchains::

  eb foss-2019b.eb -r

Intel compiler toolchains
-------------------------

Read `Using Environment Modules with Intel Development Tools <https://software.intel.com/en-us/articles/using-environment-modules-with-the-intel-development-tools>`_ (refers to old Tcl modules).

Search for the Intel compiler suite toolchains::

    eb -S '^intel*'

The Intel® Parallel_Studio_ XE 2019 compiler (see `Featured Documentation <https://software.intel.com/en-us/parallel-studio-xe/documentation/featured-documentation>`_) 
installation tar-ball files must first be downloaded under your license with Intel.
Download separately the tar-balls for ICC, IFORT and MKL_.

The Intel_Performance_Libraries_ including MKL_ and Intel_MPI_ are now available as **free downloads** (from version 2018.2 and onwards).
Please see also https://software.intel.com/en-us/articles/installing-intel-free-libs-and-python-yum-repo

.. _Parallel_Studio: https://software.intel.com/en-us/intel-parallel-studio-xe
.. _Intel_Release_notes: https://software.intel.com/en-us/articles/intel-parallel-studio-xe-2016-update-3-readme
.. _Intel_Performance_Libraries: https://software.intel.com/en-us/performance-libraries
.. _Intel_MPI: https://software.intel.com/en-us/intel-mpi-library
.. _MKL: https://software.intel.com/en-us/mkl

Installation procedure
......................

As the *modules* user create source directories where EasyBuild_ is going to look::

  mkdir -p $HOME/sources/iccifort $HOME/sources/imkl $HOME/sources/impi

Then move the tar-balls into place.  For example, with a 2019 compiler version (and the recommended 2018 version of Intel MPI)::

  mv parallel_studio_xe_2019_update5_composer_edition.tgz $HOME/modules/sources/i/iccifort/
  mv l_mkl_2019.5.281.tgz $HOME/modules/sources/i/imkl/
  mv l_mpi_2018.5.288.tgz $HOME/modules/sources/i/impi/

or alternatively make soft-links in these 3 directories pointing to where you put the tar-ball files.

The Intel compiler easyconfig_ files specify the location of the Intel license file as an **absolute path** (current directory or relative paths will not work)::

  license_file = HOME + '/licenses/intel/license.lic'

so it is recommended to use this convention and copy your site's private license file (for example, ``license.lic``)::

  mkdir -p $HOME/licenses/intel
  cp license.lic $HOME/licenses/intel/license.lic

If you use a FlexLM_ license manager server it is possible to use another approach (see https://lists.ugent.be/wws/arc/easybuild/2016-11/msg00008.html), for example::

  env INTEL_LICENSE_FILE=28518@<license-server> eb ifort-xxx.eb

It is recommended that *<license-server>* is a CNAME address pointing in DNS to the real license server. 

.. _FlexLM: https://software.intel.com/en-us/articles/intel-software-license-manager-getting-started-tutorial

In summary:

* Intel license_file must be specified.  Select a long-term valid name.
* Can be overridden by setting the environment variable INTEL_LICENSE_FILE before running eb.
* Absolute path example: license_file = HOME + '/licenses/intel/license.lic'
* License server example: license_file = port-number@license-server 

If you have the full *Intel® Parallel Studio XE 2019 Cluster Edition* you can build the full Intel compiler toolchain (including icc, ifort, impi, imkl)::

  eb intel-2019b.eb -r

To try out the latest Development_EB_files_.

.. _Development_EB_files: https://github.com/hpcugent/easybuild-easyconfigs/tree/develop/easybuild/easyconfigs/

It turns out that 2016 updates 2 and 3, and perhaps also the 2017 versions (?) may cause code crashes, see https://lists.ugent.be/wws/arc/easybuild/2016-11/msg00004.html

Intel iomkl toolchain
---------------------

The *OpenMPI 2.1.3* build of iomkl_ has some prerequisite CentOS packages::

  yum install libpciaccess-devel libxml2-devel

.. _iomkl: http://easybuild.readthedocs.io/en/latest/version-specific/Supported_software.html#list-software-iomkl-540

We have built an old 2016 version of the iomkl_ toolchain using modified EB files with these steps::

  eb icc-2016.3.210-GCC-5.4.0-2.26.eb iccifort-2016.3.210-GCC-5.4.0-2.26.eb ifort-2016.3.210-GCC-5.4.0-2.26.eb -r   # Build compiler modules
  eb OpenMPI-1.10.3-iccifort-2016.3.210-GCC-5.4.0-2.26.eb                                                           # Only for Slurm support
  eb iompi-2016.09-GCC-5.4.0-2.26.eb imkl-11.3.3.210-iompi-2016.09-GCC-5.4.0-2.26.eb -r                             # Build OpenMPI and MKL modules
  eb iomkl-2016.09-GCC-5.4.0-2.26.eb -r                                                                             # Build iomkl toolchain

Here we have configured our pre-existing GCC-5.4.0 compiler together with the Intel compilers, which requires tweaking some Development_EB_files_.

Slurm support in OpenMPI requires adding this line to the EB file::

  configopts += '--with-slurm --with-pmi=/usr/include/slurm --with-pmi-libdir=/usr '  # Support of Slurm

Toolchain step by step guide
----------------------------

The step_by_step_ guide will guide you through putting together a self-contained compiler toolchain, and using that toolchain to build a software package.

.. _step_by_step: https://github.com/hpcugent/easybuild/wiki/Step-by-step-guide

Modules of commercial software
==============================

MATLAB
------

When you have a licensed MATLAB_ software, EB files are available as shown by::

  $ eb -S MATLAB

Instructions for creating a ``MATLAB`` module are given in https://github.com/easybuilders/easybuild-easyconfigs/tree/master/easybuild/easyconfigs/m/MATLAB:

* Adjust the 'license_server', 'license_server_port' and 'key' values in the example easyconfig file.

* Steps to produce a 'source' tarball that EasyBuild can handle (example for 2020b):

  * create a directory 2020b
  * copy matlab-2020b.tar.gz in 2020b
  * copy the 'archives' directory containing installation files for toolboxes into '2020b'
  * unzip the matlab-2020b.tar.gz in 2020b
  * remove matlab-2020b.tar.gz
  * run 'chmod -R 755 \*' (overkill, yes, but it does the trick)
  * check whether bin/glnxa64/libstdc++.so.6 is correct, fix if required
  * tar the 2020b directory: 'tar cfvz matlab-2020b.tar.gz 2020b'

Note that directories in the ISO are read-only, so you must do the *chmod* above!

.. _MATLAB: https://www.mathworks.com/products/matlab.html

COMSOL
------

When you have a licensed COMSOL_ software, EB files are contributed in the mailing list thread https://lists.ugent.be/wws/arc/easybuild/2020-09/msg00019.html 

Prepare the tar-ball file just like for *MATLAB* above.

The EB file is very simple::

  name = 'COMSOL'
  version = '5.5.0.292'
  homepage = 'https://www.comsol.com'
  description = """
  COMSOL Multiphysics is a general-purpose software platform, based on
  advanced numerical methods, for modeling and simulating physics-based
  problems. 
  """
  toolchain = {'name': 'dummy', 'version': ''}
  sources = [
    SOURCELOWER_TAR_GZ,
  ]
  license_file = HOME + '/licenses/comsol/license.lic'
  moduleclass = 'phys'

You can point to a license server for building the module::

  env LMCOMSOL_LICENSE_FILE=port@lic-server-host eb COMSOL-xx.xx.eb


.. _COMSOL: https://www.comsol.com/

EasyBuild repositories
======================

Third-party EasyBuild_ repositories:

* Jülich Supercomputing Centre easyconfig repository: https://github.com/easybuilders/JSC

EasyBuild Pull Requests (PR)
============================

The EasyBuild_ easyconfigs_ package provides a collection of well-tested example easyconfig files for EasyBuild. 
Easyconfig files are used to specify which software to build, which version of the software (and its dependencies), which build parameters to use (e.g., which compiler toolchain to use), etc.

The Pull Requests are in the easyconfigs-PRs_ page.

To build software from a PR_::

  eb --from-pr=NNN  <some-file.eb>

where NNN is the number of the PR_ and the some-file.eb is an optional .eb file on the PR_ in case there are several in the PR_.

.. _easyconfigs: https://github.com/easybuilders/easybuild-easyconfigs
.. _easyconfigs-PRs: https://github.com/easybuilders/easybuild-easyconfigs/pulls
.. _PR: https://docs.github.com/en/github/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/about-pull-requests

Contributing back
=================

If you develop easyconfig_ files you can contribute them back to the community, see https://github.com/hpcugent/easybuild/wiki/Contributing-back.

Submitting pull requests (--new-pr)
-----------------------------------

See http://easybuild.readthedocs.io/en/latest/Integration_with_GitHub.html#github-new-pr

In its simplest form, you just provide the location of the file(s) that you want to include in the pull request::

  $ eb --new-pr test.eb

But first you need to set up github integration!

Setting up github integration
-----------------------------

To use ``eb --new-pr`` you need to link EasyBuild with your github account.  You only need to do this once.

1. Make an account on github_.

2. Set the environment variable `EASYBUILD_GITHUB_USER` to your github user name.

3. On github_ go to your account *Settings* and then *Developer settings*:

   * Select `Personal access tokens`_.
   * Press the ``Generate new token`` button.
   * Give the token a name (e.g. EasyBuild).
   * Select access to ``repo`` and ``gist``.
   * Then press the green ``Generate token`` button.
   * Copy the token string.

4. Run the command::

     $ eb --install-github-token

   and paste in the token at the prompt (it is treated as a password, and not displayed).

5. Check that everything works with::

     $ eb --check-github

.. _github: https://github.com

.. _`Personal access tokens`: https://github.com/settings/tokens
