%module pyupm_facedetect
%include "../upm.i"

%feature("autodoc", "3");

%include "facedetect.h"
%{
    #include "facedetect.h"
%}
