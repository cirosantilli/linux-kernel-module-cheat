#!/usr/bin/env node

// https://cirosantilli.com/linux-kernel-module-cheat#node-js

class MyClassSub {
  constructor(a, b) {
    this.a = a;
    this.b = b;
  }
}

class MyClass {
  constructor(a, b, sub) {
    this.a = a;
    this.b = b;
    this.sub = sub;
  }
}

console.log(JSON.stringify(new MyClass(1, 2, new MyClassSub(3, 4))));

class MyClassCustomSub {
  constructor(a, b) {
    this.a = a;
    this.b = b;
  }
  toJSON() {
    return {
      a: this.a,
    }
  }
}

class MyClassCustom {
  constructor(a, b, sub) {
    this.a = a;
    this.sub = sub;
  }
  toJSON() {
    return {
      a: this.a,
      sub: this.sub,
    }
  }
}

console.log(JSON.stringify(new MyClassCustom(1, 2, new MyClassCustomSub(3, 4))));
