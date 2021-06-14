#!/usr/bin/env node

// https://stackoverflow.com/questions/2257993/how-to-display-all-methods-of-an-object/67975040#67975040
// https://stackoverflow.com/questions/30881632/es6-iterate-over-class-methods/47714550#47714550

const isGetter = (x, name) => (Object.getOwnPropertyDescriptor(x, name) || {}).get
const isFunction = (x, name) => typeof x[name] === "function";
const deepFunctions = x =>
  x && x !== Object.prototype &&
  Object.getOwnPropertyNames(x)
    .filter(name => isGetter(x, name) || isFunction(x, name))
    .concat(deepFunctions(Object.getPrototypeOf(x)) || []);
const distinctDeepFunctions = x => Array.from(new Set(deepFunctions(x)));
const getMethods = (obj) => distinctDeepFunctions(obj).filter(
    name => name !== "constructor" && !~name.indexOf("__"));

// Example usage.

class BaseClass {
  constructor() {
    this.baseProp = 1
  }
  override() { return 1; }
  baseMethod() { return 2; }
}

class DerivedClass extends BaseClass {
  constructor() {
    super()
    this.derivedProp = 2
  }
  override() { return 3; }
  get myGetter() { return 4; }
  static myStatic() { return 5; }
}

const obj = new DerivedClass();
const methods = getMethods(obj)
methods.sort()
const assert = require('assert')
const util = require('util')
assert(methods[0] === 'baseMethod')
assert(methods[1] === 'myGetter')
assert(methods[2] === 'override')
assert(methods.length === 3)

console.log(getMethods(Math))
