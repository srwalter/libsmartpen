#!/usr/bin/python

from distutils.core import setup
from distutils.extension import Extension
from Pyrex.Distutils import build_ext
setup(
  name = "pysmartpen",
  ext_modules=[ 
    Extension("pysmartpen", ["pysmartpen.pyx"], libraries = ["smartpen"])
    ],
  cmdclass = {'build_ext': build_ext}
)
