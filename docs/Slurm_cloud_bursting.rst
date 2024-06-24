.. _Slurm_cloud_bursting:

====================
Slurm cloud bursting
====================

.. Contents::

Slurm_ has the ability to support a cluster that grows and shrinks on demand, typically relying upon a service such as Amazon Elastic Computing Cloud (Amazon_EC2_), Google Cloud Platform (GCP_), Microsoft Azure_, or Oracle Cloud Infrastructure (OCI_).

.. _Slurm: https://www.schedmd.com/
.. _Power_Saving_Guide: https://slurm.schedmd.com/power_save.html
.. _publications: https://slurm.schedmd.com/publications.html
.. _slurm.conf: https://slurm.schedmd.com/slurm.conf.html
.. _sinfo: https://slurm.schedmd.com/sinfo.html
.. _Amazon_EC2: https://aws.amazon.com/ec2/
.. _GCP: https://cloud.google.com/
.. _Azure: https://azure.microsoft.com/en-us/
.. _OCI: https://www.oracle.com/cloud/

Some Slurm_ cloud bursting resources:

* Slurm_ `Elastic Computing <https://slurm.schedmd.com/elastic_computing.html>`_ (Cloud Bursting).
* Presentation *Slurm in the Clouds* in the Slurm_publications_ page.
* Review of the Slurm_ Power_Saving_Guide_ is strongly recommended. 
* Presentations on GCP_ and Cloud in the publications_ page.
* `Hybrid Cluster Guide <https://github.com/SchedMD/slurm-gcp/blob/master/docs/hybrid.md>`_.
* Rclone_ is a command line program to sync files and directories to and from a large number of cloud storage systems using rsync.
  See the `FAQ <https://rclone.org/faq/>`_ and `Usage <https://rclone.org/docs/>`_ documentation.

Jump to our top-level Slurm page: :ref:`SLURM`

.. _Rclone: https://rclone.org/
.. _Slurm_publications: https://slurm.schedmd.com/publications.html

.. _Slurm_configuration_for_cloud_nodes:

Slurm configuration for cloud nodes
===================================

Prerequisites
-------------

Make sure to install these prerequisites before building Slurm_ RPM packages::

  dnf install http-parser-devel json-c-devel

Check that slurmctld_ has JSON_ support::

  strings `which slurmctld ` | grep HAVE_JSON
  HAVE_JSON_C_INC 1
  HAVE_JSON_INC 1
  HAVE_JSON 1

.. _JSON: https://www.json.org/json-en.html
.. _slurmctld: https://slurm.schedmd.com/slurmctld.html
.. _bug_14925: https://bugs.schedmd.com/show_bug.cgi?id=14925

.. _configuring-slurm-conf-for-power-saving:

Configuring slurm.conf for power saving
------------------------------------------

According to the Slurm_ Power_Saving_Guide_  the following parameters in slurm.conf_ must be carefully configured, for example::

  # Exceptions to the node suspend/resume logic (partitions):
  SuspendExcParts=xeon8
  SuspendExcNodes=node00[1-9]
  ResumeTimeout=600
  # SuspendTime=3600   # Global value: the default is INFINITE.  DO NOT change the default!
  SuspendTimeout=120
  ResumeProgram=/usr/local/bin/cloudresume
  SuspendProgram=/usr/local/bin/cloudsuspend
  DebugFlags=Power
  TreeWidth=1000     # Or 65535, the default is 50.  Large values disable tree fanout of slurmd communications

**Some important points about power saving parameters**:

* If you set ``SuspendTime`` to anything but INFINITE (or -1), power saving shutdown of nodes will commence!
* It may be preferable to omit the global parameter and leave it with the default value ``SuspendTime=INFINITE``.
  In stead define it only on any relevant partitions, for example::

    PartitionName=my_partition SuspendTime=3600

* The ``SlurmctldParameters=idle_on_node_suspend`` causes nodes drained for maintenance purposes to become idle and available
  for running jobs.
  This is most likely **not** desirable.

* An important side effect of power saving suspension of nodes pertains to on-premise nodes.
  Compute nodes that are, for example, drained for maintenance purposes will be suspended and later resumed when needed by jobs.
  This is highly undesirable!

  This issue has been resolved in Slurm_ 23.02 by bug_15184_ which introduces a new slurm.conf_ parameter ``SuspendExcStates``.
  This permits to configure node states which you want to be excluded from power saving suspension.
  Valid states for ``SuspendExcStates`` include::

    CLOUD, DOWN, DRAIN, DYNAMIC_FUTURE, DYNAMIC_NORM, FAIL, INVALID_REG, MAINTENANCE, NOT_RESPONDING, PERFCTRS, PLANNED, RESERVED

  **Beware:** If any nodes in the cluster are running Slurm_ versions lower than 23.02, the slurmd_ will fail if the new ``SuspendExcStates`` parameter is specified!

.. _bug_14270: https://bugs.schedmd.com/show_bug.cgi?id=14270
.. _bug_15184: https://bugs.schedmd.com/show_bug.cgi?id=15184
.. _slurmd: https://slurm.schedmd.com/slurmd.html

Resume and Suspend scripts
--------------------------

Each site must develop their own ResumeProgram_ and SuspendProgram_ scripts.
Remember that Slurm_ executes these scripts as the *slurm* user, which must therefore be able to power up/down cloud nodes!

In addition, a ResumeFailProgram_ script will be useful for handling nodes that do not resume (power up) correctly.

.. _SuspendProgram: https://slurm.schedmd.com/slurm.conf.html#OPT_SuspendProgram
.. _ResumeProgram: https://slurm.schedmd.com/slurm.conf.html#OPT_ResumeProgram
.. _ResumeFailProgram: https://slurm.schedmd.com/slurm.conf.html#OPT_ResumeFailProgram

Logging from the *ResumeProgram/SuspendProgram* scripts must be programmed in the scripts.
This example may be used::

  action="start"
  echo "`date` User $USER invoked $action $0 $*" >>/var/log/slurm/power_save.log

See also some suspend/resume scripts in https://github.com/OleHolmNielsen/Slurm_tools/tree/master/power_save


Site-to-Site VPN connection
===========================

The local on-premise cluster network is the subnet (for example, 10.1.x.x/16) where all your compute nodes are connected together.
Likewise, when you use an off-site cloud service, a number of (virtual) compute nodes are also connected together in another (virtual) subnet (for example, 10.0.0.0/16).

When you want to connect your on-premise cluster to the cloud service compute nodes,
you need to set up a **virtual private network** (VPN_) 
which extends a private network across a public network and enables users to send and receive data across shared or public networks 
as if their computing devices were directly connected to the private network.
Such a VPN_ connection is a prerequisite for accessing to the specific cloud services described below,
and you will need to configure this early on.

For a *Site-to-site* VPN_ tunnel from a cloud service to the local on-premise network using a Linux server, see our page:

* Libreswan_IPsec_VPN_.

Network router devices can also be used, see the cloud service's list of supported devices:

* Azure: `Validated VPN devices and device configuration guides <https://docs.microsoft.com/en-us/azure/vpn-gateway/vpn-gateway-about-vpn-devices>`_.

.. _Libreswan_IPsec_VPN: https://wiki.fysik.dtu.dk/it/Libreswan_IPsec_VPN
.. _VPN: https://en.wikipedia.org/wiki/Virtual_private_network

Resources for Slurm on Google Cloud Platform (GCP)
==================================================

* Slurm on Google Cloud Platform: https://github.com/SchedMD/slurm-gcp
* Deploy an Auto-Scaling HPC Cluster with Slurm: https://codelabs.developers.google.com/codelabs/hpc-slurm-on-gcp/#0
* Google HPC: https://cloud.google.com/solutions/hpc/

Resources for Slurm on Microsoft Azure
======================================

**Login** to Azure_:

* https://portal.azure.com/

Other topics:

* Building a Slurm_ cluster inside Azure_: https://azure.microsoft.com/en-us/resources/templates/slurm/
* `Azure CycleCloud <https://azure.microsoft.com/en-us/features/azure-cyclecloud/>`_:
  Create, manage, operate, and optimize HPC and big compute clusters of any scale.
  See `Azure CycleCloud Documentation <https://docs.microsoft.com/en-us/azure/cyclecloud/?view=cyclecloud-8>`_.

Azure subscription
------------------

Get started with the Azure EA portal: https://docs.microsoft.com/en-us/azure/billing/billing-ea-portal-get-started

To create a Microsoft Azure account related to an EA enrollment, then please follow these steps:

* Step 1: Log in to the Microsoft Azure Enterprise Portal using your Microsoft account or Work or School account.

  Manage subscriptions on the page https://ea.azure.com/manage/subscription/managedaccounts

* Step 2: Create an initial subscription by clicking on Add Subscription and stepping through the Account Sign-Up process on the Microsoft Azure Portal.

  There is an Azure Setup Guide: https://portal.azure.com/?quickstart=true#blade/Microsoft_Azure_Resources/QuickstartPlaybookBlade/guideId/intro-azure-setup

Azure CLI
---------

The Azure_ *command-line interface* Azure_CLI_ is an environment to create and manage Azure_ resources. 
Azure_CLI_ is available across Azure_ services and is designed to get you working quickly with Azure_, with an emphasis on automation.
See:

* `What is Azure CLI <https://docs.microsoft.com/en-us/cli/azure/what-is-azure-cli>`_
* `Get started with Azure CLI <https://docs.microsoft.com/en-us/cli/azure/get-started-with-azure-cli>`_.

Install Azure_CLI_ using `dnf <https://learn.microsoft.com/en-us/cli/azure/install-azure-cli-linux?pivots=dnf>`_:

* On RHEL import the Microsoft repository key::

    rpm --import https://packages.microsoft.com/keys/microsoft.asc

* Create the file ``/etc/yum.repos.d/azure-cli.repo`` with contents::

    [azure-cli]
    name=Azure CLI
    baseurl=https://packages.microsoft.com/yumrepos/azure-cli
    enabled=1
    gpgcheck=1
    gpgkey=https://packages.microsoft.com/keys/microsoft.asc

* Install the azure-cli package::

    dnf install azure-cli

Alternatively, the following Ansible_ role may be used::

  - name: Create the Azure CLI YUM repository
    yum_repository:
      name: "azure-cli"
      description: "Azure CLI"
      baseurl: "https://packages.microsoft.com/yumrepos/azure-cli"
      gpgkey: "https://packages.microsoft.com/keys/microsoft.asc"
      gpgcheck: yes
      enabled: yes
  - name: Install the azure-cli RPM
    package:
      name: "azure-cli"
      state: present


.. _Azure_CLI: https://docs.microsoft.com/en-us/cli/azure/?view=azure-cli-latest

Azure CLI login
---------------

The Linux command for Azure_CLI_ is::

  az
  az help        # Lists az subcommands
  az --version   # Display CLI version

Append the ``--help`` option to display subcommand help, for example::

  az account --help

To authenticate your CLI session with your existing Azure_ subscription, use the az_login_ command::

  az login

If the CLI can open your default browser, it will do so and load an Azure_ sign-in page.
Otherwise, open a browser page at https://aka.ms/devicelogin and enter the **authorization code** displayed in your terminal.

Select your existing Azure_ subscription in the web page.
Your Azure_ credentials will be stored in the folder ``$HOME/.azure/``.
None of your sign-in information is stored by the Azure_CLI_. 
Instead, an authentication refresh token is generated by Azure_ and stored. 
If the token is revoked you get a message from the CLI saying you need to sign in again.

After signing in, CLI commands are run against your default subscription. If you have multiple subscriptions, you can change your default subscription.

List your Azure_ account information::

  az account list --output table

If you want to log out from the Azure_CLI_::

  az logout

.. _az_login: https://docs.microsoft.com/en-us/cli/azure/authenticate-azure-cli?view=azure-cli-latest


Virtual network in Azure
========================

The Virtual Machine nodes should be connected to a shared virtual network (Azure_VNet_) inside Azure_ so that a VPN subnet connection can be made later.

See the `Quickstart: Create a virtual network using the Azure portal <https://docs.microsoft.com/en-us/azure/virtual-network/quick-create-portal>`_.

In *Home > New > Create virtual network* create a new *Resource Group*:

* In *Project details* select an existing *Resource group* for your VMs, or create a new one.
* Choose a suitable network name, for example *VPNnetresourcegroup*, and select the Azure_ region, for example *West Europe*.

* Choose a sufficiently large **IP address space**, for example 10.0.0.0/16.
* Create a subnet, for example *VMsubnet*, and define smaller subnet rangewithin your IP address space, for example 10.0.1.0/24, leaving room for other subnets (the VPN gateways require their own separate subnet).

Then join all **future VMs** to *Virtual network/subnet: VPNnetresourcegroup*.

Currently existing VMs can unfortunately not be moved to a different virtual network, 
see https://docs.microsoft.com/en-us/azure/virtual-network/virtual-network-network-interface.
However, with some tricks an existing VM can be recreated on a different virtual network, see https://techgenix.com/moving-a-vm/

In conclusion, if a VM is connected to the wrong subnet, it is easier to delete the VM and create it again from scratch.

Virtual Network management
--------------------------

`Create, change, or delete a virtual network <https://docs.microsoft.com/en-us/azure/virtual-network/manage-virtual-network>`_ including 
`Change DNS servers <https://docs.microsoft.com/en-us/azure/virtual-network/manage-virtual-network#change-dns-servers>`_.

In the *Virtual Network* you can specify the use of on-premise DNS servers in the *Settings->DNS Servers* menu by adding the IP-addresses of DNS servers:

* Select *Custom* in stead of *Default (Azure-provided)* and reboot the VM.


VPN gateway to Azure
====================

Use the Azure_ portal to create a Site-to-Site VPN gateway connection from your on-premises network to the Azure_VNet_:

* `Create a Site-to-Site connection in the Azure portal <https://docs.microsoft.com/en-us/azure/vpn-gateway/vpn-gateway-howto-site-to-site-resource-manager-portal>`_.
* About VPN Gateways see the vpngateways_ page including the `Gateway SKUs <https://docs.microsoft.com/en-us/azure/vpn-gateway/vpn-gateway-about-vpngateways#gwsku>`_ which you must choose from.
* A list of Azure_ validated VPN devices is in `About VPN devices and IPsec/IKE parameters for Site-to-Site VPN Gateway connections <https://docs.microsoft.com/en-us/azure/vpn-gateway/vpn-gateway-about-vpn-devices>`_.
  The Libreswan_ IPsec_ VPN router (which is described below) is **not** a validated device!

.. _vpngateways: https://docs.microsoft.com/en-us/azure/vpn-gateway/vpn-gateway-about-vpngateways

Do **not** choose the **Basic** gateway SKU because it does not support IKEv2_, see vpngateways_.  You may select a better SKU from the list, for example **VpnGw2** is recommended.

You may use Azure_CLI_ to list your Azure_VNet_::

  az network vnet list -o table

Configure VPN gateways
----------------------

The VPN gateways configuration steps are as follows:

1. Create an Azure Resource named **Public IP address**.
   You have to assign a name to this, for example *PublicIPAddress*.
   Write down the VPN gateway's *Public IP address*.

2. Create a **route based** Azure_ site-to-site VPN gateway for your selected Azure_ *Virtual Network* (Azure_VNet_), for example *VPNnet*:

   * Do **not** choose the **Basic** gateway SKU because it does not support IKEv2_, see vpngateways_.
   * You may select a better SKU from the list, for example **VpnGw2** on **Generation1**.
   * Create a new IP address and use the *Public IP address* assigned above.
   * Deployment may take 20 minutes or more!

3. Create a new resource, search for *Local network gateway*:

   * Assign a public IP address for the on-premise local network VPN gateway.
   * Create the new *Local network gateway* with the local IP address.
   * For *Address space* use the local cluster network's subnet which will contain the local VPN gateway router.
   * Select the VPN subnet resource group.  Creation may take 20 minutes or more!

4. Go to the VPN gateway page and click on *Settings -> Connections*.

   Click *+Add* to create a new named network connection, for example *Azure2Local*.
   You must select a *Pre-shared key* to be used both by the Azure_ and local VPN gateway routers.

You still need to configure a *Local network gateway* with the local public IP address which acts as a router to the local cluster network's subnet.

Libreswan IPsec VPN router
--------------------------

For a *Site-to-site* VPN_ tunnel from Azure_ to the local on-premise network, a Libreswan_ IPsec_ VPN_ router can be used:

* See the separate page `Libreswan IPsec VPN <https://wiki.fysik.dtu.dk/it/Libreswan_IPsec_VPN>`_.

.. _VPN: https://en.wikipedia.org/wiki/Virtual_private_network
.. _IPsec: https://en.wikipedia.org/wiki/IPsec
.. _Libreswan: https://libreswan.org/

Additional Azure VPN gateway information
----------------------------------------

There are two distinct types of Azure_ VPN gateways:

* Policy-based (static-routing) gateway.

* Route-based (dynamic-routing) gateway.

Azure supports three types of Point-to-site VPN options:

* Secure Socket Tunneling Protocol (SSTP). SSTP is a Microsoft proprietary SSL-based solution that can penetrate firewalls since most firewalls open the outbound TCP port that 443 SSL uses.

* OpenVPN. OpenVPN is a SSL-based solution that can penetrate firewalls since most firewalls open the outbound TCP port that 443 SSL uses.

* IKEv2_ VPN. IKE_ (defined in RFC7296_) VPN is a standards-based IPsec VPN solution that uses outbound UDP ports 500 and 4500 and IP protocol no. 50.
  Firewalls do not always open these ports, so there is a possibility of IKEv2 VPN not being able to traverse proxies and firewalls.

See the `VPN Gateway FAQ <https://docs.microsoft.com/en-us/azure/vpn-gateway/vpn-gateway-vpn-faq>`_ about which type to configure.
**Note:** Libreswan uses IKEv2_.

Other resources are:

* `Build an Azure site-to-site VPN for DevTest <https://blog.notnot.ninja/2020/09/19/azure-site-to-site-vpn/>`_.
* `Create a route-based VPN gateway using the Azure portal <https://docs.microsoft.com/en-us/azure/vpn-gateway/create-routebased-vpn-gateway-portal>`_.
* `Validated VPN devices and device configuration guides <https://docs.microsoft.com/en-us/azure/vpn-gateway/vpn-gateway-about-vpn-devices>`_.

* `Linux Point-to-Site (P2S) connection using OpenVPN infrastructure <https://azure.microsoft.com/en-gb/blog/point-to-site-p2s-connection-using-openvpn-infrastructure/>`_.
  Software at https://github.com/simonesavi/p2s-with-openvpn
  **Note:** OpenVPN can *not* be used for *Site-to-site* connections.
* Azure VPN Gateways VPN device configuration samples: https://github.com/Azure/Azure-vpn-config-samples

.. _IKE: https://en.wikipedia.org/wiki/Internet_Key_Exchange
.. _IKEv2: https://en.wikipedia.org/wiki/Internet_Key_Exchange
.. _RFC7296: https://tools.ietf.org/html/rfc7296
.. _Azure_VNet: https://docs.microsoft.com/en-us/azure/virtual-network/virtual-networks-overview

Create resources and machines in Azure home
===========================================

Go to the Azure_home_ page and create a Resource_group_ with a descriptive name such as *Group_compute_nodes*.

Now you can create a new machine in the Virtual_machines_ page by clicking *+Add*:

* Select the Resource_group_ created above.
* Choose a machine name, for example *node001*.
* Choose your region, for example *West Europe*.
* Availability: no redundancy
* Select a predefined Azure_ image, for example *AlmaLinux-based 8*.
  Display lists of available free Almalinux images::

    az vm image list --all -o table --publisher almalinux

  Display lists of available Rockylinux images::

    az vm image list --all -o table --publisher erockyenterprisesoftwarefoundationinc1653071250513
    az vm image list -f rocky --all -o table

  There is a free `Rocky Linux 8 - Official <https://azuremarketplace.microsoft.com/en-us/marketplace/apps/erockyenterprisesoftwarefoundationinc1653071250513.rockylinux>`_ image,
  see also the list of `Rockylinux cloud images <https://rockylinux.org/cloud-images/>`_.

* Azure_ spot instance: No
* Size of the machine: Select from the list of available Azure_ machine_sizes_ in the *Change size* menu.

  For a small compute node the memory optimized *Standard DS12 v2 (4 vcpus, 28 GiB memory) Intel Xeon Broadwell* may be suitable.
  You may be hit by errors_for_resource_quotas_.

* **Pricing** of different VMs is listed in the Linux_Virtual_Machines_Pricing_ page.

.. _Linux_Virtual_Machines_Pricing: https://azure.microsoft.com/en-us/pricing/details/virtual-machines/linux/#Linux

* For *Administrator account*:

  - For *Authentication type* the *Password* method may have to be used until we get documentation about using Linux OpenSSH public keys (only Putty_keys_ work).

    SSH public key for Azure_ is documented in https://docs.microsoft.com/en-us/azure/virtual-machines/linux/mac-create-ssh-keys

  - For the *administrator username* for the VM make a choice::

      azureuser

    which is **not a regular user** (since users will probably be created later) nor the **root superuser** (forbidden by Azure_).

    The administrator user's $HOME directory will be the OS standard */home*.
    The home directory may later be changed inside the VM by the **root** user with usermod_::

      root# usermod --home /var/lib/azureuser --move-home azureuser

    This will only work if *azureuser* is logged out and *root* has been logged in directly.

In the *Next->Disks* window select:

* OS disk type: **Standard HDD** may suffice.
* No data disks are needed because we will store user files in a separate Azure_ storage.

In the *Next->Networking* tab:

* For the Network interface select the desired *Virtual network* (Azure_VNet_) for VPN access (see above how to create a shared virtual network named *VPNnetresourcegroup*).
  It is very hard to change the virtual network once the VM has been created!

* For *Public IP* select **None** because we will not use this.

* Check the box *Delete NIC when VM is deleted*.

* SSH (port 22) access should be enabled for remote management.

In the *Next->Management* window select:

* Enable **auto-shutdown** every day at an appropriate time (choose your timezone).

In the *Next->Management* window the default values should be fine.

In the *Next->Tags* window select:

* Tag the *Name* field to identify the group using this machine for billing purposes.

Finally do a *Review and create*.
This will open a popup window::

  Generate new key pair
  Select: Download private key and create resource

**You must save** the VM's SSH public key file ``<vm-name>_key.pem`` to disk and use as shown below to login.


.. _Azure_home: https://portal.azure.com/#home
.. _Resource_group: https://portal.azure.com/#blade/HubsExtension/BrowseResourceGroups
.. _Virtual_machines: https://portal.azure.com/#blade/HubsExtension/BrowseResourceBlade/resourceType/Microsoft.Compute%2FVirtualMachines
.. _machine_sizes: https://docs.microsoft.com/en-us/azure/virtual-machines/linux/sizes
.. _errors_for_resource_quotas: https://docs.microsoft.com/en-us/azure/azure-resource-manager/templates/error-resource-quota
.. _usermod: https://linux.die.net/man/8/usermod
.. _Putty_keys: https://www.ssh.com/ssh/putty/windows/puttygen

Create and Manage Linux VMs with the Azure CLI
----------------------------------------------

* `Tutorial: Create and Manage Linux VMs with the Azure CLI <https://docs.microsoft.com/en-us/azure/virtual-machines/linux/tutorial-manage-vm>`_.

See the Azure_CLI_ subcommands for VMs::

  az vm --help
  az vm create --help
  az vm image list --all -o table --publisher almalinux   # List all AlmaLinux images

Example from the above tutorial with an AlmaLinux image::

  az vm create \
    --resource-group myResourceGroupVM \
    --name myVM \
    --image almalinux:almalinux:8-gen2:8.6.2022052501 \
    --admin-username azureuser \
    --generate-ssh-keys

To create **multiple copies of a virtual machine (VM)** for use in Azure for development and test:

* An image can be created from a VM and then used to create multiple VMs, see `Create an image of a VM in the portal <https://docs.microsoft.com/en-us/azure/virtual-machines/capture-image-portal>`_.

* The most general way is to capture a managed image of the VM or of the OS VHD.
  Go to `How to create a managed image of a virtual machine or VHD <https://docs.microsoft.com/en-us/azure/virtual-machines/linux/capture-image>`_.

To create, store and share images at scale, see `Azure Compute Galleries <https://docs.microsoft.com/en-us/azure/virtual-machines/create-gallery>`_.

The basic steps listed in the documentation are:

1. Deprovision the VM.
2. Create VM image.
3. Create a VM from the captured image.
4. Create a gallery for storing and sharing resources.

Start and stop VMs with the Azure CLI
-------------------------------------

See *Experts Inside* example `Start, Restart and Stop Azure VM from Azure CLI <https://dev.to/expertsinside/start-restart-and-stop-azure-vm-from-azure-cli-41n9>`_.

List your existing Azure_ VMs (output formatted as a simple *table*)::

  az vm list --show-details -o table

Display details of a named ``VM`` in a specified ``resourcegroup``::

  az vm show -g <resourcegroup> -n <VM> [-d|--show-details]

Starting and stopping a ``VM``::

  az vm start --help
  az vm start      -g <resourcegroup> -n <VM>
  az vm restart    -g <resourcegroup> -n <VM>
  az vm deallocate -g <resourcegroup> -n <VM>     # Shut down <VM> and deallocate resources (stops billing)
  az vm stop       -g <resourcegroup> -n <VM>     # Does NOT deallocate resources and BILLING CONTINUES

**Important:**
The VM_deallocate_ operation is required to release the VM resource and stop billing.
The VM_stop_ operation only powers off, but does not release resources.

Add the flag ``--no-wait`` to avoid waiting for the long-running operation to finish.
Add the flag ``--verbose`` to increase command logging.

See `Azure CLI 2.0: Quickly Start / Stop ALL VMs <https://build5nines.com/azure-cli-2-0-quickly-start-stop-all-vms/>`_.

.. _VM_deallocate: https://docs.microsoft.com/en-us/rest/api/compute/virtualmachines/deallocate
.. _VM_stop: https://docs.microsoft.com/en-us/rest/api/compute/virtualmachines/poweroff

Adding swap space
-----------------

The Azure_ web interface does not seem to allow the adding of a swap space.
It remains to be seen whether VM compute nodes would actually benefit from having a swap space.

How to configure a swap file in Linux Azure virtual machines: 

* https://support.microsoft.com/en-us/help/4010058/how-to-add-a-swap-file-in-linux-azure-virtual-machines
* https://azure.microsoft.com/en-us/blog/swap-space-in-windows-azure-virtual-machines-running-pre-built-linux-images-part-1/

Increasing machine size
-----------------------

* Click on the *?* (Help) item at the top right of the web page.
* Click the HelpSupport_ page and open a *New support request*.
* For the *Issue type* select *Service and subscription limits (quotas)*.
* **Details needed**.

.. _HelpSupport: https://portal.azure.com/#blade/Microsoft_Azure_Support/HelpAndSupportBlade/overview

Create a virtual machine
-------------------------------

Some RedHat resources:

* How to prepare a Red Hat-based virtual machine for Azure: https://azure.microsoft.com/en-us/resources/how-to-prepare-a-red-hat-based-virtual-machine-for-azure/
* Prepare a Red Hat-based virtual machine for Azure: https://docs.microsoft.com/en-us/azure/virtual-machines/linux/redhat-create-upload-vhd

  * Prepare a Red Hat-based virtual machine from an ISO by using a **kickstart file** automatically: https://docs.microsoft.com/en-us/azure/virtual-machines/linux/redhat-create-upload-vhd#prepare-a-red-hat-based-virtual-machine-from-an-iso-by-using-a-kickstart-file-automatically

SSH login to Azure VM
=====================

When the VM was created, the SSH authorized key for login was offered by the Azure_ GUI.
You must save that SSH key file to a local folder, for example ``$HOME/.ssh/<vm-name>_key.pem``.
If you wish to display the public key in the certificate use the openssl-rsa_ command::

  openssl rsa -in $HOME/.ssh/<vm-name>_key.pem -pubout

.. _openssl-rsa: https://www.openssl.org/docs/man1.1.1/man1/rsa.html

The VM login **username** may be defined as **azureuser**.

First ensure that you can ping the VM's IP-address (private subnet or public IP).

Next you can try to make an SSH login to the VM named <vm-name>::

  ssh -i $HOME/.ssh/<vm-name>_key.pem azureuser@<vm-name>

You can now append additional SSH public keys from your on-premise hosts to the ``azureuser`` file::

  /home/azureuser/.ssh/authorized_keys

Root login
----------

Root logins to Azure VMs are not permitted.

Use sudo_ to run root commands, for example::

  sudo dnf update

.. _sudo: https://en.wikipedia.org/wiki/Sudo

Ansible with Azure
==================

* Ansible on Azure documentation: https://docs.microsoft.com/en-us/azure/ansible/

  * Using Ansible with Azure: https://docs.microsoft.com/en-us/azure/ansible/ansible-overview

* Ansible: Microsoft Azure Guide: https://docs.ansible.com/ansible/latest/scenario_guides/guide_azure.html

.. _Ansible: https://www.ansible.com/

Azure_ does not allow remote root logins, so root access must be done using sudo_ access.

Use this Ansible_ command to run commands as user ``azureuser`` on the VM named ``<vm-name>``::

  ansible-playbook --private-key $HOME/.ssh/<vm-name>_key.pem --user azureuser --become <Ansible-commands> -l <vm-name>

Note:

* The SSH key defined above may be required, and this is used with the ``--private-key`` option. 
* The user ``azureuser`` should be able to run superuser commands using sudo_ which is used with the ``--become`` option.

.. _sudo: https://en.wikipedia.org/wiki/Sudo

Storage in Azure
================

NFS file service on Azure
-------------------------

NFS v3 is supported in Azure since (about) February 2022.
Some Azure_ NFSv3 documentation:

* `Network File System (NFS) 3.0 protocol support for Azure Blob Storage <https://docs.microsoft.com/en-us/azure/storage/blobs/network-file-system-protocol-support>`_.

* `Mount Blob Storage by using the Network File System (NFS) 3.0 protocol <https://docs.microsoft.com/en-us/azure/storage/blobs/network-file-system-protocol-support-how-to>`_.

  This article provides guidance on how to mount a container in Azure_Blob_ Storage from a Linux-based Azure virtual machine (VM) or a Linux system that runs on-premises by using the Network File System (NFS) 3.0 protocol.

Another NFS file service is `Azure NetApp Files <https://azure.microsoft.com/en-us/services/netapp/>`_.

.. _Azure_Blob: https://docs.microsoft.com/en-us/azure/storage/blobs/storage-blobs-introduction

Azure storage accounts
----------------------

For NFSv3 storage read `Network File System (NFS) 3.0 protocol support for Azure Blob Storage <https://docs.microsoft.com/en-us/azure/storage/blobs/network-file-system-protocol-support>`_.

.. _Storage_accounts: https://portal.azure.com/#blade/HubsExtension/BrowseResource/resourceType/Microsoft.Storage%2FStorageAccounts

Go to Azure_home_ and select Storage_accounts_:

* Create a new *Resource group*, for example *My_storage* (any previously defined storage resource groups can also be used).

* For the *Storage account name* you must select a name that is globally unique within all of Azure_!
  Example name: *myexamplestorage* (only lowercase letters and numbers are permitted).

* For replication you may want to choose the cheapest *Locally redundant storage* (LRS) which is only replicated inside the given data center, not at other locations.
  More expensive alternatives are also available.

* In the *Advanced* window enable NFSv3 storage by:

  * Check the box *Enable hierarchical namespace*
  * Then check the box *Enable network file system v3*

  Choose a suitable Access_tier_ (see Blob_pricing_):

  * Hot Tier (The Hot tier has the highest storage costs, but the lowest access costs.)
  * Cool Tier (The Cool tier has lower storage costs and higher access costs compared to the Hot tier.)

* In the *Networking* window set *Public network access* to **Disabled**.
  When 'network file share v3' is enabled, the connectivity method must be set to 'public endpoint (selected networks)'.
  Then you must create a Storage_Private_Endpoint_ connection to grant access.
  See also `What is a private endpoint? <https://docs.microsoft.com/en-us/azure/private-link/private-endpoint-overview>`_.

.. _Storage_Private_Endpoint: https://docs.microsoft.com/en-us/azure/storage/common/storage-private-endpoints

  Select the appropriate *Virtual network* and *Subnets* for your subscription.

  All Storage_accounts_ have a public IP-address associated with them.
  The public IP-address can be looked up with DNS as described in `Azure Private Endpoint DNS configuration <https://docs.microsoft.com/en-us/azure/private-link/private-endpoint-dns>`_ 
  and the section *DNS changes for private endpoints* in Storage_Private_Endpoint_,
  for example::

    $ nslookup myexamplestorage.blob.core.windows.net
    $ nslookup myexamplestorage.privatelink.blob.core.windows.net

  Go to the *Private endpoint connections* tab and *Add a private endpoint* with an IP-address in your Azure Vnet (10.0.0.0/24.).
  This private IP will be used for NFS mounts in VMs as well as from on-premise nodes.

  For the private endpoint select *Private DNS integration=No*.

* In the *Tags* window select the appropriate name tag.

* Finally *Review and Create* the storage account.

The DNS servers used in your cluster must be configured to resolve Azure_ DNS names to the *Private endpoint connections*, for example::

  myexamplestorage.privatelink.blob.core.windows.net has address 10.0.0.7

If you use your on-premise DNS server, you have to create a new DNS zone ``privatelink.blob.core.windows.net`` containing addresses for the *Private endpoint connections*, for example this DNS name::

  myexamplestorage IN A 10.0.0.7

It is a good idea to add the reverse-DNS lookup in the 0.0.10.in-addr.arpa. zone as well::

  7 IN PTR myexamplestorage.privatelink.blob.core.windows.net.

Mount Azure storage container
-----------------------------

Mount an NFSv3 storage:

* Mount Blob Storage by using the `Network File System (NFS) 3.0 protocol <https://docs.microsoft.com/en-us/azure/storage/blobs/network-file-system-protocol-support-how-to>`_.

In your *Storage account* created above, go to the **Containers** item:

* Click + Containers to create a new container.  
  Select an appropriate name, then click *Advanced* and check that *No Root Squash* is selected.

  Now click *Create*.

* In a VM machine mount the container to some example directory::

    mkdir /nfsdata
    mount -o sec=sys,vers=3,nolock,proto=tcp <storage-account-name>.blob.core.windows.net:/<storage-account-name>/<container-name>  /nfsdata

  If you add this to ``/etc/fstab`` you must also add the ``nofail`` flag.

  The ``sec=sys`` is the default setting, which uses local UNIX UIDs and GIDs by means of AUTH_SYS to authenticate NFS operations. 

  The same NFS mount name can also be used by on-premise nodes, provided they have been configured for routing to the Azure subnet.

.. _Access_tier: https://docs.microsoft.com/en-us/azure/storage/blobs/access-tiers-overview
.. _Blob_pricing: https://azure.microsoft.com/pricing/details/storage/blobs/

NFS automount Azure storage container
-------------------------------------

In stead of the above static NFS mounting of NFSv3 storage, you can also use the Linux NFS autofs_ automounter.
Here is a suggested method which works on both Azure_ VMs and on-premise servers:

1. Create the autofs_ file ``/etc/auto.azure`` with contents::

     nfsdata -sec=sys,vers=3,nolock,proto=tcp <storage-account-name>.blob.core.windows.net:/<storage-account-name>/<container-name>

   Make sure that DNS resolving ``<storage-account-name>.blob.core.windows.net`` works correctly both on Azure VMs and on-premise with your chosen ``<storage-account-name>``, see the above DNS setup.
   The storage name ``nfsdata`` can be any other name that makes sense.

2. Append to ``/etc/auto.master`` a line::

     /azure /etc/auto.azure --timeout=60

   This will automount onto the ``/azure`` directory.

3. Optional: You may add a link in the Azure_ VM from ``/home`` to the NFSv3 storage::

     $ ln -s /azure/nfsdata /home/nfsdata

   This can be used, for example, to keep users' $HOME directories under ``/home`` while actually storing the directories on the shared Azure NFSv3 filesystem.

4. Restart the autofs_ service::

     systemctl restart autofs

Now you have automatic mounting of the Azure NFSv3 storage on both Azure_ VMs and on-premise servers::

  $ cd /home/nfsdata

The autofs_ will unmount filesystems after some minutes of inactivity.

.. _autofs: https://wiki.archlinux.org/title/autofs

Cloud node configuration
========================

When the cloud node subnet has been connected to the on-premise cluster subnet through the above described VPN_ tunnel,
there are some additional network configurations which you need to consider:

1. DNS_ servers configured in the cloud subnet.  
   It is recommended to configure the on-premise DNS_ server IP-addresses in the cloud nodes as the primary DNS_ servers, see this file::

     cat /etc/resolv.conf

   which should contain the same DNS_ server IP-addresses as for the on-premise nodes.
   The cloud's default DNS_ server should be added last (a maximum of 3 IP-addresses is allowed, see the resolv.conf_ man-page).

2. The DNS_ domain name configured in the cloud nodes should be the same as your on-premise nodes.
   This can be accomplished with the hostnamectl_ command, for example::

     cloudnode001# hostnamectl set-hostname cloudnode001.cluster.example.com

3. Add the cloud nodes' static IP-addresses to your on-premise DNS_ server so that DNS_ lookups work, for example::

     host cloudnode001.cluster.example.com

4. Cloud nodes should relay SMTP_ E-mails via an SMTP_ server in your on-premise cluster subnet.
   For example, a Sendmail_ server could have this in the ``/etc/mail/sendmail.mc`` file::

     define(`SMART_HOST', `mail.cluster.example.com')dnl

   Therefore it is recommended to set up an SMTP_ relay server on your cluster subnet for relaying mails to your organization's mail service. 
   Note: The SMTP_ relay server's firewall must permit SMTP_ connections from the cloud nodes.

5. Set up IP routing from on-premise servers and compute nodes to the cloud nodes as described in https://wiki.fysik.dtu.dk/it/Libreswan_IPsec_VPN#route-ip-traffic-via-the-vpn-tunnel.

.. _DNS: https://en.wikipedia.org/wiki/Domain_Name_System
.. _resolv.conf: https://en.wikipedia.org/wiki/Resolv.conf
.. _hostnamectl: https://www.redhat.com/sysadmin/set-hostname-linux
.. _SMTP: https://en.wikipedia.org/wiki/Simple_Mail_Transfer_Protocol
.. _Sendmail: https://en.wikipedia.org/wiki/Sendmail
