#!/usr/bin/env node
// Based on: https://medium.com/@Trott/using-worker-threads-in-node-js-80494136dbb6
'use strict';
const { Worker, isMainThread, parentPort, workerData } = require('worker_threads');
const min = 2;
let primes = [];
function generatePrimes(start, range) {
  console.log('started: ' + start);
  let isPrime = true;
  let end = start + range;
  for (let i = start; i < end; i++) {
    for (let j = min; j < Math.sqrt(end); j++) {
      if (i !== j && i%j === 0) {
        isPrime = false;
        break;
      }
    }
    if (isPrime) {
      primes.push(i);
    }
    isPrime = true;
  }
  console.log('ended: ' + start);
}
if (isMainThread) {
  const max = 1e2;
  const threadCount = +process.argv[2] || 10;
  const threads = new Set();
  const range = Math.ceil((max - min) / threadCount);
  let start = min;
  for (let i = 0; i < threadCount - 1; i++) {
    const myStart = start;
    threads.add(new Worker(__filename, { workerData: { start: myStart, range }}));
    start += range;
  }
  threads.add(new Worker(__filename, { workerData: { start, range: range + ((max - min + 1) % threadCount)}}));
  for (let worker of threads) {
    worker.on('error', (err) => { throw err; });
    worker.on('exit', () => {
      threads.delete(worker);
      if (threads.size === 0) {
        console.log(primes.sort((a, b) => a - b).join('\n'));
      }
    })
    worker.on('message', (msg) => {
      primes = primes.concat(msg);
    });
  }
} else {
  generatePrimes(workerData.start, workerData.range);
  parentPort.postMessage(primes);
}
