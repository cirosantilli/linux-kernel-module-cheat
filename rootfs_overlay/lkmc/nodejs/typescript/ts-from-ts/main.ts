#!/usr/bin/env ts-node

import { strict as assert } from 'assert';

// Transpiles require which node understands natively.
import anyname from './notmain'

const notmain = require('./notmain')

assert(anyname === 1)
assert(notmain.default === 1)
