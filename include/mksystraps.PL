use strict;

if (@ARGV == 0) {
  print <<EOF;
Usage: $0 [-s] file...
Generates an annotated list of systraps from the named files.  Normally, the
output is suitable for the build-prc data file; with `-s', produces an asm
include file.
EOF
  exit;
  }

my $format;

if ($ARGV[0] eq "-s") {
  shift;
  $format = "\t.equ %s,%s\n";
  print <<EOF;
/* DO NOT EDIT!  This file was automatically produced
   by $0
   from @ARGV */

	.macro systrap trapname
	trap #15
	.word \\trapname
	.endm

EOF
  }
else {
  $format = "<%s>\t%s\n";
  print <<EOF;
DO NOT EDIT!  This file was automatically produced
by $0
from @ARGV

EOF
  }

my $num = 0;

while (<>) {
  s"//.*$"";		# eat C++-style comments
  s"/\*[^/]*\*/""g;	# eat simple C-style comments (just in case)

  if (/define\W+sysTrapBase\W+((0[xX][\da-fA-F]+)|(\d+))/) {
    $num = $1;
    $num = oct $num if $num =~ /^0/;
    }

  next unless /=.*sysTrapBase.*,/ .. /}.*SysTrapNumber.*;/;

  printf $format, $1, sprintf("0x%lX", $num++) if /sysTrap(\w+)\b/;
  }
