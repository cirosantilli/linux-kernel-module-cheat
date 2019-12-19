const fs = require('fs');
const path = require('path');

function myfunc() {
  const package_path = path.dirname(require.resolve(path.join('cirosantilli-data-files', 'package.json')));
  return fs.readFileSync(path.join(package_path, 'mydata.txt'), 'utf-8');
}
exports.myfunc = myfunc;
