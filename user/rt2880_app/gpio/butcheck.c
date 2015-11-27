#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <linux/autoconf.h>
#include "linux/ralink_gpio.h"

//#define DEBUG

#if (CONFIG_RALINK_GPIO_BTN_RESET >= 32)
#define GPIO_GRP		32
#define TEST_BIT(x, n)		(((x) & (1 << (n-GPIO_GRP))) != 0)
#else
#define GPIO_GRP		0
#define TEST_BIT(x, n)		(((x) & (1 << n)) != 0)
#endif

#define LOADDEFAULTS		5
#define FULLRESETTIME		90

static int gpio_read_but(int *value) {
	int fd, req;

	*value = 0;
	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}

#if (CONFIG_RALINK_GPIO_BTN_RESET >= 32)
	req = RALINK_GPIO6332_READ;
#else
	req = RALINK_GPIO_READ;
#endif

	if (ioctl(fd, req, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

static int gpio_set_dir_in(int gpio_num) {
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}

#if (CONFIG_RALINK_GPIO_BTN_RESET >= 32)
	req = RALINK_GPIO6332_SET_DIR_IN;
#else
	req = RALINK_GPIO_SET_DIR_IN;
#endif
	if (ioctl(fd, req, ((0xffffffff) & (1 << (gpio_num-GPIO_GRP)))) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

static void gpio_wait(void) {
	int d, presstime_reset=0;
#if (CONFIG_RALINK_GPIO_BTN_WPS > 0) && defined(CONFIG_USER_STORAGE)
	int presstime_wps=0;
#endif
	while (1) {
	    gpio_set_dir_in(CONFIG_RALINK_GPIO_BTN_RESET);
	    gpio_read_but(&d);
            /*
	     * gpio number = bit for test (if 0 - pressed, if 1 - open)
	     * if pressed wait and up count after one minit stop wait and call fullreset
	     */
	    if ((!TEST_BIT(d, CONFIG_RALINK_GPIO_BTN_RESET)) && presstime_reset < FULLRESETTIME) {
#ifdef DEBUG
		printf("butcheck_reset: pressed 0x%x, test %d, selected %d\n", d, !TEST_BIT(d, CONFIG_RALINK_GPIO_BTN_RESET), CONFIG_RALINK_GPIO_BTN_RESET);
#endif
		presstime_reset++;
	    } else {
		if (presstime_reset > 0) {
		    if (presstime_reset > LOADDEFAULTS) {
			if (presstime_reset < FULLRESETTIME) {
			    printf("butcheck_reset: fs_nvram_reset - load nvram default and restore original rwfs...");
    			    system("fs nvramreset && fs restore");
	    		    sleep(2);
			    reboot(RB_AUTOBOOT);
	    		} else {
			    printf("butcheck_reset: fs_nvram_fullreset - load nvram default and restore original rwfs...");
    			    system("fs fullreset");
	    		    sleep(2);
			    reboot(RB_AUTOBOOT);
	    		}
		    }
		    printf("butcheck_reset: short press - skip...\n");
		    presstime_reset=0;
	    	}
	    }
#if (CONFIG_RALINK_GPIO_BTN_WPS > 0) && defined(CONFIG_USER_STORAGE)
	    gpio_set_dir_in(CONFIG_RALINK_GPIO_BTN_WPS);
	    gpio_read_but(&d);
	    if ((!TEST_BIT(d, CONFIG_RALINK_GPIO_BTN_WPS)) && presstime_wps < FULLRESETTIME) {
#ifdef DEBUG
		printf("butcheck_wps: pressed 0x%x, test %d, selected %d\n", d, !TEST_BIT(d, CONFIG_RALINK_GPIO_BTN_WPS), CONFIG_RALINK_GPIO_BTN_WPS);
#endif
		presstime_wps++;
	    } else {
		if (presstime_wps > 0) {
		    if (presstime_wps > LOADDEFAULTS) {
			    printf("butcheck_wps: umount all external devices.");
    			    system("umount_all.sh");
	    		    sleep(2);
		    } else {
			printf("butcheck_wps: short press - skip...\n");
			presstime_wps=0;
		    }
	    	}
	    }
#endif
	    sleep(1);
	}
}

int main(int argc, char *argv[]) {
    printf("Start buttons WDG\n");
    gpio_wait();
    return 0;
}
