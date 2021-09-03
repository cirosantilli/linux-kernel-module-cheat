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
const Integer = sequelize.define('Integer',
  {
    value: {
      type: DataTypes.INTEGER,
      unique: true, // mandatory
      primaryKey: true,
    },
    name: {
      type: DataTypes.STRING,
    },
    inverse: {
      type: DataTypes.INTEGER,
    },
  },
  {
    timestamps: false,
  }
);
await Integer.sync({force: true})
await Integer.create({value: 2, inverse: -2, name: 'two'});
await Integer.create({value: 3, inverse: -3, name: 'three'});
await Integer.create({value: 5, inverse: -5, name: 'five'});

// Initial state.
assert.strictEqual((await Integer.findOne({ where: { value: 2 } })).name, 'two');
assert.strictEqual((await Integer.findOne({ where: { value: 3 } })).name, 'three');
assert.strictEqual((await Integer.findOne({ where: { value: 5 } })).name, 'five');
assert.strictEqual((await Integer.findOne({ where: { value: 2 } })).inverse, -2);
assert.strictEqual((await Integer.findOne({ where: { value: 3 } })).inverse, -3);
assert.strictEqual((await Integer.findOne({ where: { value: 5 } })).inverse, -5);
assert.strictEqual(await Integer.count(), 3);

// Update.
await Integer.bulkCreate(
  [
    {value: 2, name: 'TWO'},
    {value: 3, name: 'THREE'},
    {value: 7, name: 'SEVEN'},
  ],
  { updateOnDuplicate: ["name"] }
);

// Final state.
assert.strictEqual((await Integer.findOne({ where: { value: 2 } })).name, 'TWO');
assert.strictEqual((await Integer.findOne({ where: { value: 3 } })).name, 'THREE');
assert.strictEqual((await Integer.findOne({ where: { value: 5 } })).name, 'five');
assert.strictEqual((await Integer.findOne({ where: { value: 7 } })).name, 'SEVEN');
assert.strictEqual((await Integer.findOne({ where: { value: 2 } })).inverse, -2);
assert.strictEqual((await Integer.findOne({ where: { value: 3 } })).inverse, -3);
assert.strictEqual((await Integer.findOne({ where: { value: 5 } })).inverse, -5);
assert.strictEqual(await Integer.count(), 4);

await sequelize.close();
})();
