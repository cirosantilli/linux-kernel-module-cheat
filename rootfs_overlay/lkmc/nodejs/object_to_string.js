#!/usr/bin/env node

// https://cirosantilli.com/linux-kernel-module-cheat#node-js


const util = require('util');

class MyClassUtilInspectCustom {
  constructor(a, b) {
    this.a = a;
    this.b = b;
  }
  [util.inspect.custom]() {
    return `my type is MyClassUtilInspectCustom and a is ${this.a} and b is ${this.b}`;
  }
}

let my_object = new MyClassUtilInspectCustom(1, 2);
console.log('util.inspect');
console.log(util.inspect(my_object));
console.log('console.log');
console.log(my_object);
console.log('toString');
console.log(my_object.toString());
console.log();

class MyClassToString {
  constructor(a, b) {
    this.a = a;
    this.b = b;
  }
  toString() {
    return `my type is MyClassToString and a is ${this.a} and b is ${this.b}`;
  }
}

my_object = new MyClassToString(1, 2);
console.log('util.inspect');
console.log(util.inspect(my_object));
console.log('console.log');
console.log(my_object);
console.log('toString');
console.log(my_object.toString());
