#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/autoconf.h>

#include "libnvram.h"
#include "libnvram_utils.h"

static int set_usage(char *aout)
{
	int i;

	printf("Usage example: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr 1.2.3.4\n");
	}
	return -1;
}

static int get_usage(char *aout)
{
	int i;

	printf("Usage: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr\n");
	}
	return -1;
}

static int buf_get_usage(char *aout)
{
	int i;

	printf("Usage: \n");
	for (i = 0; i < getNvramNum(); i++){
		printf("\t%s %s ", aout, getNvramName(i));
		printf("lan_ipaddr wan_ipaddr ...\n");
	}
	return -1;
}

static int ra_nv_set(int argc,char **argv)
{
	int index, rc;
	char *fz = "", *key = "", *value = "";

	if (argc == 1 || argc > 5)
		return set_usage(argv[0]);

	if (argc == 2) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
		value = "";
	} else if (argc == 3) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
		value = argv[2];
	} else {
		fz = argv[1];
		key = argv[2];
		value = argv[3];
	}

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return set_usage(argv[0]);
	}

	rc = nvram_set(index, key, value);

	return rc;
}

static int ra_nv_get(int argc, char *argv[])
{
	char *fz;
	char *key;
	char *rc;

	int index, ret;

	if (argc != 3 && argc != 2)
		return get_usage(argv[0]);

	if (argc == 2) {
		fz = DEFAULT_FLASH_ZONE_NAME;
		key = argv[1];
	} else {
		fz = argv[1];
		key = argv[2];
	}

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return get_usage(argv[0]);
	}

	rc = nvram_get(index, key);
	if (rc) {
	    printf("%s\n", rc);
	    ret = 0;
	} else {
	    fprintf(stderr, "nvram_get return error or not return data!\n");
	    ret = -1;
	}

	return (ret);
}

static int ra_nv_buf_get(int argc, char *argv[])
{
	char *fz, *key, *rc;
	int i, index, ret=0;

	if (argc < 3)
		return buf_get_usage(argv[0]);

	fz = argv[1];
	key = argv[2];

	if ((index = getNvramIndex(fz)) == -1) {
		fprintf(stderr,"%s: Error: \"%s\" flash zone not existed\n", argv[0], fz);
		return get_usage(argv[0]);
	}

	for (i = 2; i < argc; i++) {
	    rc = nvram_get(index, argv[i]);
	    if (rc) {
		printf("%s=\"%s\"\n",argv[i], rc);
	    } else {
		fprintf(stderr, "nvram_buf_get return error or not return data!\n");
		ret = -1;
		break;
	    }
	}

	return (ret);
}

static int gen_staticroutes(int mode, int genmode)
{
	static_routing_rebuild_etc();

	return 0;
}

static int gen_iptablesrules(int mode, int genmode)
{
	firewall_rebuild_etc();

	return 0;
}

void usage(char *cmd)
{
	printf("Usage:\n");
	printf("nvram_<command> [<platform>] [<file>]\n\n");
	printf("command:\n");
	printf("  get              - get value from nvram for <platform>\n");
	printf("  buf_get          - buffered get value from nvram for <platform>\n");
	printf("  set              - set value to nvram for <platform>\n");
	printf("  show             - display values in nvram for <platform>\n");
	printf("  renew            - replace nvram values for <platform> with <file>\n");
	printf("  clear            - clear all entries in nvram for <platform>\n");
	printf("  default          - load default for <platform>\n");
	printf("  genwlconfig      - generate config file from nvram for <platform>\n");
	printf("  genstaticroutes  - generate static routes scripts from nvram for <platform>\n");
	printf("  geniptablesrules - generate iptables rules from nvram for <platform>\n");
	printf("platform:\n");
	printf("  2860    - first module\n");
#ifndef CONFIG_RT_SECOND_IF_NONE
	printf("  rtdev    - second module\n");
#endif
	printf("file:\n");
	printf("          - file name for renew command\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	char *cmd = "";
	int mode = RT2860_NVRAM, genmode = RT2860_NVRAM;

	if (argc < 2)
		usage(argv[0]);

	//call nvram_get, nvram_set or nvram_show
	if (cmd == strrchr(argv[0], '/'))
		cmd++;
	else
		cmd = argv[0];

	if (!strncmp(argv[1], "rtdev", 5)) {
#if !defined(CONFIG_RT_SECOND_IF_NONE) && defined(CONFIG_KERNEL_NVRAM_SPLIT_INIC)
	    mode = RTINIC_NVRAM;
#endif
	    genmode = RTINIC_NVRAM;
	}

	if (!strncmp(cmd, "nvram_get", 10))
		return ra_nv_get(argc, argv);
	else if (!strncmp(cmd, "nvram_buf_get", 14))
		return ra_nv_buf_get(argc, argv);
	else if (!strncmp(cmd, "nvram_set", 10))
		return ra_nv_set(argc, argv);
	else if (!strncmp(cmd, "nvram_show", 11))
		return nvram_show(mode);
	else if (!strncmp(cmd, "nvram_clear", 12))
		return nvram_clear(mode);
	else if (!strncmp(cmd, "nvram_genwlconfig", 18))
		return gen_wifi_config(mode, genmode);
	else if (!strncmp(cmd, "nvram_genstaticroutes", 22))
		return gen_staticroutes(mode, genmode);
	else if (!strncmp(cmd, "nvram_geniptablesrules", 23))
		return gen_iptablesrules(mode, genmode);
	else if (!strncmp(cmd, "nvram_renew", 12)) {
		if (argc >= 3)
		    return nvram_renew(mode, argv[2]);
	} else if (!strncmp(cmd, "nvram_default", 14))
		return nvram_load_default();
	else
	    usage(argv[0]);

	return 0;
}
