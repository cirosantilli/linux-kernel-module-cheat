#!/usr/bin/env python3

import importlib.machinery
import importlib.util
import os
import sys

root_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def import_path(path):
    '''
    https://stackoverflow.com/questions/2601047/import-a-python-module-without-the-py-extension
    https://stackoverflow.com/questions/31773310/what-does-the-first-argument-of-the-imp-load-source-method-do
    '''
    module_name = os.path.basename(path).replace('-', '_')
    spec = importlib.util.spec_from_loader(
        module_name,
        importlib.machinery.SourceFileLoader(module_name, path)
    )
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    sys.modules[module_name] = module
    return module

def import_path_relative_root(basename):
    return import_path(os.path.join(root_dir, basename))

def import_path_main(basename):
    '''
    Import an object of the Main class of a given file.

    By convention, we call the main object of all our CLI scripts as Main.
    '''
    return import_path_relative_root(basename).Main()
