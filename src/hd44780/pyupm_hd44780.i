%module pyupm_hd44780
%include "../upm.i"

%feature("autodoc", "3");

%include "hd44780.h"
%{
    #include "hd44780.h"
%}
