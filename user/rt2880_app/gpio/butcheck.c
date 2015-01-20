#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/autoconf.h>
#include "linux/ralink_gpio.h"

int gpio_read_int(int *value) {
	int fd, req;

	*value = 0;
	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}

	req = RALINK_GPIO_READ;

	if (ioctl(fd, req, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int gpio_set_dir(void) {
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}

	req = RALINK_GPIO_SET_DIR_IN;

	if (ioctl(fd, req, 0xffffffff) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

//#define DEBUG

#define TEST_BIT(x, n)		(((x) & (1 << n)) != 0)

#define LOADDEFAULTS		5
#define FULLRESETTIME		60

void gpio_wait(void) {
	int d, presstime=0;
	while (1) {
	    gpio_set_dir();
	    gpio_read_int(&d);
            /*
	     * gpio number = bit for test (if 0 - pressed, if 1 - open)
	     * if pressed wait and up count after one minit stop wait and call fullreset
	     */
#ifdef DEBUG
	    printf("butcheck: pressed 0x%x, test %d, selected %d\n", d, !TEST_BIT(d, GPIO_BTN_RESET), GPIO_BTN_RESET);
#endif
	    if ((!TEST_BIT(d, GPIO_BTN_RESET)) && presstime < FULLRESETTIME) {
		presstime++;
	    } else {
		if (presstime > 0) {
		    if (presstime > LOADDEFAULTS) {
			if (presstime < FULLRESETTIME) {
			    printf("butcheck: fs_nvram_reset - load nvram default and restore original rwfs...");
    			    system("fs nvramreset && fs restore &");
	    		    sleep(20000000);
	    		} else {
			    printf("butcheck: fs_nvram_fullreset - load nvram default and restore original rwfs...");
    			    system("fs fullreset &");
	    		    sleep(20000000);
	    		}
		    }
		    printf("butcheck: short press - skip...\n");
		    presstime=0;
	    	}
	    }
	    sleep(1);
	}
}

int main(int argc, char *argv[]) {
    printf("Start buttons WDG\n");
    gpio_wait();
    return 0;
}
