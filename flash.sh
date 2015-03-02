#!/bin/sh

ifconfig eth0 192.168.1.131

tftp -4 -m binary 192.168.1.1 -c put wive.bin
