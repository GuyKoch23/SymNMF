from setuptools import Extension, setup

module = Extension("csymnmf", sources=['symnmfmodule.c', 'symnmf.c'])
setup(name='csymnmf',
      version='1.0.0.0',
      description='Python wrapper',
      ext_modules=[module])