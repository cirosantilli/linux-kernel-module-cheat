#!/usr/bin/env node

const express = require('express')

function check_helper(req, res) {
  if (req.params.param.length > 2) {
    res.status(404)
    res.send('ko')
  } else {
    return req.params.param + 'ok'
  }
}

const app = express()
app.get('/', (req, res) => {
  res.send('hello world')
})
app.get('/check-helper-1/:param', (req, res) => {
  const ret = check_helper(req, res)
  if (ret) {
    res.send(ret)
  }
})
app.get('/check-helper-2/:param', (req, res) => {
  const ret = check_helper(req, res)
  if (ret) {
    res.send(ret)
  }
})
app.get('/error', async (req, res, next) => {
  try {
    throw 'my error'
    res.send('never returned')
  } catch(error) {
    next(error);
  }
})
app.get('/query', (req, res) => {
  res.send(`aa: ${req.query.aa} bb: ${req.query.bb}`)
})
app.get('/splat/:splat(*)', (req, res) => {
  res.send('splat ' + req.params.splat)
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
      assert.strictEqual(res.statusCode, status);
      res.on('data', d => {
        if (body !== undefined) {
          assert.strictEqual(d.toString(), body);
        }
      })
    }).end()
  }
  test('/', 'GET', 200, 'hello world')
  test('/', 'POST', 404)
  test('/dontexist', 'GET', 404)

  // Shows 'my error' on terminal, without stack trace.
  test('/error', 'GET', 500)

  test('/query?aa=000&bb=111', 'GET', 200, 'aa: 000 bb: 111')

  // https://stackoverflow.com/questions/10020099/express-js-routing-optional-splat-param
  // https://stackoverflow.com/questions/16829803/express-js-route-parameter-with-slashes
  // https://stackoverflow.com/questions/34571784/how-to-use-parameters-containing-a-slash-character
  test('/splat/aaa', 'GET', 200, 'splat aaa')
  test('/splat/aaa/bbb', 'GET', 200, 'splat aaa/bbb')
  test('/splat/aaa/bbb/ccc', 'GET', 200, 'splat aaa/bbb/ccc')

  test('/check-helper-1/aa', 'GET', 200, 'aaok')
  test('/check-helper-2/bb', 'GET', 200, 'bbok')
  test('/check-helper-1/ccc', 'GET', 404, 'ko')
  test('/check-helper-2/ddd', 'GET', 404, 'ko')
})
