#!/usr/bin/env node

const assert = require('assert');

let date = new Date(2000, 0, 1, 2, 3, 4, 5);
let date2 = new Date(2000, 0, 1, 2, 3, 4, 5);
assert(date.getTime() === date2.getTime());
assert(date.toString() === 'Sat Jan 01 2000 02:03:04 GMT+0000 (Greenwich Mean Time)');
