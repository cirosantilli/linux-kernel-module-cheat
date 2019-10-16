#!/usr/bin/env node

// https://cirosantilli.com/linux-kernel-module-cheat#node-js

const assert = require('assert');

const char_is_alphanumeric = function(c) {
  let code = c.codePointAt(0);
  return (
    // 0-9
    (code > 47 && code < 58) ||
    // A-Z
    (code > 64 && code < 91) ||
    // a-z
    (code > 96 && code < 123)
  )
}

const is_alphanumeric = function (str) {
  for (let c of str) {
    if (!char_is_alphanumeric(c)) {
      return false;
    }
  }
  return true;
};

// Arbitrarily defined as alphanumeric or '-' or '_'.
const is_almost_alphanumeric = function (str) {
  for (let c of str) {
    if (
      !char_is_alphanumeric(c) &&
      !is_almost_alphanumeric.chars.has(c)
    ) {
      return false;
    }
  }
  return true;
};
is_almost_alphanumeric.chars = new Set(['-', '_']);

assert( is_alphanumeric('aB0'));
assert(!is_alphanumeric('aB0_-'));
assert(!is_alphanumeric('aB0_-*'));
assert(!is_alphanumeric('你好'));

assert( is_almost_alphanumeric('aB0'));
assert( is_almost_alphanumeric('aB0_-'));
assert(!is_almost_alphanumeric('aB0_-*'));
assert(!is_almost_alphanumeric('你好'));
