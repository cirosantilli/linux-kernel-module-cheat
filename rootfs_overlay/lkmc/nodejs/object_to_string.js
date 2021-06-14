#!/usr/bin/env node

// https://cirosantilli.com/linux-kernel-module-cheat#node-js

const util = require('util');

class MyClassUtilInspectCustomSubobject {
  constructor(a, b) {
    this.a = a;
    this.b = b;
  }
  [util.inspect.custom]() {
    return `my type is MyClassUtilInspectCustomSubobject and a is ${this.a} and b is ${this.b}`;
  }
}

class MyClassUtilInspectCustom {
  constructor(a, b, subobject) {
    this.a = a;
    this.b = b;
    this.subobject = subobject;
  }
  [util.inspect.custom]() {
    return `my type is MyClassUtilInspectCustom and a is ${this.a} and b is ${this.b} and subobject is ${util.inspect(this.subobject)}`;
  }
}

let my_object = new MyClassUtilInspectCustom(1, 2, new MyClassUtilInspectCustomSubobject(3, 4));
// Affected.
console.log('util.inspect');
console.log(util.inspect(my_object));
console.log();
// Affected.
console.log('console.log');
console.log(my_object);
console.log();
// Not affected.
console.log('toString');
console.log(my_object.toString());
console.log();
// Not affected.
console.log('toString implicit +');
console.log('implicit ' + my_object);
console.log();
// Not affected.
console.log('template string');
console.log(`${my_object}`);
console.log();

class MyClassToStringSubobject {
  constructor(a, b) {
    this.a = a;
    this.b = b;
  }
  toString() {
    return `my type is MyClassToString and a is ${this.a} and b is ${this.b}`;
  }
}

class MyClassToString {
  constructor(a, b, subobject) {
    this.a = a;
    this.b = b;
    this.subobject = subobject;
  }
  toString() {
    return `my type is MyClassToString and a is ${this.a} and b is ${this.b} and subobject is ${this.subobject}`;
  }
}

my_object = new MyClassToString(1, 2, new MyClassToStringSubobject(3, 4));
// Affected.
console.log('util.inspect');
console.log(util.inspect(my_object));
console.log();
// Affected.
console.log('console.log');
console.log(my_object);
console.log();
// Affected.
console.log('toString');
console.log(my_object.toString());
console.log();
// Affected.
console.log('toString implicit +');
console.log('implicit ' + my_object);
console.log();
// Affected.
console.log('template string');
console.log(`${my_object}`);
console.log();
