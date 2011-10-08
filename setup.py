#!/usr/bin/python

from distutils.core import setup
from distutils.extension import Extension
from Pyrex.Distutils import build_ext
if __name__ == "__main__":
    setup(
      version = "0.6",
      author = "Steven Walter",
      author_email = "stevenrwalter@gmail.com",
      url = "https://github.com/srwalter/libsmartpen/",
      license = "GPLv2",
      name = "pysmartpen",
      py_modules=["parsestf"],
      ext_modules=[ 
        Extension("pysmartpen", ["pysmartpen.pyx"], libraries = ["smartpen"],
                  library_dirs=["."])
        ],
      cmdclass = {'build_ext': build_ext}
    )
