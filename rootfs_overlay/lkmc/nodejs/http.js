#!/usr/bin/env node

// https://cirosantilli.com/linux-kernel-module-cheat#node-js

const http = require('http');
const util = require('util');
const url = require('url');

http.createServer(
  (req, res) => {
    const myUrl = new url.URL(req.url, 'http://example.com');
    const searchString = [];
    for (const [key, value] of myUrl.searchParams) {
      searchString.push(`<div>${key}: ${value}</div>\n`)
    }
    const reqString = [];
    for (let prop of Object.keys(req).sort()) {
      reqString.push(`<div>${prop}: ${util.inspect(req[prop])}</div>\n`);
    }
    ret = `<!doctype html>
<html lang=en>
<head>
<meta charset=utf-8>
<title>hello html</title>
</head>
<body>
<div>req.url: ${req.url}</div>
<div>url.pathname: ${myUrl.pathname}</div>
<div>url.search:</div>
${searchString.join('')}
<div>req:</div>
${reqString.join('')}
</body>
</html>
`
  res.write(ret);
  res.end();
}).listen(8080);
