#!/usr/bin/env node

// https://stackoverflow.com/questions/54898994/bulkupdate-in-sequelize-orm/69044138#69044138

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
      unique: true, // mandatory
      primaryKey: true,
    },
    name: {
      type: DataTypes.STRING,
    },
  },
  {
    timestamps: false,
  }
);
await IntegerNames.sync({force: true})
await IntegerNames.create({value: 2, name: 'two'});
await IntegerNames.create({value: 3, name: 'three'});
await IntegerNames.create({value: 5, name: 'five'});

// Initial state.
assert.strictEqual((await IntegerNames.findOne({ where: { value: 2 } })).name, 'two');
assert.strictEqual((await IntegerNames.findOne({ where: { value: 3 } })).name, 'three');
assert.strictEqual((await IntegerNames.findOne({ where: { value: 5 } })).name, 'five');
assert.strictEqual(await IntegerNames.count(), 3);

// Update.
await IntegerNames.bulkCreate(
  [
    {value: 2, name: 'TWO'},
    {value: 3, name: 'THREE'},
    {value: 7, name: 'SEVEN'},
  ],
  { updateOnDuplicate: ["name"] }
);

// Final state.
assert.strictEqual((await IntegerNames.findOne({ where: { value: 2 } })).name, 'TWO');
assert.strictEqual((await IntegerNames.findOne({ where: { value: 3 } })).name, 'THREE');
assert.strictEqual((await IntegerNames.findOne({ where: { value: 5 } })).name, 'five');
assert.strictEqual((await IntegerNames.findOne({ where: { value: 7 } })).name, 'SEVEN');
assert.strictEqual(await IntegerNames.count(), 4);

await sequelize.close();
})();
