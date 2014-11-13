Ralink APSoC SDK3.2:

1. We change cross-compiler to gcc3.4.2 to support firmware optimization feature. 
   please decompress RT288x_SDK/toolchain/buildrot-gcc342.tar.bz2 to /opt/buildroot-gcc342

2. please install mksquashfs and lzma_alone again.

	#cd RT288x_SDK/toolchain/mksquash_lzma-3.2
	#make
	#make install (install mksquashfs-lzma & lzma_alone to /opt/buildroot-gcc342/bin)

3. In SDK3.2, you can use the same cross-compiler to build uboot&linux image
   => mips4KCle-gcc is OBSOLETE


