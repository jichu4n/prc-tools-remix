# RPM_SPEC_VERSION(FNAME)  Extract a version number from an RPM .spec file
# -----------------------
# (We must use m4_define because using AC_DEFUN will lead to complaints when
# RPM_SPEC_VERSION is used from within AC_INIT, before AC_INIT has finished.
# The funky translit() strips \n characters.)
m4_define([RPM_SPEC_VERSION],
[translit(esyscmd([sed -n '/^Version:/{s/.*: *//p;q}' $1]), [
])])
