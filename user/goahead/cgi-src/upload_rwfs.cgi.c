#include "upload.h"

void import(char *filename, int offset, int len)
{
	char cmd[4096];
	char data;
	FILE *fp, *src;
	char *pname = "/tmp/rwfstmp.tar.bz2";

	snprintf(cmd, 4096, "cp %s /var/tmpcgi", filename);
	system(cmd);

	if (!( fp = fopen(pname, "w+")))
		return;

	if(!( src = fopen(filename, "r")))
	{
		fclose(fp);
		return;
	}

	if (fseek(src, offset, SEEK_SET) == -1)
		printf("fseek error\n");

	while (len > 0)
	{
		if (!fread(&data, 1, 1, src))
			return;
		fwrite(&data, 1, 1, fp);
		len--;
	}

	fclose(fp);
	fclose(src);

	snprintf(cmd, 4096, "unpackrwfs.sh %s", pname);
	system(cmd);
}

int main (int argc, char *argv[])
{
	int file_begin, file_end;
	int line_begin, line_end;
	char *boundary; int boundary_len;
	char *filename = getenv("UPLOAD_FILENAME");

	html_header();

	if (!filename)
	{
		html_error("failed, can't get env var.\n");
		return -1;
	}

	line_begin = 0;
	if ((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1)
	{
		html_error(RFC_ERROR);
		return -1;
	}

	boundary_len = line_end - line_begin;
	boundary = getMemInFile(filename, line_begin, boundary_len);

	// sth like this..
	// Content-Disposition: form-data; name="filename"; filename="\\192.168.3.171\tftpboot\a.out"
	//
	char *line, *semicolon, *user_filename;
	line_begin = line_end + 2;
	if ((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1)
	{
		html_error(RFC_ERROR);
		goto err;
	}

	line = getMemInFile(filename, line_begin, line_end - line_begin);
	if (strncasecmp(line, "content-disposition: form-data;", strlen("content-disposition: form-data;")))
	{
		html_error(RFC_ERROR);
		goto err;
	}

	semicolon = line + strlen("content-disposition: form-data;") + 1;
	if (! (semicolon = strchr(semicolon, ';')))
	{
		html_error("dont support multi-field upload.\n");
		goto err;
	}

	user_filename = semicolon + 2;
	if (strncasecmp(user_filename, "filename=", strlen("filename=")))
	{
		html_error(RFC_ERROR);
		goto err;
	}

	user_filename += strlen("filename=");
	//until now we dont care about what  true filename is.
	free(line);

	// We may check a string  "Content-Type: application/octet-stream" here
	// but if our firmware extension name is the same with other known name, 
	// the browser will define other content-type itself instead,
	line_begin = line_end + 2;
	if ((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1)
	{
		html_error(RFC_ERROR);
		goto err;
	}

	line_begin = line_end + 2;
	if ((line_end = findStrInFile(filename, line_begin, "\r\n", 2)) == -1)
	{
		html_error(RFC_ERROR);
		goto err;
	}

	file_begin = line_end + 2;

	if ( (file_end = findStrInFile(filename, file_begin, boundary, boundary_len)) == -1)
	{
		printf(RFC_ERROR);
		goto err;
	}

	file_end -= 2;		// back 2 chars.(\r\n);

	// rwfs update base timeout
	html_success(60);

	import(filename, file_begin, file_end - file_begin);

	sleep (3);
	reboot(RB_AUTOBOOT);
err:
	free(boundary);
	exit(0);
}
