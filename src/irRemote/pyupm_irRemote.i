%module pyupm_irRemote
%include "../upm.i"

%feature("autodoc", "3");

%include "irRemote.h"
%{
    #include "irRemote.h"
%}
