#!/usr/bin/env ts-node

// Does not work. ts does not transpile this file apparently,
// only files with the .ts extension are transpiled.
//import { x, y } from './notmain'

const assert = require('assert');

const notmain = require('./notmain');

assert(notmain.x === 1);
assert(notmain.y === 2);
