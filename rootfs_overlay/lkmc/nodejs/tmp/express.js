#!/usr/bin/env node

const assert = require('assert')
const http = require('http')

const express = require('express')
const app = express()
const port = 3000

app.get('/error', async (req, res) => {
  throw 'my error'
})

const server = app.listen(port, () => {
  // Test it.
  function test(path, method, status, body) {
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
        console.error(d.toString());
        if (body !== undefined) {
          assert(d.toString() === body);
        }
      })
    }).end()
  }
  test('/error', 'GET', 500, 'my error')
})
