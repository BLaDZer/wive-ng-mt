#!/bin/bash

make clean; make STATIC=true bcm947xx_ddwrt

make clean; make STATIC=true bcm947xx

make clean; make STATIC=true ar71xx

make clean; make STATIC=true ar231x

make clean
