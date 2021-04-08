#!/usr/bin/env node

// Outcome:
//
// before
// after
// [sleep 1 second]
// ok

var p1 = new Promise((resolve, reject) => {
  setTimeout(
    () => {
      resolve('ok')
      //reject(new Error('my error'));
    },
    1000
  );
});

console.log('before');
p1.then(value => {
  console.log(value);
}, reason => {
  console.log(reason);
});
console.log('after');
