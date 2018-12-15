/*
 *	utils.h -- System Utilitie Header
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: utils.h,v 1.25 2008-03-17 09:28:40 yy Exp $
 */

#include <time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <linux/autoconf.h>
#include "linux/ralink_gpio.h"

#define LED_ON				1
#define LED_OFF				0

extern int ledAlways(int gpio, int on);
extern int gpioLedSet(int gpio, unsigned int on, unsigned int off, unsigned int blinks, unsigned int rests, unsigned int times);
