#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/autoconf.h>

#include "linux/ralink_gpio.h"

enum {
	gpio_in,
	gpio_out,
};
enum {
#if defined (CONFIG_RALINK_RT3052)
	gpio2300,
	gpio3924,
	gpio5140,
#elif defined (CONFIG_RALINK_RT3883)
	gpio2300,
	gpio3924,
	gpio7140,
	gpio9572,
#elif defined (CONFIG_RALINK_RT3352)
	gpio2300,
	gpio3924,
	gpio4540,
#elif defined (CONFIG_RALINK_RT5350)
	gpio2100,
	gpio2722,
#elif defined (CONFIG_RALINK_MT7620)
	gpio2300,
	gpio3924,
	gpio7140,
	gpio72,
#elif defined (CONFIG_RALINK_MT7621)
	gpio3100,
	gpio6332,
	gpio9564,
#elif defined (CONFIG_RALINK_MT7628)
	gpio3100,
	gpio6332,
	gpio9564,
#else
	gpio2300,
#endif
};

int gpio_set_dir(int r, int dir)
{
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (dir == gpio_in) {
#if defined (CONFIG_RALINK_RT3052)
		if (r == gpio5140)
			req = RALINK_GPIO5140_SET_DIR_IN;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_RT3883)
		if (r == gpio9572)
			req = RALINK_GPIO9572_SET_DIR_IN;
		else if (r == gpio7140)
			req = RALINK_GPIO7140_SET_DIR_IN;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_RT3352)
		if (r == gpio4540)
			req = RALINK_GPIO4540_SET_DIR_IN;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_RT5350)
		if (r == gpio2722)
			req = RALINK_GPIO2722_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_MT7620)
		if (r == gpio72)
			req = RALINK_GPIO72_SET_DIR_IN;
		else if (r == gpio7140)
			req = RALINK_GPIO7140_SET_DIR_IN;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_MT7621)
		if (r == gpio9564)
			req = RALINK_GPIO9564_SET_DIR_IN;
		else if (r == gpio6332)
			req = RALINK_GPIO6332_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_MT7628)
		if (r == gpio9564)
			req = RALINK_GPIO9564_SET_DIR_IN;
		else if (r == gpio6332)
			req = RALINK_GPIO6332_SET_DIR_IN;
		else
#endif
			req = RALINK_GPIO_SET_DIR_IN;
	}
	else {
#if defined (CONFIG_RALINK_RT3052)
		if (r == gpio5140)
			req = RALINK_GPIO5140_SET_DIR_OUT;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_RT3883)
		if (r == gpio9572)
			req = RALINK_GPIO9572_SET_DIR_OUT;
		else if (r == gpio7140)
			req = RALINK_GPIO7140_SET_DIR_OUT;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_RT3352)
		if (r == gpio4540)
			req = RALINK_GPIO4540_SET_DIR_OUT;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_RT5350)
		if (r == gpio2722)
			req = RALINK_GPIO2722_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_MT7620)
		if (r == gpio72)
			req = RALINK_GPIO72_SET_DIR_OUT;
		else if (r == gpio7140)
			req = RALINK_GPIO7140_SET_DIR_OUT;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_MT7621)
		if (r == gpio9564)
			req = RALINK_GPIO9564_SET_DIR_OUT;
		else if (r == gpio6332)
			req = RALINK_GPIO6332_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_MT7628)
		if (r == gpio9564)
			req = RALINK_GPIO9564_SET_DIR_OUT;
		else if (r == gpio6332)
			req = RALINK_GPIO6332_SET_DIR_OUT;
		else
#endif
			req = RALINK_GPIO_SET_DIR_OUT;
	}
	if (ioctl(fd, req, 0xffffffff) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int gpio_set_dir_g(int r, int dir, int gpio_num)
{
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (dir == gpio_in) {
#if defined (CONFIG_RALINK_RT3052)
		if (r == gpio5140)
			req = RALINK_GPIO5140_SET_DIR_IN;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_RT3883)
		if (r == gpio9572)
			req = RALINK_GPIO9572_SET_DIR_IN;
		else if (r == gpio7140)
			req = RALINK_GPIO7140_SET_DIR_IN;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_RT3352)
		if (r == gpio4540)
			req = RALINK_GPIO4540_SET_DIR_IN;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_RT5350)
		if (r == gpio2722)
			req = RALINK_GPIO2722_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_MT7620)
		if (r == gpio72)
			req = RALINK_GPIO72_SET_DIR_IN;
		else if (r == gpio7140)
			req = RALINK_GPIO7140_SET_DIR_IN;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_MT7621)
		if (r == gpio9564)
			req = RALINK_GPIO9564_SET_DIR_IN;
		else if (r == gpio6332)
			req = RALINK_GPIO6332_SET_DIR_IN;
		else
#elif defined (CONFIG_RALINK_MT7628)
		if (r == gpio9564)
			req = RALINK_GPIO9564_SET_DIR_IN;
		else if (r == gpio6332)
			req = RALINK_GPIO6332_SET_DIR_IN;
		else
#endif
			req = RALINK_GPIO_SET_DIR_IN;
	}
	else {
#if defined (CONFIG_RALINK_RT3052)
		if (r == gpio5140)
			req = RALINK_GPIO5140_SET_DIR_OUT;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_RT3883)
		if (r == gpio9572)
			req = RALINK_GPIO9572_SET_DIR_OUT;
		else if (r == gpio7140)
			req = RALINK_GPIO7140_SET_DIR_OUT;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_RT3352)
		if (r == gpio4540)
			req = RALINK_GPIO4540_SET_DIR_OUT;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_RT5350)
		if (r == gpio2722)
			req = RALINK_GPIO2722_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_MT7620)
		if (r == gpio72)
			req = RALINK_GPIO72_SET_DIR_OUT;
		else if (r == gpio7140)
			req = RALINK_GPIO7140_SET_DIR_OUT;
		else if (r == gpio3924)
			req = RALINK_GPIO3924_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_MT7621)
		if (r == gpio9564)
			req = RALINK_GPIO9564_SET_DIR_OUT;
		else if (r == gpio6332)
			req = RALINK_GPIO6332_SET_DIR_OUT;
		else
#elif defined (CONFIG_RALINK_MT7628)
		if (r == gpio9564)
			req = RALINK_GPIO9564_SET_DIR_OUT;
		else if (r == gpio6332)
			req = RALINK_GPIO6332_SET_DIR_OUT;
		else
#endif
			req = RALINK_GPIO_SET_DIR_OUT;
	}
#if defined(CONFIG_RALINK_MT7620)
	if (gpio_num >= 24 && gpio_num <=39){
		gpio_num = gpio_num - 24;
	}else if(gpio_num >=40 && gpio_num<=71){
		gpio_num = gpio_num - 40;
	}else if(gpio_num == 72){
		gpio_num = gpio_num - 72;
	}

#elif defined(CONFIG_RALINK_MT7621)
	if (gpio_num >= 32 && gpio_num <=63){
		gpio_num = gpio_num - 32;
	}else if(gpio_num >=64 && gpio_num<=95){
		gpio_num = gpio_num - 64;
	}

#elif defined(CONFIG_RALINK_MT7628)
	if (gpio_num >= 32 && gpio_num <=63){
		gpio_num = gpio_num - 32;
	}else if(gpio_num >=64 && gpio_num<=95){
		gpio_num = gpio_num - 64;
	}
#endif
	if (ioctl(fd, req, (0xffffffff) & (1 << gpio_num) ) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int gpio_read_int(int r, int *value)
{
	int fd, req;

	*value = 0;
	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}

#if defined (CONFIG_RALINK_RT3052)
	if (r == gpio5140)
		req = RALINK_GPIO5140_READ;
	else if (r == gpio3924)
		req = RALINK_GPIO3924_READ;
	else
#elif defined (CONFIG_RALINK_RT3883)
	if (r == gpio9572)
		req = RALINK_GPIO9572_READ;
	else if (r == gpio7140)
		req = RALINK_GPIO7140_READ;
	else if (r == gpio3924)
		req = RALINK_GPIO3924_READ;
	else
#elif defined (CONFIG_RALINK_RT3352)
	if (r == gpio4540)
		req = RALINK_GPIO4540_READ;
	else if (r == gpio3924)
		req = RALINK_GPIO3924_READ;
	else
#elif defined (CONFIG_RALINK_RT5350)
	if (r == gpio2722)
		req = RALINK_GPIO2722_READ;
	else
#elif defined (CONFIG_RALINK_MT7620)
	if (r == gpio72)
		req = RALINK_GPIO72_READ;
	else if (r == gpio7140)
		req = RALINK_GPIO7140_READ;
	else if (r == gpio3924)
		req = RALINK_GPIO3924_READ;
	else
#elif defined (CONFIG_RALINK_MT7621)
	if (r == gpio9564)
		req = RALINK_GPIO9564_READ;
	else if (r == gpio6332)
		req = RALINK_GPIO6332_READ;
	else
#elif defined (CONFIG_RALINK_MT7628)
	if (r == gpio9564)
		req = RALINK_GPIO9564_READ;
	else if (r == gpio6332)
		req = RALINK_GPIO6332_READ;
	else
#endif
		req = RALINK_GPIO_READ;
	if (ioctl(fd, req, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int gpio_write_int(int r, int value)
{
	int fd, req;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
#if defined (CONFIG_RALINK_RT3052)
	if (r == gpio5140)
		req = RALINK_GPIO5140_WRITE;
	else if (r == gpio3924)
		req = RALINK_GPIO3924_WRITE;
	else
#elif defined (CONFIG_RALINK_RT3883)
	if (r == gpio9572)
		req = RALINK_GPIO9572_WRITE;
	else if (r == gpio7140)
		req = RALINK_GPIO7140_WRITE;
	else if (r == gpio3924)
		req = RALINK_GPIO3924_WRITE;
	else
#elif defined (CONFIG_RALINK_RT3352)
	if (r == gpio4540)
		req = RALINK_GPIO4540_WRITE;
	else if (r == gpio3924)
		req = RALINK_GPIO3924_WRITE;
	else
#elif defined (CONFIG_RALINK_RT5350)
	if (r == gpio2722)
		req = RALINK_GPIO2722_WRITE;
	else
#elif defined (CONFIG_RALINK_MT7620)
	if (r == gpio72)
		req = RALINK_GPIO72_WRITE;
	else if (r == gpio7140)
		req = RALINK_GPIO7140_WRITE;
	else if (r == gpio3924)
		req = RALINK_GPIO3924_WRITE;
	else
#elif defined (CONFIG_RALINK_MT7621)
	if (r == gpio9564)
		req = RALINK_GPIO9564_WRITE;
	else if (r == gpio6332)
		req = RALINK_GPIO6332_WRITE;
	else
#elif defined (CONFIG_RALINK_MT7628)
	if (r == gpio9564)
		req = RALINK_GPIO9564_WRITE;
	else if (r == gpio6332)
		req = RALINK_GPIO6332_WRITE;
	else
#endif
		req = RALINK_GPIO_WRITE;
	if (ioctl(fd, req, value) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int gpio_enb_irq(void)
{
	int fd;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_GPIO_ENABLE_INTP) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int gpio_dis_irq(void)
{
	int fd;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_GPIO_DISABLE_INTP) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

int gpio_reg_info(int gpio_num)
{
	int fd;
	ralink_gpio_reg_info info;

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	info.pid = getpid();
	info.irq = gpio_num;
	if (ioctl(fd, RALINK_GPIO_REG_IRQ, &info) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

void gpio_test_write(void)
{
	//set gpio direction to output
#if defined (CONFIG_RALINK_RT3052)
	gpio_set_dir(gpio5140, gpio_out);
	gpio_set_dir(gpio3924, gpio_out);
	gpio_set_dir(gpio2300, gpio_out);
#elif defined (CONFIG_RALINK_RT3883)
	gpio_set_dir(gpio9572, gpio_out);
	gpio_set_dir(gpio7140, gpio_out);
	gpio_set_dir(gpio3924, gpio_out);
	gpio_set_dir(gpio2300, gpio_out);
#elif defined (CONFIG_RALINK_RT3352)
	gpio_set_dir(gpio4540, gpio_out);
	gpio_set_dir(gpio3924, gpio_out);
	gpio_set_dir(gpio2300, gpio_out);
#elif defined (CONFIG_RALINK_RT5350)
	gpio_set_dir(gpio2722, gpio_out);
	gpio_set_dir(gpio2100, gpio_out);
#elif defined (CONFIG_RALINK_MT7620)
	gpio_set_dir(gpio72, gpio_out);
	gpio_set_dir(gpio7140, gpio_out);
	gpio_set_dir(gpio3924, gpio_out);
	gpio_set_dir(gpio2300, gpio_out);
#elif defined (CONFIG_RALINK_MT7621)
	gpio_set_dir(gpio9564, gpio_out);
	gpio_set_dir(gpio6332, gpio_out);
	gpio_set_dir(gpio3100, gpio_out);
#elif defined (CONFIG_RALINK_MT7628)
	gpio_set_dir(gpio9564, gpio_out);
	gpio_set_dir(gpio6332, gpio_out);
	gpio_set_dir(gpio3100, gpio_out);
#else
	gpio_set_dir(gpio2300, gpio_out);
#endif

	//turn off LEDs
#if defined (CONFIG_RALINK_RT3052)
	gpio_write_int(gpio5140, 0xffffffff);
	gpio_write_int(gpio3924, 0xffffffff);
	gpio_write_int(gpio2300, 0xffffffff);
#elif defined (CONFIG_RALINK_RT3883)
	gpio_write_int(gpio9572, 0xffffffff);
	gpio_write_int(gpio7140, 0xffffffff);
	gpio_write_int(gpio3924, 0xffffffff);
	gpio_write_int(gpio2300, 0xffffffff);
#elif defined (CONFIG_RALINK_RT3352)
	gpio_write_int(gpio4540, 0xffffffff);
	gpio_write_int(gpio3924, 0xffffffff);
	gpio_write_int(gpio2300, 0xffffffff);
#elif defined (CONFIG_RALINK_RT5350)
	gpio_write_int(gpio2722, 0xffffffff);
	gpio_write_int(gpio2100, 0xffffffff);
#elif defined (CONFIG_RALINK_MT7620)
	gpio_write_int(gpio72, 0xffffffff);
	gpio_write_int(gpio7140, 0xffffffff);
	gpio_write_int(gpio3924, 0xffffffff);
	gpio_write_int(gpio2300, 0xffffffff);
#elif defined (CONFIG_RALINK_MT7621)
	gpio_write_int(gpio9564, 0xffffffff);
	gpio_write_int(gpio6332, 0xffffffff);
	gpio_write_int(gpio3100, 0xffffffff);
#elif defined (CONFIG_RALINK_MT7628)
	gpio_write_int(gpio9564, 0xffffffff);
	gpio_write_int(gpio6332, 0xffffffff);
	gpio_write_int(gpio3100, 0xffffffff);
#else
	gpio_write_int(gpio2300, 0xffffffff);
#endif
	sleep(3);

	//turn on all LEDs
#if defined (CONFIG_RALINK_RT3052)
	gpio_write_int(gpio5140, 0);
	gpio_write_int(gpio3924, 0);
	gpio_write_int(gpio2300, 0);
#elif defined (CONFIG_RALINK_RT3883)
	gpio_write_int(gpio9572, 0);
	gpio_write_int(gpio7140, 0);
	gpio_write_int(gpio3924, 0);
	gpio_write_int(gpio2300, 0);
#elif defined (RALINK_GPIO_HAS_3352)
	gpio_write_int(gpio4540, 0);
	gpio_write_int(gpio3924, 0);
	gpio_write_int(gpio2300, 0);
#elif defined (CONFIG_RALINK_RT5350)
	gpio_write_int(gpio2722, 0);
	gpio_write_int(gpio2100, 0);
#elif defined (CONFIG_RALINK_MT7620)
	gpio_write_int(gpio72, 0);
	gpio_write_int(gpio7140, 0);
	gpio_write_int(gpio3924, 0);
	gpio_write_int(gpio2300, 0);
#elif defined (CONFIG_RALINK_MT7621)
	gpio_write_int(gpio9564, 0);
	gpio_write_int(gpio6332, 0);
	gpio_write_int(gpio3100, 0);
#elif defined (CONFIG_RALINK_MT7628)
	gpio_write_int(gpio9564, 0);
	gpio_write_int(gpio6332, 0);
	gpio_write_int(gpio3100, 0);
#else
	gpio_write_int(gpio2300, 0);
#endif
}

void gpio_test_read_g(int gpio_number)
{
	int d, value0, value1, value2;
#if defined (CONFIG_RALINK_MT7620)
	int value3;
#endif
#if defined (CONFIG_RALINK_RT3052)
	gpio_set_dir(gpio5140, gpio_in);
	gpio_read_int(gpio5140, &d);
	printf("gpio 51~40 = 0x%x\n", d);

	gpio_set_dir(gpio3924, gpio_in);
	gpio_read_int(gpio3924, &d);
	printf("gpio 39~24 = 0x%x\n", d);

	gpio_set_dir(gpio2300, gpio_in);
	gpio_read_int(gpio2300, &d);
	printf("gpio 23~00 = 0x%x\n", d);
#elif defined (CONFIG_RALINK_RT3883)
	gpio_set_dir(gpio9572, gpio_in);
	gpio_read_int(gpio9572, &d);
	printf("gpio 95~72 = 0x%x\n", d);

	gpio_set_dir(gpio7140, gpio_in);
	gpio_read_int(gpio7140, &d);
	printf("gpio 71~40 = 0x%x\n", d);

	gpio_set_dir(gpio3924, gpio_in);
	gpio_read_int(gpio3924, &d);
	printf("gpio 39~24 = 0x%x\n", d);

	gpio_set_dir(gpio2300, gpio_in);
	gpio_read_int(gpio2300, &d);
	printf("gpio 23~00 = 0x%x\n", d);
#elif defined (CONFIG_RALINK_RT3352)
	gpio_set_dir(gpio4540, gpio_in);
	gpio_read_int(gpio4540, &d);
	printf("gpio 45~40 = 0x%x\n", d);

	gpio_set_dir(gpio3924, gpio_in);
	gpio_read_int(gpio3924, &d);
	printf("gpio 39~24 = 0x%x\n", d);

	gpio_set_dir(gpio2300, gpio_in);
	gpio_read_int(gpio2300, &d);
	printf("gpio 23~00 = 0x%x\n", d);
#elif defined (CONFIG_RALINK_RT5350)
	gpio_set_dir(gpio2722, gpio_in);
	gpio_read_int(gpio2722, &d);
	printf("gpio 27~22 = 0x%x\n", d);

	gpio_set_dir(gpio2100, gpio_in);
	gpio_read_int(gpio2100, &d);
	printf("gpio 21~00 = 0x%x\n", d);
#elif defined (CONFIG_RALINK_MT7620)
  if(gpio_number == 72){
		gpio_set_dir_g(gpio72, gpio_in, gpio_number);
		gpio_read_int(gpio72, &d);
		value0 = d;
	}
	if(gpio_number >= 40 && gpio_number <=71 ){
		gpio_set_dir_g(gpio7140, gpio_in, gpio_number);
		gpio_read_int(gpio7140, &d);
		value1 = d;
	}
	if(gpio_number >= 24 && gpio_number <=39 ){
		gpio_set_dir_g(gpio3924, gpio_in, gpio_number);
		gpio_read_int(gpio3924, &d);
		value2 = d;
	}
	if(gpio_number <= 23){
		gpio_set_dir_g(gpio2300, gpio_in, gpio_number);
		gpio_read_int(gpio2300, &d);
		value3 = d;
	}
	if(gpio_number == 72 ){

		printf("gpio %d = %d\n", gpio_number, (value0 & (1 << (gpio_number-72))) >> (gpio_number - 72  ));
	}
	if(gpio_number >= 40 && gpio_number <=71 ){

		printf("gpio %d = %d\n", gpio_number, (value1 & (1 << (gpio_number-40))) >> (gpio_number - 40)  );
	}
	if(gpio_number >= 24 && gpio_number <=39 ){

		printf("gpio %d = %d\n", gpio_number, (value2 & (1 << (gpio_number-24))) >> (gpio_number - 24)  );
	}
	if(gpio_number <= 23){

		printf("gpio %d = %d\n", gpio_number, (value3 & (1 << gpio_number)) >> (gpio_number));
	}
	 if(gpio_number > 72 ){

		printf("MT7620 just have 72 GPIO pin\n");
	}

#elif defined (CONFIG_RALINK_MT7621)
	if(gpio_number >= 64 && gpio_number <=95 ){
		gpio_set_dir_g(gpio9564, gpio_in, gpio_number);
		gpio_read_int(gpio9564, &d);
		value0 = d;
	}
	if(gpio_number >= 32 && gpio_number <=63 ){
		gpio_set_dir_g(gpio6332, gpio_in, gpio_number);
		gpio_read_int(gpio6332, &d);
		value1 = d;
	}
	if(gpio_number <= 31){
		gpio_set_dir_g(gpio3100, gpio_in, gpio_number);
		gpio_read_int(gpio3100, &d);
		value2 = d;
	}
	if(gpio_number >= 64 && gpio_number <=95 ){

		printf("gpio %d = %d\n", gpio_number, (value0 & (1 << (gpio_number - 64))) >> (gpio_number - 64)  );
	}
	if(gpio_number >= 32 && gpio_number <=63 ){

		printf("gpio %d = %d\n", gpio_number, (value1 & (1 << (gpio_number - 32))) >> (gpio_number - 32)  );
	}
	if(gpio_number <= 31){

		printf("gpio %d = %d\n", gpio_number, (value2 & (1 << gpio_number)) >> (gpio_number));
	}
	if(gpio_number > 95 ){

		printf("MT7621 just have 95 GPIO pin\n");
	}

#elif defined (CONFIG_RALINK_MT7628)
	if(gpio_number >= 64 && gpio_number <=95 ){
		gpio_set_dir_g(gpio9564, gpio_in, gpio_number);
		gpio_read_int(gpio9564, &d);
		value0 = d;
	}
	if(gpio_number >= 32 && gpio_number <=63 ){
		gpio_set_dir_g(gpio6332, gpio_in, gpio_number);
		gpio_read_int(gpio6332, &d);
		value1 = d;
	}
	if(gpio_number <= 31){
		gpio_set_dir_g(gpio3100, gpio_in, gpio_number);
		gpio_read_int(gpio3100, &d);
		value2 = d;
	}

	if(gpio_number >= 64 && gpio_number <=95 ){

		printf("gpio %d = %d\n", gpio_number, (value0 & (1 << (gpio_number - 64))) >> (gpio_number - 64)  );
	}
	if(gpio_number >= 32 && gpio_number <=63 ){

		printf("gpio %d = %d\n", gpio_number, (value1 & (1 << (gpio_number - 32))) >> (gpio_number - 32)  );
	}
	if(gpio_number <= 31){

		printf("gpio %d = %d\n", gpio_number, (value2 & (1 << gpio_number)) >> (gpio_number));
	}
	if(gpio_number > 95 ){

		printf("MT7628 just have 95 GPIO pin\n");
	}
#else
	gpio_set_dir(gpio2300, gpio_in);
	gpio_read_int(gpio2300, &d);
	printf("gpio 23~00 = 0x%x\n", d);
#endif
}
void gpio_test_read(void)
{
	int d;

#if defined (CONFIG_RALINK_RT3052)
	gpio_set_dir(gpio5140, gpio_in);
	gpio_read_int(gpio5140, &d);
	printf("gpio 51~40 = 0x%x\n", d);

	gpio_set_dir(gpio3924, gpio_in);
	gpio_read_int(gpio3924, &d);
	printf("gpio 39~24 = 0x%x\n", d);

	gpio_set_dir(gpio2300, gpio_in);
	gpio_read_int(gpio2300, &d);
	printf("gpio 23~00 = 0x%x\n", d);
#elif defined (CONFIG_RALINK_RT3883)
	gpio_set_dir(gpio9572, gpio_in);
	gpio_read_int(gpio9572, &d);
	printf("gpio 95~72 = 0x%x\n", d);

	gpio_set_dir(gpio7140, gpio_in);
	gpio_read_int(gpio7140, &d);
	printf("gpio 71~40 = 0x%x\n", d);

	gpio_set_dir(gpio3924, gpio_in);
	gpio_read_int(gpio3924, &d);
	printf("gpio 39~24 = 0x%x\n", d);

	gpio_set_dir(gpio2300, gpio_in);
	gpio_read_int(gpio2300, &d);
	printf("gpio 23~00 = 0x%x\n", d);
#elif defined (CONFIG_RALINK_RT3352)
	gpio_set_dir(gpio4540, gpio_in);
	gpio_read_int(gpio4540, &d);
	printf("gpio 45~40 = 0x%x\n", d);

	gpio_set_dir(gpio3924, gpio_in);
	gpio_read_int(gpio3924, &d);
	printf("gpio 39~24 = 0x%x\n", d);

	gpio_set_dir(gpio2300, gpio_in);
	gpio_read_int(gpio2300, &d);
	printf("gpio 23~00 = 0x%x\n", d);
#elif defined (CONFIG_RALINK_RT5350)
	gpio_set_dir(gpio2722, gpio_in);
	gpio_read_int(gpio2722, &d);
	printf("gpio 27~22 = 0x%x\n", d);

	gpio_set_dir(gpio2100, gpio_in);
	gpio_read_int(gpio2100, &d);
	printf("gpio 21~00 = 0x%x\n", d);
#elif defined (CONFIG_RALINK_MT7620)
	gpio_set_dir(gpio72, gpio_in);
	gpio_read_int(gpio72, &d);
	printf("gpio 72 = 0x%x\n", d);

	gpio_set_dir(gpio7140, gpio_in);
	gpio_read_int(gpio7140, &d);
	printf("gpio 71~40 = 0x%x\n", d);

	gpio_set_dir(gpio3924, gpio_in);
	gpio_read_int(gpio3924, &d);
	printf("gpio 39~24 = 0x%x\n", d);

	gpio_set_dir(gpio2300, gpio_in);
	gpio_read_int(gpio2300, &d);
	printf("gpio 23~00 = 0x%x\n", d);
#elif defined (CONFIG_RALINK_MT7621)
	gpio_set_dir(gpio9564, gpio_in);
	gpio_read_int(gpio9564, &d);
	printf("gpio 95~64 = 0x%x\n", d);

	gpio_set_dir(gpio6332, gpio_in);
	gpio_read_int(gpio6332, &d);
	printf("gpio 63~32 = 0x%x\n", d);

	gpio_set_dir(gpio3100, gpio_in);
	gpio_read_int(gpio3100, &d);
	printf("gpio 31~00 = 0x%x\n", d);
#elif defined (CONFIG_RALINK_MT7628)
	gpio_set_dir(gpio9564, gpio_in);
	gpio_read_int(gpio9564, &d);
	printf("gpio 95~64 = 0x%x\n", d);

	gpio_set_dir(gpio6332, gpio_in);
	gpio_read_int(gpio6332, &d);
	printf("gpio 63~32 = 0x%x\n", d);

	gpio_set_dir(gpio3100, gpio_in);
	gpio_read_int(gpio3100, &d);
	printf("gpio 31~00 = 0x%x\n", d);
#else
	gpio_set_dir(gpio2300, gpio_in);
	gpio_read_int(gpio2300, &d);
	printf("gpio 23~00 = 0x%x\n", d);
#endif
}

void signal_handler(int signum)
{
	printf("gpio tester: signal ");
	if (signum == SIGUSR1)
		printf("SIGUSR1");
	else if (signum == SIGUSR2)
		printf("SIGUSR2");
	else
		printf("%d", signum);
	printf(" received\n");
}

void gpio_test_intr(int gpio_num)
{
	//set gpio direction to input
#if defined (CONFIG_RALINK_RT3052)
	gpio_set_dir(gpio5140, gpio_in);
	gpio_set_dir(gpio3924, gpio_in);
	gpio_set_dir(gpio2300, gpio_in);
#elif defined (CONFIG_RALINK_RT3352)
	gpio_set_dir(gpio4540, gpio_in);
	gpio_set_dir(gpio3924, gpio_in);
	gpio_set_dir(gpio2300, gpio_in);
#elif defined (CONFIG_RALINK_RT3883)
	gpio_set_dir(gpio9572, gpio_in);
	gpio_set_dir(gpio7140, gpio_in);
	gpio_set_dir(gpio3924, gpio_in);
	gpio_set_dir(gpio2300, gpio_in);
#elif defined (CONFIG_RALINK_RT5350)
	gpio_set_dir(gpio2722, gpio_in);
	gpio_set_dir(gpio2100, gpio_in);
#elif defined (CONFIG_RALINK_MT7620)
	gpio_set_dir(gpio72, gpio_in);
	gpio_set_dir(gpio7140, gpio_in);
	gpio_set_dir(gpio3924, gpio_in);
	gpio_set_dir(gpio2300, gpio_in);
#elif defined (CONFIG_RALINK_MT7621)
	gpio_set_dir(gpio9564, gpio_in);
	gpio_set_dir(gpio6332, gpio_in);
	gpio_set_dir(gpio3100, gpio_in);
#elif defined (CONFIG_RALINK_MT7628)
	gpio_set_dir(gpio9564, gpio_in);
	gpio_set_dir(gpio6332, gpio_in);
	gpio_set_dir(gpio3100, gpio_in);
#else
	gpio_set_dir(gpio2300, gpio_in);
#endif

	//enable gpio interrupt
	gpio_enb_irq();

	//register my information
	gpio_reg_info(gpio_num);

	//issue a handler to handle SIGUSR1
	signal(SIGUSR1, signal_handler);
	signal(SIGUSR2, signal_handler);

	//wait for signal
	pause();

	//disable gpio interrupt
	gpio_dis_irq();
}

void gpio_set_led(int argc, char *argv[])
{
	int fd;
	ralink_gpio_led_info led;

	led.gpio = atoi(argv[2]);
	if (led.gpio < 0 || led.gpio >= RALINK_GPIO_NUMBER) {
		printf("gpio number %d out of range (should be 0 ~ %d)\n", led.gpio, RALINK_GPIO_NUMBER);
		return;
	}
	led.on = (unsigned int)atoi(argv[3]);
	if (led.on > RALINK_GPIO_LED_INFINITY) {
		printf("on interval %d out of range (should be 0 ~ %d)\n", led.on, RALINK_GPIO_LED_INFINITY);
		return;
	}
	led.off = (unsigned int)atoi(argv[4]);
	if (led.off > RALINK_GPIO_LED_INFINITY) {
		printf("off interval %d out of range (should be 0 ~ %d)\n", led.off, RALINK_GPIO_LED_INFINITY);
		return;
	}
	led.blinks = (unsigned int)atoi(argv[5]);
	if (led.blinks > RALINK_GPIO_LED_INFINITY) {
		printf("number of blinking cycles %d out of range (should be 0 ~ %d)\n", led.blinks, RALINK_GPIO_LED_INFINITY);
		return;
	}
	led.rests = (unsigned int)atoi(argv[6]);
	if (led.rests > RALINK_GPIO_LED_INFINITY) {
		printf("number of resting cycles %d out of range (should be 0 ~ %d)\n", led.rests, RALINK_GPIO_LED_INFINITY);
		return;
	}
	led.times = (unsigned int)atoi(argv[7]);
	if (led.times > RALINK_GPIO_LED_INFINITY) {
		printf("times of blinking %d out of range (should be 0 ~ %d)\n", led.times, RALINK_GPIO_LED_INFINITY);
		return;
	}

	fd = open(GPIO_DEV, O_RDONLY);
	if (fd < 0) {
		perror(GPIO_DEV);
		return;
	}
	if (ioctl(fd, RALINK_GPIO_LED_SET, &led) < 0) {
		perror("ioctl");
		close(fd);
		return;
	}
	close(fd);
}

void usage(char *cmd)
{
	printf("Usage: %s w - writing test (output)\n", cmd);
	printf("       %s r - reading test (input)\n", cmd);
	printf("       %s g (<gpio>)- reading gpio number (input)\n", cmd);
	printf("       %s i (<gpio>) - interrupt test for gpio number\n", cmd);
	printf("       %s l <gpio> <on> <off> <blinks> <rests> <times>\n", cmd);
	printf("            - set led on <gpio>(0~24) on/off interval, no. of blinking/resting cycles, times of blinking\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		usage(argv[0]);

	switch (argv[1][0]) {
	case 'w':
		gpio_test_write();
		break;
	case 'r':
		gpio_test_read();
		break;
	case 'g':
		if (argc != 3){
			usage(argv[0]);
		}
		gpio_test_read_g(atoi(argv[2]));
		break;
	case 'i':
		if (argc == 3)
			gpio_test_intr(atoi(argv[2]));
		else
			gpio_test_intr(0);
		break;
	case 'l':
		if (argc != 8)
			usage(argv[0]);
		gpio_set_led(argc, argv);
		break;
	default:
		usage(argv[0]);
	}

	return 0;
}
