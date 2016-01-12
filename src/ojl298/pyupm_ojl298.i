%module pyupm_ojl298
%include "../upm.i"

%feature("autodoc", "3");

%include "ojl298.h"
%{
    #include "ojl298.h"
%}
