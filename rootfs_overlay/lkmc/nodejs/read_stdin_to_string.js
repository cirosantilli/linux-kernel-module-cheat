#!/usr/bin/env node

// https://cirosantilli.com/linux-kernel-module-cheat#node-js

const fs = require('fs');
console.log(fs.readFileSync(0, 'utf8'));
