# Providing A Standard Set Of Modules for all Users.
# From http://lmod.readthedocs.io/en/latest/070_standard_modules.html
if [ -z "$__Init_Default_Modules" ]; then
   export __Init_Default_Modules=1
   export EASYBUILD_MODULES_TOOL=Lmod
   export EASYBUILD_PREFIX=/home/modules
   module use $EASYBUILD_PREFIX/modules/all
else
   module refresh
fi
