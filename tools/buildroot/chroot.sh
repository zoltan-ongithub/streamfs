mount -t proc /proc root/proc/
mount --rbind /sys root/sys/
mount --rbind /dev root/dev/
mount --rbind /mnt root/mnt
mkdir -p root/run/log
mount --rbind /run/log root/run/log

chroot root

umount root/run/log
umount root/mnt
umount root/dev/
umount root/sys/
umount root/proc/
