#!/usr/bin/python

from distutils.core import setup
from distutils.extension import Extension
from Pyrex.Distutils import build_ext
setup(
  name = "pysmartpen",
  py_modules=["parsestf"],
  ext_modules=[ 
    Extension("pysmartpen", ["pysmartpen.pyx"], libraries = ["smartpen"],
              library_dirs=["."])
    ],
  cmdclass = {'build_ext': build_ext}
)
