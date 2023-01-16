

SUBDIRS = dox etc examples inc perl src

# Install the pkg-config file; the directory is set using
# PKG_INSTALLDIR in configure.ac.
pkgconfig_DATA = libsvarog.pc


EXTRA_DIST = Doxyfile

documents:
	doxygen && cp dox/www_perkun_org.html_files/*.png doc/html
	
