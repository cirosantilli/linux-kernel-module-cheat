#!/usr/bin/env node

const express = require('express')

const app = express()
app.get('/', (req, res) => {
  res.send('hello world')
})
app.get('/error', async (req, res, next) => {
  try {
    throw 'my error'
    res.send('never returned')
  } catch(error) {
    next(error);
  }
})
const server = app.listen(3000, () => {
  console.log(`listening: http://localhost:${server.address().port}`)

  // Test it.
  function test(path, method, status, body) {
    const assert = require('assert')
    const http = require('http')
    const options = {
      hostname: 'localhost',
      port: server.address().port,
      path: path,
      method: method,
    }
    http.request(options, res => {
      console.error(res.statusCode);
      assert(res.statusCode === status);
      res.on('data', d => {
        if (body !== undefined) {
          assert(d.toString() === body);
        }
      })
    }).end()
  }
  test('/', 'GET', 200, 'hello world')
  test('/', 'POST', 404)
  test('/dontexist', 'GET', 404)
  // Shows 'my error' on terminal, without stack trace.
  test('/error', 'GET', 500)
})
