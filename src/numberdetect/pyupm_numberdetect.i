%module pyupm_numberdetect
%include "../upm.i"

%feature("autodoc", "3");

%include "numberdetect.h"
%{
    #include "numberdetect.h"
%}
