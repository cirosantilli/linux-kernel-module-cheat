#!/usr/bin/env node

// https://github.com/sequelize/sequelize/issues/3534
// https://github.com/sequelize/sequelize/issues/8586

const assert = require('assert');
const path = require('path');

const { Sequelize, DataTypes } = require('sequelize');

const sequelize = new Sequelize({
  dialect: 'sqlite',
  storage: 'tmp.' + path.basename(__filename) + '.sqlite',
});

(async () => {

const IntegerNames = sequelize.define('IntegerNames',
  {
    value: {
      type: DataTypes.INTEGER,
      allowNull: false,
      unique: true,
    },
    name: {
      type: DataTypes.STRING,
    },
    name2: {
      type: DataTypes.STRING,
    },

  },
  {
    hooks: {
      beforeValidate: (integerName, options) => {
        integerName.name2 = integerName.name + 'asdf'
        // This fixes the failure.
        //options.fields.push('name2');
      }
    ,}
  },
);
await IntegerNames.sync({force: true})
await IntegerNames.create({value: 2, name: 'two'});
await IntegerNames.create({value: 3, name: 'three'});
await IntegerNames.create({value: 5, name: 'five'});

const integerName = await IntegerNames.findOne({ where: { value: 2 } });
assert.strictEqual(integerName.name, 'two');
assert.strictEqual(integerName.name2, 'twoasdf');
integerName.name = 'TWO'
integerName.save();

const integerName2 = await IntegerNames.findOne({ where: { value: 2 } });
assert.strictEqual(integerName2.name, 'TWO');
// Fails.
//assert.strictEqual(integerName2.name2, 'TWOasdf');

await sequelize.close();

})();
