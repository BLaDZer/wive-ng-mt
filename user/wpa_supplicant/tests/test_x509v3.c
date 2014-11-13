/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/*
 * Testing tool for X.509v3 routines
 * Copyright (c) 2006, Jouni Malinen <jkmaline@cc.hut.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Alternatively, this software may be distributed under the terms of BSD
 * license.
 *
 * See README and COPYING for more details.
 */

#include "includes.h"

#include "common.h"
#include "asn1.h"
#include "x509v3.h"

extern int wpa_debug_level;


int main(int argc, char *argv[])
{
	char *buf;
	size_t len;
	struct x509_certificate *certs = NULL, *last = NULL, *cert;
	int i, reason;

	wpa_debug_level = 0;

	if (argc < 3 || strcmp(argv[1], "-v") != 0) {
		printf("usage: test_x509v3 -v <cert1.der> <cert2.der> ..\n");
		return -1;
	}

	for (i = 2; i < argc; i++) {
		printf("Reading: %s\n", argv[i]);
		buf = os_readfile(argv[i], &len);
		if (buf == NULL) {
			printf("Failed to read '%s'\n", argv[i]);
			return -1;
		}

		cert = x509_certificate_parse(buf, len);
		if (cert == NULL) {
			printf("Failed to parse X.509 certificate\n");
			return -1;
		}

		free(buf);

		if (certs == NULL)
			certs = cert;
		else
			last->next = cert;
		last = cert;
	}

	printf("\n\nValidating certificate chain\n");
	if (x509_certificate_chain_validate(last, certs, &reason) < 0) {
		printf("\nCertificate chain validation failed: %d\n", reason);
		return -1;
	}
	printf("\nCertificate chain is valid\n");

	return 0;
}
