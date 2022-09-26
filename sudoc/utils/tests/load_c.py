"""
Load C modules into python using CFFI.
"""

import os
import cffi
import importlib

ROOT = r"sudoc/"


def load(folder, module_name):
    """
    Load a C module into python using CFFI.

    Args:
        filepath (str): relative path to the C module.
    """
    
    src_path = os.path.join(
        ROOT,
        folder,
        "src",
        module_name + ".c",
    )

    include_path = os.path.join(
        ROOT,
        folder,
        "include",
        module_name + ".h",
    )

    print("src_path:", src_path)
    print("include_path:", include_path)

    source = open(src_path).read()
    includes = open(include_path).read()

    ffi = cffi.FFI()
    ffi.cdef(includes)
    ffi.set_source(module_name + '_', source)
    ffi.compile()

    module = importlib.import_module(module_name + '_')
    return module
