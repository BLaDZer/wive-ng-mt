/*
 *	um.h -- GoAhead User Management public header
 *
 *	Copyright (c) GoAhead Software Inc., 1992-2000. All Rights Reserved.
 *
 *	See the file "license.txt" for information on usage and redistribution
 *
 * $Id: um.h,v 1.3 2007-02-01 07:41:01 winfred Exp $
 */

#ifndef _h_UM
#define _h_UM 1

/******************************** Description *********************************/

/*
 *	GoAhead User Management header. This defines the User Management
 *	public APIs.  Include this header for files that contain access to
 *	user inquiry or management.
 */

/********************************* Includes ***********************************/
#include "uemf.h"
/********************************** Defines ***********************************/

/*
 *	Error Return Flags
 */
#define UM_OK				0
#define UM_ERR_GENERAL		-1
#define UM_ERR_NOT_FOUND	-2
#define UM_ERR_PROTECTED	-3
#define UM_ERR_DUPLICATE	-4
#define UM_ERR_IN_USE		-5
#define UM_ERR_BAD_NAME		-6
#define UM_ERR_BAD_PASSWORD -7

/*
 *	Privilege Masks
 */
#define PRIV_NONE	0x00
#define PRIV_READ	0x01
#define PRIV_WRITE	0x02
#define PRIV_ADMIN	0x04

/*
 *	User classes
 */
typedef short bool_t;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef enum {
	AM_NONE = 0,
	AM_FULL,
	AM_BASIC,
	AM_DIGEST,
	AM_INVALID
} accessMeth_t;

/********************************** Prototypes ********************************/

/*
 *	umOpen() must be called before accessing User Management functions
 */
int				umOpen();

/*
 *	umClose() should be called before shutdown to free memory
 */
void				umClose();

/*
 *	umCommit() persists the user management database
 */
int				umCommit(char_t *filename);

/*
 *	umRestore() loads the user management database
 */
int				umRestore(char_t *filename);

/*
 *	umUser functions use a user ID for a key
 */
int				umAddUser(char_t *user, char_t *password,
							char_t *group, bool_t protect, bool_t disabled);

int				umDeleteUser(char_t *user);

char_t			*umGetFirstUser();
char_t			*umGetNextUser(char_t *lastUser);

bool_t			umUserExists(char_t *user);

char_t			*umGetUserPassword(char_t *user);
int				umSetUserPassword(char_t *user, char_t *password);

char_t			*umGetUserGroup(char_t *user);
int				umSetUserGroup(char_t *user, char_t *password);

bool_t			umGetUserEnabled(char_t *user);
int				umSetUserEnabled(char_t *user, bool_t enabled);

bool_t			umGetUserProtected(char_t *user);
int				umSetUserProtected(char_t *user, bool_t protect);

/*
 *	umGroup functions use a group name for a key
 */
int				umAddGroup(char_t *group, short privilege,
							accessMeth_t am, bool_t protect, bool_t disabled);

int				umDeleteGroup(char_t *group);

char_t 			*umGetFirstGroup();
char_t			*umGetNextGroup(char_t *lastUser);

bool_t			umGroupExists(char_t *group);
bool_t			umGetGroupInUse(char_t *group);

accessMeth_t		umGetGroupAccessMethod(char_t *group);
int				umSetGroupAccessMethod(char_t *group, accessMeth_t am);

bool_t			umGetGroupEnabled(char_t *group);
int				umSetGroupEnabled(char_t *group, bool_t enabled);

short			umGetGroupPrivilege(char_t *group);
int				umSetGroupPrivilege(char_t *group, short privileges);

bool_t			umGetGroupProtected(char_t *group);
int				umSetGroupProtected(char_t *group, bool_t protect);

/*
 *	umAccessLimit functions use a URL as a key
 */
int			umAddAccessLimit(char_t *url, accessMeth_t am,
						short secure, char_t *group);

int			umDeleteAccessLimit(char_t *url);

char_t		*umGetFirstAccessLimit();
char_t		*umGetNextAccessLimit(char_t *lastUser);

/*
 *	Returns the name of an ancestor access limit if
 */
char_t		*umGetAccessLimit(char_t *url);

bool_t		umAccessLimitExists(char_t *url);

accessMeth_t	umGetAccessLimitMethod(char_t *url);
int			umSetAccessLimitMethod(char_t *url, accessMeth_t am);

short		umGetAccessLimitSecure(char_t *url);
int			umSetAccessLimitSecure(char_t *url, short secure);

char_t		*umGetAccessLimitGroup(char_t *url);
int			umSetAccessLimitGroup(char_t *url, char_t *group);

/*
 *	Convenience Functions
 */

accessMeth_t	umGetAccessMethodForURL(char_t *url);
bool_t		umUserCanAccessURL(char_t *user, char_t *url);
#endif /* _h_UM */
