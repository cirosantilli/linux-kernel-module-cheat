#!/usr/bin/env node

import { strict as assert } from 'assert';

import anyname from './notmain.js'
assert(anyname === 1)


// Not possible, have to use import everywhere.
//import notmain2 from './notmain2.js'
//const notmain2 = require('notmain2');
