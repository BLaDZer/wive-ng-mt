#include "asp_mod_storage.h"

#define	LSDIR_INFO	"/tmp/lsdir"
#define	MOUNT_INFO	"/proc/mounts"

#define STORAGE_PATH	"/media"
#define STORAGE_SIGN	"/media/sd"

static int dir_count;
static int part_count;
static int media_dir_count;

static void storageRemoveFile(webs_t* wp, char_t *path, char_t *query) {
	char* file_name_in = websGetVar(wp, T("name"), T(""));
	char file_name[PATH_MAX] = {0};
	snprintf(file_name, PATH_MAX, "/media/%s", file_name_in);

	if (strstr(file_name, "/../") != NULL)
	{
		websError(wp, 500, T("Incorrect pathname!"));
		return;
	}

	int errcode = remove(file_name);

	if (errcode != 0) {
		websError(wp, 500, T("File removal error! (%i, %s)"), errcode, file_name);
		return;
	}

	outWrite("%i", errcode);
}

static void storageUnmountDrive(webs_t* wp, char_t *path, char_t *query) {
	char* pathname_in = websGetVar(wp, T("name"), T(""));
	char pathname[PATH_MAX] = {0};
	snprintf(pathname, PATH_MAX, "/media/%s", pathname_in);

	if (strstr(pathname, "/../") != NULL)
	{
		outWrite("-1");
		return;
	}

	int status = doSystem("umount \"%s\" && rmdir \"%s\"", pathname, pathname); //FIXME: insecure
	if (!WIFEXITED(status)) {
		outWrite("-1");
		return;
	}

	outWrite("%i", WEXITSTATUS(status));
}



static void storageGetDir(webs_t* wp, char_t *path, char_t *query) {
	DIR *dir;
	struct stat fstat;
	struct dirent *ent;
	int perm;
	int first = 1;

	char* dir_name_in = websGetVar(wp, T("name"), T(""));
	char dir_name[PATH_MAX] = {0};
	snprintf(dir_name, PATH_MAX, "/media/%s", dir_name_in);
	char* resolved = realpath(dir_name, NULL);
	strcpy(dir_name, resolved);
	free(resolved);


	outWrite("[\n");

	if ((dir = opendir(dir_name)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {

			if (!first) outWrite(",\n");
			outWrite("{ \"name\":\"%s\"", ent->d_name);

			const char* dtype = "UNK";
			if (ent->d_type == DT_DIR) dtype = "DIR";
			if (ent->d_type == DT_REG) dtype = "FILE";
			if (ent->d_type == DT_LNK) dtype = "LINK";

			char fullpath[PATH_MAX] = {0};
			snprintf(fullpath, PATH_MAX, "%s/%s", dir_name, ent->d_name);
			resolved = realpath(fullpath, NULL);
			strcpy(fullpath, resolved);
			free(resolved);

			if (stat(fullpath, &fstat) < 0) {
				perm = -1;
			} else {
				perm = fstat.st_mode;
			}

			outWrite(", \"perm\":%i", perm);

			if (ent->d_type == DT_DIR && strstr(fullpath,"/media/") == fullpath && strlen(ent->d_name)>2 && ent->d_name[0] == 's' && ent->d_name[1] == 'd') {
				dtype = "DRIVE";

				FILE *fp_mount = NULL;
				char partition[PATH_MAX] = {0};
				char mpath[PATH_MAX] = {0};
				char fs[32] = {0};
				char fsflags[1024] = {0};

				if (NULL != (fp_mount = fopen("/proc/mounts", "r")))
				{
					while(EOF != fscanf(fp_mount, "%s %s %s %s %*s %*s\n", partition, mpath, fs, fsflags))
					{
						if (0 == strcmp(mpath, fullpath)) {
							outWrite(", \"partition\":\"%s\", \"fs\":\"%s\", \"fsflags\":\"%s\"", partition, fs, fsflags);
							break;
						}
					}
					fclose(fp_mount);
				}
			}

			outWrite(", \"type\":\"%s\"", dtype);

			if (ent->d_type == DT_REG) {
				outWrite(", \"size\":%lld", (long long)fstat.st_size);
			}

			outWrite(" }");
			first = 0;
	}

		closedir(dir);
	}
	outWrite("\n]\n");
}

static void storageDiskAdm(webs_t* wp, char_t *path, char_t *query)
{
	char_t *submit;

	submit = websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "delete"))
	{
		char_t *dir_path = websGetVar(wp, T("dir_path"), T(""));
		doSystem("storage.sh deldir \"%s\"", dir_path);
		websRedirect(wp, "storage/disk_admin.asp");
	}
	else if (0 == strcmp(submit, "add"))
	{
		char_t *dir_name, *disk_part;

		dir_name = websGetVar(wp, T("adddir_name"), T(""));
		disk_part = websGetVar(wp, T("disk_part"), T(""));
		doSystem("storage.sh adddir \"%s/%s\"", disk_part, dir_name);
		websRedirect(wp, "form_close.asp");
	}
	else if (0 == strcmp(submit, "format"))
	{
		char_t *disk_part = websGetVar(wp, T("disk_part"), T(""));
		FILE *fp_mount = NULL;
		char part[30], mpath[30];

		if (NULL == (fp_mount = fopen("/proc/mounts", "r")))
		{
        		websRedirect(wp, "storage/disk_admin.asp");
			return;
		}
		while(EOF != fscanf(fp_mount, "%s %s %*s %*s %*s %*s\n", part, mpath))
		{
			if (0 == strcmp(mpath, disk_part))
				break;
		}
		fclose(fp_mount);
		doSystem("storage.sh %s %s %s",submit, part, mpath);
		websRedirect(wp, "storage/disk_admin.asp");
	}
	else if (0 == strcmp(submit, "remove"))
	{
		doSystem("storage.sh remove");
		websRedirect(wp, "storage/disk_admin.asp");
	}
}

static void storageDiskPart(webs_t* wp, char_t *mpath, char_t *query)
{
	char_t *part1_vol, *part2_vol, *part3_vol, *part4_vol;
	FILE *fp_mount = NULL;
	char part[30];

	if (NULL == (fp_mount = fopen("/proc/mounts", "r")))
		return;

	while(EOF != fscanf(fp_mount, "%s %*s %*s %*s %*s %*s\n", part))
	{
		fprintf(stderr, "chhung part: %s\n", part);
		if (NULL != strstr(part, "/dev/sd"))
			doSystem("umount -fl %s", part);
	}
	part1_vol = websGetVar(wp, T("part1_vol"), T(""));
	part2_vol = websGetVar(wp, T("part2_vol"), T(""));
	part3_vol = websGetVar(wp, T("part3_vol"), T(""));
	part4_vol = websGetVar(wp, T("part4_vol"), T(""));

	doSystem("storage.sh reparted %s %s %s %s", part1_vol, part2_vol, part3_vol, part4_vol);
	fclose(fp_mount);
}

#ifdef CONFIG_FTPD
parameter_fetch_t ftp_server_args[] =
{
	{ T("ftp_port"), "FtpPort", 0, T("") },
	{ T("ftp_idle_timeout"), "FtpIdleTime", 0, T("") },
	{ NULL, NULL, 0, NULL }
};

static void storageFtpSrv(webs_t* wp, char_t *mpath, char_t *query)
{
	char_t *ftp_enable = websGetVar(wp, T("ftp_enabled"), T("0"));
	char_t *submitUrl;
	if (ftp_enable == NULL)
		ftp_enable = "0";

	nvram_init(RT2860_NVRAM);
	ngx_nvram_bufset(wp, "RemoteFTP", ftp_enable);

	if (CHK_IF_DIGIT(ftp_enable, 1) || CHK_IF_DIGIT(ftp_enable, 2))
		setupParameters(wp, ftp_server_args, 0);

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	firewall_rebuild(0);
	doSystem("service inetd restart");

	submitUrl = websGetVar(wp, T("submit-url"), T(""));
	websRedirect(wp, submitUrl);
}
#endif

#ifdef CONFIG_USER_TRANSMISSION
parameter_fetch_t transmission_args[] =
{
	{ T("transRPCPort"), "TransRPCPort", 0, T("") },
	{ T("transAccess"), "TransAccess", 0, T("") },
	{ T("transAuthor"), "TransAuthor", 0, T("") },
	{ T("transLog"), "TransLogin", 0, T("") },
	{ T("transPass"), "TransPass", 0, T("") },
	{ T("transInPort"), "TransInPort", 0, T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void transmission(webs_t* wp, char_t *mpath, char_t *query)
{
	char_t *submitUrl;
	char_t *submit;

	submit = websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "apply")) {
		char_t *trans_enabled = websGetVar(wp, T("TransEnabled"), T("0"));
		if (trans_enabled == NULL)
		    trans_enabled = "0";

		nvram_init(RT2860_NVRAM);
		ngx_nvram_bufset(wp, "TransmissionEnabled", trans_enabled);

		if (CHK_IF_DIGIT(trans_enabled, 1))
			setupParameters(wp, transmission_args, 0);

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
		firewall_rebuild(0);
		doSystem("service transmission restart");
	} else if (0 == strcmp(submit, "start")) {
		doSystem("service transmission start");
	} else if (0 == strcmp(submit, "stop")) {
		doSystem("service transmission stop");
	} else if (0 == strcmp(submit, "reload")) {
		doSystem("service transmission reload");
	}

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}
#endif

static int getCount(webs_t *wp, char** params, int nparams)
{
    int type;
    char_t *field;
    char count[3];

    if (nparams < 2)
        return websWrite(wp, T("Insufficient args\n"));

    type = strToIntDef(params[0], 0);
    field = params[1];

    if (0 == strcmp(field, "AllDir"))
        sprintf(count, "%d", dir_count);
    else if (0 == strcmp(field, "AllPart"))
        sprintf(count, "%d", part_count);
    else if (0 == strcmp(field, "AllMediaDir"))
        sprintf(count, "%d", media_dir_count);

    if (1 == type) {
        if (!strcmp(count, ""))
        {
            return websWrite(wp, T("0"));
        }

        return websWrite(wp, T("%s"), count);
    }


//FIXME
/*
	if (!strcmp(count, ""))
		ejSetResult(eid, "0");

	ejSetResult(eid, count);
*/

    return 0;

}

static int ShowAllDir(webs_t *wp, char** params, int nparams)

{
	FILE *fp_mount = fopen(MOUNT_INFO, "r");
	char part[50], mpath[30];
	char dir_name[30];
	int dir_len = 0;

	if (NULL == fp_mount)
		return -1;

	dir_count = 0;

	while(EOF != fscanf(fp_mount, "%s %s %*s %*s %*s %*s\n", part, mpath))
	{
		DIR *dp;
		struct dirent *dirp;
		struct stat statbuf;

		if (0 != strncmp(mpath, "/media/sd", 9))
		{
			continue;
		}
		if (NULL == (dp = opendir(mpath)))
		{
			fprintf(stderr, "open %s error\n", mpath);
			return -1;
		}
		chdir(mpath);
		while(NULL != (dirp = readdir(dp)))
		{
			lstat(dirp->d_name, &statbuf);
			if(S_ISDIR(statbuf.st_mode))
			{
				if (0 == strncmp(dirp->d_name, ".", 1) ||
					0 == strcmp(dirp->d_name, "home"))
					continue;
				strcpy(dir_name, dirp->d_name);
				dir_len = strlen(dir_name);
				if (dir_len < 30 && dir_len > 0)
				{
					websWrite(wp, T("<tr><td><input type=\"radio\" name=\"dir_path\" value=\"%s/%s\"></td>"), mpath, dir_name);
					websWrite(wp, T("<td>%s/%s</td>"), mpath, dir_name);
					websWrite(wp, T("<input type=\"hidden\" name=\"dir_part\" value=\"%s\">"), part);
					websWrite(wp, T("<td>%s</td>"), part);
					websWrite(wp, T("</tr>"));
					dir_count++;
				}
			}
		}
		chdir("/");
		closedir(dp);
	}
	fclose(fp_mount);
	return 0;
}

static int ShowPartition(webs_t *wp, char** params, int nparams)

{
	FILE *fp = fopen(MOUNT_INFO, "r");
	char part[50], mpath[30];

	if (NULL == fp)
	    return -1;

	part_count = 0;

	while(EOF != fscanf(fp, "%s %s %*s %*s %*s %*s\n", part, mpath))
	{
		if (0 != strncmp(mpath, "/media/sd", 9))
			continue;
		websWrite(wp, T("<tr align=center>"));
		websWrite(wp, T("<td><input type=\"radio\" name=\"disk_part\" value=\"%s\"></td>"), mpath);
		websWrite(wp, T("<td>%s</td>"), part);
		websWrite(wp, T("<td>%s</td>"), mpath);
		websWrite(wp, T("</tr>"));
		part_count++;
	}
	fclose(fp);
	return 0;
}

static int getMaxVol(webs_t *wp, char** params, int nparams)
{
	FILE *pp = popen("fdisk -l /dev/sda", "r");
	char maxvol[5], unit[5];
	double transfer, result=0;

	if(!pp){
		syslog(LOG_ERR, "no fdisk, %s", __FUNCTION__);
		return -1;
	}

	fscanf(pp, "%*s %*s %s %s %*s %*s\n", maxvol, unit);
	pclose(pp);

	transfer = atof(maxvol);
	if (0 == strcmp(unit, "GB,"))
		result = transfer*1000;
	else if (0 == strcmp(unit, "MB,"))
		result = transfer;

	return websWrite(wp, T("%d"), result);
}

void asp_mod_storage_init() {
        aspDefineFunc(T("ShowPartition"), ShowPartition, ADMIN);
	aspDefineFunc(T("ShowAllDir"), ShowAllDir, ADMIN);
	aspDefineFunc(T("getCount"), getCount, ADMIN);
	aspDefineFunc(T("getMaxVol"), getMaxVol, ADMIN);
	websFormDefine(T("storageDiskAdm"), storageDiskAdm, ADMIN);
	websFormDefine(T("storageDiskPart"), storageDiskPart, ADMIN);

	websFormDefine(T("storageGetDir"), storageGetDir, ADMIN);
	websFormDefine(T("storageRemoveFile"), storageRemoveFile, ADMIN);
	websFormDefine(T("storageUnmountDrive"), storageUnmountDrive, ADMIN);
#ifdef CONFIG_FTPD
	websFormDefine(T("storageFtpSrv"), storageFtpSrv, ADMIN);
#endif
#ifdef CONFIG_USER_TRANSMISSION
	websFormDefine(T("formTrans"), transmission, ADMIN);
#endif
}
