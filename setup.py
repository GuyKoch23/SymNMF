from setuptools import Extension, setup

module = Extension("symmnf", sources=['symnmfmodule.c'])
setup(name='symmnf',
      version='1.0.0.0',
      description='Python wrapper',
      ext_modules=[module])