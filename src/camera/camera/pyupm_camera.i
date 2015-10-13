%module pyupm_camera
%include "../upm.i"

%feature("autodoc", "3");

%include "camera.h"
%{
    #include "camera.h"
%}
