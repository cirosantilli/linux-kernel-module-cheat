#!/usr/bin/env node

const assert = require('assert');

const notmain = require('./notmain.js')
assert(notmain.x === 1)
assert(notmain.y === 2)
