Name: openocd
Version: 0.10.0
Release: alt1.git2dcf7bf
Summary: Debugging, in-system programming and boundary-scan testing for embedded devices

Group: Development/Tools
License: GPLv2
Url: http://sourceforge.net/projects/openocd
Source: %name-%version.tar
Source10: git2cl.tar
Source20: jimtcl.tar
Patch1: fsl-add-k22-k24-k64-flash-support.patch

BuildRequires: libftdi-devel

%description
The Open On-Chip Debugger (OpenOCD) provides debugging, in-system
programming and boundary-scan testing for embedded devices. Various
different boards, targets, and interfaces are supported to ease
development time.

Install OpenOCD if you are looking for an open source solution for
hardware debugging.

%prep
%setup
tar -xf %SOURCE10 -C tools
tar -xf %SOURCE20
%patch1 -p1

%build
%autoreconf
%configure \
  --disable-werror \
  --enable-ftdi \
  #
%make_build

%install
make install DESTDIR=%buildroot INSTALL="install -p"
%makeinstall_std

%files
%doc README COPYING AUTHORS ChangeLog NEWS TODO
%doc %_datadir/%name/contrib
%dir %_datadir/%name
%_datadir/%name/scripts
%_datadir/%name/OpenULINK
%_bindir/%name
%_infodir/%name.info*.gz
%_mandir/man1/*

%changelog
* Mon Aug 17 2015 Pavel Nakonechny <pavel.nakonechny@skitlab.ru> 0.10.0-alt1.git2dcf7bf
- Initial build.
