Name: prc-tools
# The version line is grepped for by configure.  It must be exactly
# Version<colon><space><versionnumber><newline>
Version: 2.0.93
Release: 1
Summary: GCC and related tools for Palm OS development
License: GPL
URL: http://prc-tools.sourceforge.net/
Group: Development/Tools
Source0: http://prdownloads.sourceforge.net/prc-tools/%{name}-%{version}.tar.gz
Source1: ftp://sources.redhat.com/pub/binutils/releases/binutils-2.11.2.tar.gz
Source2: ftp://sources.redhat.com/pub/gdb/releases/gdb-5.0.tar.gz
Source3: ftp://gcc.gnu.org/pub/gcc/releases/gcc-2.95.3/gcc-2.95.3.tar.gz
Source4: ftp://ftp.gnu.org/pub/gnu/make/make-3.77.tar.gz
NoSource: 1
NoSource: 2
NoSource: 3
NoSource: 4

%define prefix /usr/local
%define exec_prefix %{prefix}
%define palmdev_prefix /opt/palmdev
%define exeext %{nil}

# The target used to be `m68k-palmos-coff'.  Some people may want to leave
# it thus to avoid changing their makefiles a little bit.
%define target m68k-palmos

%description
A complete compiler tool chain for building Palm OS applications in C or C++.
Includes (patched versions of) binutils 2.11.2, gdb 5.0, and GCC 2.95.3, along
with various post-linker tools to produce Palm OS .prc files.

You will also need a Palm OS SDK and some way of creating resources, such as
PilRC.

%package htmldocs
Summary: GCC, binutils, gdb, and general prc-tools documentation in HTML format
Group: Development/Tools
%description htmldocs
GCC, binutils, gdb, and general prc-tools documentation in HTML format

%prep
%setup -n binutils-2.11.2 -T -b 1
%setup -n gdb-5.0 -T -b 2
%setup -n gcc-2.95.3 -T -b 3
%setup -n make-3.77 -T -b 4
%setup

cat *.palmos.diff | (cd .. && patch -p0)

mv ../binutils-2.11.2 binutils
mv ../gdb-5.0 gdb
mv ../gcc-2.95.3 gcc
mv ../make-3.77 make

# The patch touches a file this depends on, and you need autoconf to remake
# it.  There's no changes, so let's just touch it so people don't have to
# have autoconf installed.
touch gcc/gcc/cstamp-h.in

cd $RPM_BUILD_DIR
rm -rf build-prc-tools
mkdir build-prc-tools
mkdir build-prc-tools/empty

%build
# The --with-headers bit is a nasty hack to try to make fixinc happy on
# Solaris and simultaneously stop it from doing anything.
cd $RPM_BUILD_DIR/build-prc-tools
../prc-tools-%{version}/configure --target=%{target} --enable-languages=c,c++ \
  --prefix=%{prefix} --exec-prefix=%{exec_prefix} \
  --with-headers=$RPM_BUILD_DIR/build-prc-tools/empty \
  --without-shared-libstdcxx-for-tools \
  --with-palmdev-prefix=%{palmdev_prefix} \
  --with-palmdev-extra-path=/usr/local/palmdev

cd $RPM_BUILD_DIR/build-prc-tools/doc
make html

%install
# Yes, we really are doing most of the build in the install script.  This is
# because we need binutils installed before we can build GCC, and we need GCC
# installed before we can build the target crt and lib stuff.  Probably we'll
# eventually tidy this up and do a proper unified binutils/gcc tree thing.
cd $RPM_BUILD_DIR/build-prc-tools
make all-install

cd %{exec_prefix}/bin
strip build-prc%{exeext} multigen%{exeext} stubgen%{exeext} obj-res%{exeext} \
%{target}-obj-res%{exeext} %{target}-addr2line%{exeext} %{target}-ar%{exeext} \
%{target}-as%{exeext} %{target}-c++%{exeext} %{target}-c++filt%{exeext} \
%{target}-cpp%{exeext} %{target}-g++%{exeext} %{target}-gasp%{exeext} \
%{target}-gcc%{exeext} %{target}-gdb%{exeext} %{target}-ld%{exeext} \
%{target}-nm%{exeext} %{target}-objcopy%{exeext} %{target}-objdump%{exeext} \
%{target}-protoize%{exeext} %{target}-ranlib%{exeext} %{target}-size%{exeext} \
%{target}-strings%{exeext} %{target}-strip%{exeext} \
%{target}-unprotoize%{exeext}
cd %{exec_prefix}/lib/gcc-lib/%{target}/2.95.3-kgpd
strip cc1%{exeext} cc1plus%{exeext} collect2%{exeext} cpp0%{exeext}
cd %{exec_prefix}/%{target}/bin
strip ar%{exeext} as%{exeext} gcc%{exeext} ld%{exeext} nm%{exeext} \
ranlib%{exeext} strip%{exeext}
cd %{exec_prefix}/%{target}/real-bin
strip cpp%{exeext} gcc%{exeext} g++%{exeext} c++%{exeext}

cd $RPM_BUILD_DIR/build-prc-tools/doc
make install-html

%post
if /bin/sh -c 'install-info --version' >/dev/null 2>&1; then
  # tidy up after bug in 2.0 spec file :-(
  if [ -f /usr/info/dir ] && grep prc-tools /usr/info/dir; then
    install-info --remove --info-dir=/usr/info %{prefix}/info/prc-tools.info
    true
  fi

  if [ -f %{prefix}/info/dir ]; then
    install-info --info-dir=%{prefix}/info %{prefix}/info/prc-tools.info
  fi
fi

%preun
if [ "$1" = 0 ]; then
  if /bin/sh -c 'install-info --version' >/dev/null 2>&1; then
    if [ -f %{prefix}/info/dir ]; then
      install-info --remove --info-dir=%{prefix}/info %{prefix}/info/prc-tools.info
    fi
  fi
fi

%files
# prc-tools-specific post-linker tools
%{exec_prefix}/bin/build-prc%{exeext}
%{exec_prefix}/bin/%{target}-multigen%{exeext}
%{exec_prefix}/bin/%{target}-obj-res%{exeext}
%{exec_prefix}/bin/%{target}-sdkfind%{exeext}
%{exec_prefix}/bin/%{target}-stubgen%{exeext}
# these unadorned names (here for compatibility) will stop being installed soon
%{exec_prefix}/bin/multigen%{exeext}
%{exec_prefix}/bin/obj-res%{exeext}
%{exec_prefix}/bin/stubgen%{exeext}

# generic binutils/gcc/gdb tools
%{exec_prefix}/bin/%{target}-addr2line%{exeext}
%{exec_prefix}/bin/%{target}-ar%{exeext}
%{exec_prefix}/bin/%{target}-as%{exeext}
%{exec_prefix}/bin/%{target}-c++%{exeext}
%{exec_prefix}/bin/%{target}-c++filt%{exeext}
%{exec_prefix}/bin/%{target}-cpp%{exeext}
%{exec_prefix}/bin/%{target}-g++%{exeext}
%{exec_prefix}/bin/%{target}-gasp%{exeext}
%{exec_prefix}/bin/%{target}-gcc%{exeext}
%{exec_prefix}/bin/%{target}-gdb%{exeext}
%{exec_prefix}/bin/%{target}-ld%{exeext}
%{exec_prefix}/bin/%{target}-nm%{exeext}
%{exec_prefix}/bin/%{target}-objcopy%{exeext}
%{exec_prefix}/bin/%{target}-objdump%{exeext}
%{exec_prefix}/bin/%{target}-protoize%{exeext}
%{exec_prefix}/bin/%{target}-ranlib%{exeext}
%{exec_prefix}/bin/%{target}-size%{exeext}
%{exec_prefix}/bin/%{target}-strings%{exeext}
%{exec_prefix}/bin/%{target}-strip%{exeext}
%{exec_prefix}/bin/%{target}-unprotoize%{exeext}

# info files and man pages
%doc %{prefix}/info/as.info*
%doc %{prefix}/info/binutils.info*
%doc %{prefix}/info/cpp.info*
%doc %{prefix}/info/gasp.info*
%doc %{prefix}/info/gcc.info*
%doc %{prefix}/info/gdb.info*
%doc %{prefix}/info/ld.info*
%doc %{prefix}/info/prc-tools.info*
%doc %{prefix}/man/man1/%{target}-addr2line.1*
%doc %{prefix}/man/man1/%{target}-ar.1*
%doc %{prefix}/man/man1/%{target}-as.1*
%doc %{prefix}/man/man1/%{target}-c++filt.1*
%doc %{prefix}/man/man1/%{target}-g++.1*
%doc %{prefix}/man/man1/%{target}-gcc.1*
%doc %{prefix}/man/man1/%{target}-gdb.1*
%doc %{prefix}/man/man1/%{target}-ld.1*
%doc %{prefix}/man/man1/%{target}-nlmconv.1*
%doc %{prefix}/man/man1/%{target}-nm.1*
%doc %{prefix}/man/man1/%{target}-objcopy.1*
%doc %{prefix}/man/man1/%{target}-objdump.1*
%doc %{prefix}/man/man1/%{target}-ranlib.1*
%doc %{prefix}/man/man1/%{target}-size.1*
%doc %{prefix}/man/man1/%{target}-strings.1*
%doc %{prefix}/man/man1/%{target}-strip.1*

# support stuff tucked away in GCC's directories
%{exec_prefix}/lib/gcc-lib/%{target}/2.95.3-kgpd/[A-o]*
%config %{exec_prefix}/lib/gcc-lib/%{target}/2.95.3-kgpd/pilot.ld
%config %{exec_prefix}/lib/gcc-lib/%{target}/2.95.3-kgpd/specs
%{exec_prefix}/%{target}/bin/
%{exec_prefix}/%{target}/include/
%{exec_prefix}/%{target}/lib/
%{exec_prefix}/%{target}/real-bin/
# but not .../sys-include

# PalmDev framework
%dir %{palmdev_prefix}
%dir %{palmdev_prefix}/include
%dir %{palmdev_prefix}/lib
# yes, this is intentionally not %{target}
%dir %{palmdev_prefix}/lib/m68k-palmos-coff
%docdir %{palmdev_prefix}/doc

%doc COPYING README

%files htmldocs
%doc %{palmdev_prefix}/doc/index.html
%doc %{palmdev_prefix}/doc/texi/as_*
%doc %{palmdev_prefix}/doc/texi/binutils_*
%doc %{palmdev_prefix}/doc/texi/cpp_*
%doc %{palmdev_prefix}/doc/texi/gcc_*
%doc %{palmdev_prefix}/doc/texi/gdb_*
%doc %{palmdev_prefix}/doc/texi/ld_*
%doc %{palmdev_prefix}/doc/texi/make_*
%doc %{palmdev_prefix}/doc/texi/prc-tools_*
