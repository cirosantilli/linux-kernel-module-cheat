#!/usr/bin/env node

const assert = require('assert');

class C {
  static f(i) {
    return i + 1;
  }
  g(i) {
    // https://stackoverflow.com/questions/19470559/how-to-access-static-member-on-instance
    return this.constructor.f(i) + 1;
  }
}

assert(new C().g(1) === 3);
