#include <stdio.h>
#include <stdlib.h>

void usage(char *prog)
{
	printf ("Usage: %s <option> <IP-subnet> <IP-start> <IP-end> <name-prefix> <numbering-offset>\n", prog);
	/* Example: clusterlabel d 10.1.1 5 55 0 */
	printf ("option is: d (DNS map), r (Reverse DNS map),  p (PBS nodes file)\n");
	printf ("           l (labels),  h (hosts.equiv file), s (ssh_known_hosts)\n");
	exit(-1);
}

int main(int argc, char **argv)
{
	int i,start=-1,end=-1,offset=-1;
	char opt, *subnet, *prefix;

	/* Taylor these constants to your own cluster network */

	const char *DOM = "dcsc.fysik.dtu.dk";		/* DNS domain name */
	const char *MX = "ymer2.dcsc.fysik.dtu.dk";	/* Mail Exchanger */
	const char *PBS_PROP = "np=8 xeon5550 hp5412f";	/* Portable Batch System node props */
	const char *SSH_KEY = "ssh-rsa AAAAB3NzaC1yc2EAAAABIwAAAIEAn5D9SJrEArchjrVYZUI0kcBDdy2jvIAl4lZ3/hlyXlgN5XV2XtgYB3atmQlgnnqWJQK72MFEo8o5mEZG4V3qkmvG+gs2nSJH4PaKf4SoFSlpn7wLooa+LLjgWytJcWj0+ONHVNTeUb4Vdyvr26zq58cDMLEi7/4BXeMbjx6SXdc=";	/* Your Secure Shell public key from known_hosts */
	/* The printf formats assume 3-digit node numbering including
	 * leading zeroes (%3.3d).  Change this if you wish */

	if (argc!=7) usage(argv[0]);
	opt = argv[1][0];		/* Command option */
	subnet = argv[2];		/* IP class C subnet */
	start = atoi(argv[3]);		/* Starting IP-address */
	end = atoi(argv[4]);		/* Ending IP-address */
	prefix = argv[5];		/* Node name prefix */
	offset = atoi(argv[6]);		/* Offset in node numbering */

	if (start<0 || end<=start || end>254) usage(argv[0]);

	switch (opt) {
	
	case 'd':	/* DNS map */
		for (i=start; i<=end; i++) {
			printf("%s%3.3d\t\tIN\tA\t%s.%d\n",prefix,i+offset,subnet,i);
			printf("\t\tIN\tMX\t10 %s.\n",MX);
		}
		break;
	
	case 'r':	/* Reverse DNS map */
		for (i=start; i<=end; i++) {
			printf("%d\tIN\tPTR\t%s%3.3d.%s.\n",i,prefix,i+offset,DOM);
		}
		break;
	
	case 'p':	/* PBS nodes file */
		for (i=start; i<=end; i++) {
			printf("%s%3.3d %s\n",prefix,i,PBS_PROP);
		}
		break;
	
	case 'l':	/* Adhesive labels */
		for (i=start; i<=end; i++) {
			printf("%s%3.3d   %s%3.3d\n",prefix,i,prefix,i);
		}
		break;
	
	case 'h':	/* /etc/hosts.equiv file */
		for (i=start; i<=end; i++) {
			printf("%s%3.3d.%s\n",prefix,i,DOM);
		}
		break;
	
	case 's':	/* ssh_known_hosts */
		for (i=start; i<=end; i++) {
			printf("%s%3.3d,%s%3.3d.%s,%s.%d %s\n",
				prefix,i+offset,
				prefix,i+offset,DOM,subnet,i,SSH_KEY);
		}
		break;
	default:
		printf ("Unknown option: %c\n", opt);
		exit(-1);

	}
}
