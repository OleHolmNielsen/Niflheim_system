# Providing A Standard Set Of Modules for all Users.
# From http://lmod.readthedocs.io/en/latest/070_standard_modules.html
if ( ! $?__Init_Default_Modules )  then
  setenv __Init_Default_Modules 1
  setenv EASYBUILD_MODULES_TOOL Lmod
  setenv EASYBUILD_PREFIX /home/modules
  module use $EASYBUILD_PREFIX/modules/all
else
  module refresh
endif
