%module pyupm_rgbled
%include "../upm.i"

%feature("autodoc", "3");

%include "rgbled.h"
%{
    #include "rgbled.h"
%}
