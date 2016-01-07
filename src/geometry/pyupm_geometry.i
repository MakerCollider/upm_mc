%module pyupm_geometry
%include "../upm.i"

%feature("autodoc", "3");

%include "geometry.h"
%{
    #include "geometry.h"
%}
