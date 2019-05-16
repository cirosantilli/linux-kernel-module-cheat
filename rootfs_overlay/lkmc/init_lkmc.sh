#!/bin/sh

# This is an attempt to replace /etc/inittab with a single shell file,
# in order to to make everything explicit and sane.
#
# The rationale is that it will make it easier to port our setup to any image
# if our minimal init is contained in a single portable file.
#
# Of course, packages that rely on extra init may start failing at any
# point with this minimized setup, but this is a risk worth taking, especially
# because gem5 is slow and basically mandates a minimal init.
#
# Here we try to some basic common init tasks that will cover what the large
# majority of software will need.
#
# We don't care about shutdown commands since we focus on stateless disks.

export PATH=/bin:/sbin:/usr/bin:/usr/sbin
mount -t proc proc /proc
mkdir -p /dev/pts
mkdir -p /dev/shm
# TODO do mounts direclty here. fstab only adds the fschk as extra functionality,
# but we don't care since our disks are stateless.
mount -a
login=/tmp/login
printf '#!/bin/sh
exec /bin/login root
' > "$login"
chmod +x "$login"
exec /sbin/getty -n -L -l "$login" console 0 vt100
