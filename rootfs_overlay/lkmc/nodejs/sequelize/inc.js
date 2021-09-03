#!/usr/bin/env node

const assert = require('assert');
const path = require('path');

const { Sequelize, DataTypes } = require('sequelize');
const sequelize = new Sequelize({
  dialect: 'sqlite',
  storage: 'tmp.' + path.basename(__filename) + '.sqlite',
});

(async () => {
await sequelize.authenticate();
const IntegerNames = sequelize.define('IntegerNames', {
  value: {
    type: DataTypes.INTEGER,
  },
  name: {
    type: DataTypes.STRING,
  },
});
await IntegerNames.sync({force: true})
await IntegerNames.create({value: 2, name: 'two'});
await IntegerNames.create({value: 3, name: 'three'});
await IntegerNames.create({value: 5, name: 'five'});
const integerName5 = await IntegerNames.findOne({ where: { value: 5 } });
integerName5.increment('value')
// Sequelize updates, but others don't...
console.error(integerName5.value);
const integerName6 = await IntegerNames.findOne({ where: { value: 6 } });
assert.strictEqual(integerName6.name, 'five')
await sequelize.close();
})();
