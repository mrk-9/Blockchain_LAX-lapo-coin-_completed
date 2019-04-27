
Debian
====================
This directory contains files used to package lapod/lapo-qt
for Debian-based Linux systems. If you compile lapod/lapo-qt yourself, there are some useful files here.

## lapo: URI support ##


lapo-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install lapo-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your lapoqt binary to `/usr/bin`
and the `../../share/pixmaps/lapo128.png` to `/usr/share/pixmaps`

lapo-qt.protocol (KDE)

