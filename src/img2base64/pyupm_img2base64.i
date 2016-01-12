%module pyupm_img2base64
%include "../upm.i"

%feature("autodoc", "3");

%include "img2base64.h"
%{
    #include "img2base64.h"
%}
