
** Download **

* Get buildroot from here and unpack it
  https://buildroot.org/downloads/buildroot-2021.02.6.tar.gz

  or unpack a prebuilt from here:

  https://www.dropbox.com/s/9lkaxam2pv13iv3/rootfs.tar?dl=0

* Copy the .config file to the unpacked folder and run

```
  $ make -j12
```

* Unpack the ./output/images/rootfs.tar file on the device to /userdata/root folder.

* Copy the chroot.sh scrip to /userdata/ on the device and execute it
