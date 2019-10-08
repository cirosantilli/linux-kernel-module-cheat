#!/usr/bin/env node

// https://cirosantilli.com/linux-kernel-module-cheat#node-js

const assert = require('assert');
const fs = require('fs');
const path = require('path');

const file_path = path.basename(__filename) + '.tmp';
const data = 'asdf\nqwer\n';
const encoding = 'utf8';

// Write to file.
//
// Error handling through exceptions:
// https://stackoverflow.com/questions/15543235/checking-if-writefilesync-successfully-wrote-the-file
fs.writeFileSync(file_path, data, encoding);

// Read from file.
assert.strictEqual(fs.readFileSync(file_path, encoding), data);
