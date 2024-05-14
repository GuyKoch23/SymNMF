from setuptools import Extension, setup

module = Extension("symnmf", sources=['symnmfmodule.c'])
setup(name='symnmf',
      version='1.0.0.0',
      description='Python wrapper',
      ext_modules=[module])