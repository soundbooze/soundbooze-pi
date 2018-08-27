1. https://github.com/torvalds/linux/blob/master/Documentation/kbuild/modules.txt

```
# git clone --depth=1 -b rpi-4.14.y-rt https://github.com/raspberrypi/linux
```

<pre>
/* manual patches
wget https://www.kernel.org/pub/linux/kernel/projects/rt/*

$ gunzip patch*.gz
$ cd linux
$ cat patch* | patch -p1
*/
</pre>

# Adjust 

<pre>

according to your /proc/cpuinfo hardware

```
# make bcm2709_defconfig
# make menuconfig
```

</pre>

<pre>

system type
============
broadcom Soc 2835

| General Setup | --- | Timer Subsystem | -- | High Res Timer |
| Kernel Features | -- | Preemption | -- | Full |
| Timer Freq | -- | 250-1000 | (cari yang stabil)

| Floating Point Emulation | - | * |

device driver: sound card support - usb sound devices - behringer
// device driver: device tree*
// Kernel hacking: debug preempt kernel disable
</pre>

<pre>
<b>FTRACE</b>
<b>BPF</b>
</pre>

```
# make -j4 zImage modules dtbs
# make modules_install
# cp arch/arm/boot/dts/*.dtb /boot/
# cp arch/arm/boot/dts/overlays/*.dtb* /boot/overlays/
# cp arch/arm/boot/dts/overlays/README /boot/overlays/
# cp arch/arm/boot/zImage /boot/kernel-rt.img
```

# Boot

```
# vim /boot/config.txt
kernel=kernel-rt.img
```

# Clean

```
# make clean
# make mrproper
```
# CROSS-COMPILE

-- 32bit kernel7 (enable: EARM EABI - kernel features)

``` 
# export KERNEL=kernel7
# export ARCH=arm
# export CROSS_COMPILE=arm-none-eabi-
# export CONCURRENCY_LEVEL=4
```

#### ln -sf /lib/modules {source, build}

```
# make INSTALL_MOD_PATH=/frodo modules_install
```

# Reboot && Check

```
# cat /sys/kernel/realtime
# uname -a
```

# FTRACE

```
mount -t debugfs nodev /sys/kernel/debug
echo 1 > /proc/sys/kernel/stack_tracer_enabled
/debug/tracing # cat stack_trace

```

1. https://git.kernel.org/pub/scm/linux/kernel/git/rostedt/trace-cmd.git/tree/
2. https://github.com/brendangregg/perf-tools
3. https://github.com/iovisor/bcc
4. https://github.com/torvalds/linux/tree/master/tools/perf

## Notes

https://github.com/umiddelb/armhf/wiki/How-To-compile-a-custom-Linux-kernel-for-your-ARM-device
https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/preemptrt_setup
https://wiki.linuxfoundation.org/realtime/documentation/howto/applications/application_base

## Tools

https://wiki.linuxfoundation.org/realtime/documentation/howto/tools/worstcaselatency
https://wiki.linuxfoundation.org/realtime/documentation/howto/tools/cyclictest

## Compiler

https://www.solarianprogrammer.com/2017/12/07/raspberry-pi-raspbian-compiling-gcc/
http://www.linuxfromscratch.org/blfs/view/svn/general/llvm.html

<pre>
 -DLLVM_TARGETS_TO_BUILD="host;ARM" \
</pre>
