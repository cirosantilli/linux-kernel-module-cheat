#!/usr/bin/env ts-node

import { strict as assert } from 'assert';

// Fails.
// https://stackoverflow.com/questions/41292559/could-not-find-a-declaration-file-for-module-module-name-path-to-module-nam
//import {x, y} from './notmain'
//assert(x === 1)
//assert(y === 2)

const notmain = require('./notmain')
assert(notmain.x === 1)
assert(notmain.y === 2)
