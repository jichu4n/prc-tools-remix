Name: prc-tools
%define version 2.1
Version: %{version}
Release: 1
Summary: GCC and related tools for Palm OS development
Copyright: GPL
URL: http://www.palm.com/devzone/tools/gcc/
Group: Development/Tools
Source0: http://www.palm.com/devzone/tools/gcc/dist/prc-tools-%{version}.tar.gz
Source1: ftp://sourceware.cygnus.com/pub/binutils/releases/binutils-2.9.1.tar.gz
Source2: ftp://sourceware.cygnus.com/pub/gdb/releases/gdb-4.18.tar.gz
Source3: ftp://gcc.gnu.org/pub/gcc/releases/gcc-2.95.2/gcc-2.95.2.tar.gz
Source4: ftp://ftp.gnu.org/pub/gnu/make/make-3.77.tar.gz
NoSource: 1 2 3 4
%ifos cygwin
# We don't have find-requires/find-provides scripts for Cygwin
AutoReqProv: no
%endif

%ifnos cygwin
%define prefix /usr/local
%define exec_prefix %{prefix}
%define palmdev_prefix /usr/local/palmdev
%define exeext
%else
%define prefix /prc-tools
# If you want to install into the Cygwin directory, use this one:
#%define prefix /cygnus/cygwin-b20
%define exec_prefix %{prefix}/H-i586-cygwin32
%define palmdev_prefix /PalmDev
%define exeext .exe
%endif

# The target used to be `m68k-palmos-coff'.  Some people may want to leave
# it thus to avoid changing their makefiles a little bit.
%define target m68k-palmos

%description
A complete compiler tool chain for building Palm OS applications in C or C++.
Includes (patched versions of) binutils 2.9.1, gdb 4.18, and GCC 2.95.2, along
with various post-linker tools to produce Palm OS .prc files.

You will also need a Palm OS SDK and some way of creating resources, such as
PilRC.

%package htmldocs
Summary: GCC, binutils, gdb, and general prc-tools documentation in HTML format
Group: Development/Tools
%description htmldocs
GCC, binutils, gdb, and general prc-tools documentation in HTML format

%prep
%setup -n binutils-2.9.1 -T -b 1
%setup -n gdb-4.18 -T -b 2
%setup -n gcc-2.95.2 -T -b 3
%setup -n make-3.77 -T -b 4
%setup

cat *.palmos.diff | (cd .. && patch -p0)
%ifos cygwin
cat *.cygwin.diff | (cd .. && patch -p0)
%endif

mv ../binutils-2.9.1 binutils
mv ../gdb-4.18 gdb
mv ../gcc-2.95.2 gcc
mv ../make-3.77 make

%ifos cygwin
# Convert line endings -- important because some files get installed as is
canon -lr .
%endif

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
  --sharedstatedir=%{palmdev_prefix}

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
strip build-prc%{exeext} multigen%{exeext} obj-res%{exeext} %{target}-*%{exeext}
cd %{exec_prefix}/lib/gcc-lib/%{target}/2.95.2-kgpd
strip cc1%{exeext} cc1plus%{exeext} collect2%{exeext} cpp%{exeext}
cd %{exec_prefix}/%{target}/bin
strip *%{exeext}

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
%{exec_prefix}/bin/build-prc%{exeext}
%{exec_prefix}/bin/multigen%{exeext}
%{exec_prefix}/bin/obj-res%{exeext}
%{exec_prefix}/bin/%{target}-obj-res%{exeext}
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
%ifnos cygwin
%doc %{prefix}/info/as.info*
%doc %{prefix}/info/binutils.info*
%doc %{prefix}/info/cpp.info*
%doc %{prefix}/info/gasp.info*
%doc %{prefix}/info/gcc.info*
%doc %{prefix}/info/gdb.info*
%doc %{prefix}/info/ld.info*
%doc %{prefix}/info/prc-tools.info*
%endif
%doc %{prefix}/man/man1/%{target}-*.1

# support stuff tucked away in GCC's directories
%{exec_prefix}/lib/gcc-lib/%{target}/2.95.2-kgpd/[A-o]*
%config %{exec_prefix}/lib/gcc-lib/%{target}/2.95.2-kgpd/pilot.ld
%config %{exec_prefix}/lib/gcc-lib/%{target}/2.95.2-kgpd/specs
%{exec_prefix}/%{target}/bin/
%{exec_prefix}/%{target}/include/
%{exec_prefix}/%{target}/lib/
# but not .../sys-include

# PalmDev framework
%dir %{palmdev_prefix}
%dir %{palmdev_prefix}/include
%dir %{palmdev_prefix}/lib
# yes, this is intentionally not %{target}
%dir %{palmdev_prefix}/lib/m68k-palmos-coff

%docdir %{palmdev_prefix}/doc
%dir %{palmdev_prefix}/doc
%doc %{palmdev_prefix}/doc/COPYING
%doc %{palmdev_prefix}/doc/README

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
