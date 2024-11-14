from setuptools import setup, Extension

module = Extension('symnmf',
                    sources=['symnmfmodule.c', 'symnmf.c'],
                    include_dirs=[],
                    extra_compile_args=[],
                    extra_link_args=[])

setup(name='symnmf',
      version='1.0',
      description='Python C extension for symmetric normalized similarity matrix calculations',
      ext_modules=[module])
