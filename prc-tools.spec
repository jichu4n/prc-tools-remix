Name: prc-tools
# The version line is grepped for by configure.  It must be exactly
# Version<colon><space><versionnumber><newline>
Version: 2.2.90
Release: 1
Summary: GCC and related tools for Palm OS development
License: GPL
URL: http://prc-tools.sourceforge.net/
Group: Development/Palm OS
Source0: http://prdownloads.sourceforge.net/prc-tools/%{name}-%{version}.tar.gz
Source1: ftp://ftp.gnu.org/pub/gnu/binutils/binutils-2.13.2.1.tar.bz2
Source2: ftp://gcc.gnu.org/pub/gcc/releases/gcc-2.95.3/gcc-2.95.3.tar.gz
Source3: ftp://gcc.gnu.org/pub/gcc/releases/gcc-3.2.2/gcc-3.2.2.tar.bz2
Source4: ftp://sources.redhat.com/pub/gdb/releases/gdb-5.3.tar.bz2
Source5: ftp://ftp.gnu.org/pub/gnu/make/make-3.80.tar.bz2
NoSource: 1
NoSource: 2
NoSource: 3
NoSource: 4
NoSource: 5
BuildRoot: %{_tmppath}/%{name}-root
BuildRequires: texinfo

# This is the canonical place to look for Palm OS-related header files and
# such on Unix-like file systems.
%define palmdev_prefix /opt/palmdev

%description
A complete compiler tool chain for building Palm OS applications in C or C++.
Includes (patched versions of) binutils 2.13.2.1, GCC 2.95.3, and GDB 5.3,
along with various post-linker tools to produce Palm OS .prc files.

You will also need a Palm OS SDK and some way of creating resources, such as
PilRC.

%package arm
Summary: GCC and related tools for ARM targeted Palm OS development
Group: Development/Palm OS
Requires: prc-tools >= 2.2
%description arm
A compiler tool chain for building Palm OS armlets in C or C++.
Includes (patched versions of) binutils 2.13.2.1 and GCC 3.2.2, and requires
the various post-linker tools from a corresponding prc-tools package.

Note that this version of ARM prc-tools does not provide startup code or
other niceties:  by itself, it is only useful for building stand-alone
code resources such as armlets.

%package htmldocs
Summary: GCC, GDB, binutils, make, and prc-tools documentation as HTML
Group: Development/Palm OS
Prefix: %{palmdev_prefix}
%description htmldocs
GCC, GDB, binutils, make, and general prc-tools documentation in HTML
format.  The various native development packages and the main prc-tools
package, respectively, provide exactly this documentation in info format.
This optional package is for those who prefer HTML-formatted documentation.

By default, this package will be installed at %{palmdev_prefix}/doc, and
you should point your web browser at %{palmdev_prefix}/doc/index.html.
If you want to install it elsewhere, you can do so via the prefix and/or
relocation facilities of your RPM installation tool.

%prep
%setup -q -T -b 1 -n binutils-2.13.2.1
%setup -q -T -b 2 -n gcc-2.95.3
%setup -q -T -b 3 -n gcc-3.2.2
%setup -q -T -b 4 -n gdb-5.3
%setup -q -T -b 5 -n make-3.80
%setup -q

cat *.palmos.diff | (cd .. && patch -p0)

mv ../binutils-2.13.2.1 binutils
mv ../gcc-2.95.3 gcc295
mv ../gcc-3.2.2 gcc
mv ../gdb-5.3 gdb
mv ../make-3.80 make

# The patch touches a file this depends on, and you need autoconf to remake
# it.  There's no changes, so let's just touch it so people don't have to
# have autoconf installed.
touch gcc295/gcc/cstamp-h.in

mkdir static-libs

%build
# Ensure that we link *statically* against the stdc++ library
rm -f static-libs/*
ln -s `${CXX:-g++} -print-file-name=libstdc++.a` static-libs/libstdc++.a

# The m68k target used to be 'm68k-palmos-coff'.  Some people may want to
# leave it thus to avoid changing their makefiles a little bit.

# We can't use %%configure because it insists on libtoolizing, which will
# likely break our config.sub.
LDFLAGS=-L`pwd`/static-libs ./configure \
  --enable-targets=m68k-palmos,arm-palmos \
  --enable-languages=c,c++ \
  --disable-cpp \
  --with-palmdev-prefix=%{palmdev_prefix} \
  --enable-html-docs=%{palmdev_prefix}/doc \
  --prefix=%{_prefix} --exec-prefix=%{_exec_prefix} \
  --bindir=%{_bindir} --sbindir=%{_sbindir} --libexecdir=%{_libexecdir} \
  --localstatedir=%{_localstatedir} --sharedstatedir=%{_sharedstatedir} \
  --sysconfdir=%{_sysconfdir} --datadir=%{_datadir} \
  --includedir=%{_includedir} --libdir=%{_libdir} \
  --mandir=%{_mandir} --infodir=%{_infodir}

%ifarch noarch
# For a noarch package, we want to avoid wasting time building and installing
# all the other directories.
%define make_opts  subdirs=doc
%else
%define make_opts  %{nil}
%endif

make %{make_opts}

%install
[ ${RPM_BUILD_ROOT:-/} != / ] && rm -rf $RPM_BUILD_ROOT
%makeinstall htmldir=$RPM_BUILD_ROOT%{palmdev_prefix}/doc %{make_opts}

%clean
[ ${RPM_BUILD_ROOT:-/} != / ] && rm -rf $RPM_BUILD_ROOT

%post
# Given foo.info, install-info will check for both foo.info and foo.info.gz
if /bin/sh -c 'install-info --version' >/dev/null 2>&1; then
  install-info --info-dir=%{_infodir} %{_infodir}/prc-tools.info
fi

%preun
if [ "$1" = 0 ]; then
  if /bin/sh -c 'install-info --version' >/dev/null 2>&1; then
    install-info --remove --info-dir=%{_infodir} %{_infodir}/prc-tools.info
  fi
  palmdev-prep --remove
fi

%preun arm
if [ "$1" = 0 ]; then
  rm -f %{_libdir}/gcc-lib/arm-palmos/specs
fi

%files
%defattr(-, root, root)
%ifnarch noarch
%{_bindir}/[b-z]*
%{_exec_prefix}/m68k*
%{_libdir}/gcc-lib/m68k*
%{_datadir}/prc-tools
%endif
# Native packages provide gcc.info* and gcc.1 etc, so we limit ourselves to
# these prc-tools-specific ones
%doc %{_infodir}/prc-tools*
%doc %{_mandir}/man1/build-prc*
%doc %{_mandir}/man1/palmdev-prep*

%doc COPYING README

%files arm
%defattr(-, root, root)
%ifnarch noarch
%{_bindir}/arm*
%{_exec_prefix}/arm*
%{_libdir}/gcc-lib/arm*
%endif

%files htmldocs
%defattr(-, root, root)
%doc %{palmdev_prefix}/doc
