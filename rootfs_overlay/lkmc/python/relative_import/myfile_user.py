#!/usr/bin/env python

import myfile
import mydir.myfile_user

assert myfile.a == 1
assert mydir.myfile_user.b == 2
