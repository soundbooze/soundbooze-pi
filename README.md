# Recovery

in windows sdcard

```javascript
cmdline.txt - add " init=/bin/bash" 
```

# remount /boot

```javascript
mount -t vfat -o rw /dev/mmcblk0p1 /boot
```

# remount read-write /

```javascript
mount -o remount, rw /
```

# wifi boot problem

```javascript
/boot/config.txt

dtoverlay=pi3-disable-wifi
dtoverlay=pi3-disable-bt
```
 
# GL

1. https://github.com/peepo/openGL-RPi-tutorial
2. https://github.com/benosteen/opengles-book-samples
3. https://github.com/andrecurvello/raspberry-gl

# Driver

1. https://www.waveshare.com/wiki/5inch_HDMI_LCD?amazon

# pilfs

http://intestinate.com/pilfs/
