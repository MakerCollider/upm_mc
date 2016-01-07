%module pyupm_irremote
%include "../upm.i"

%feature("autodoc", "3");

%include "irremote.h"
%{
    #include "irremote.h"
%}
