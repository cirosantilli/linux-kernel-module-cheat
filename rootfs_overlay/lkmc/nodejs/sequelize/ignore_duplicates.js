#!/usr/bin/env node

const assert = require('assert');
const path = require('path');
const { Sequelize, DataTypes } = require('sequelize');
const sequelize = new Sequelize({
  dialect: 'sqlite',
  storage: 'tmp.' + path.basename(__filename) + '.sqlite',
  define: {
    timestamps: false
  },
});
(async () => {
const Tag = sequelize.define('Tag', {
  name: {
    type: DataTypes.STRING,
    unique: true,
  },
});
await sequelize.sync({force: true})
await Tag.create({name: 't0'})

// Individual create does not have the option for some reason.
// Apparently you're just supposed to catch.
// https://github.com/sequelize/sequelize/issues/4513
//await Tag.create({name: 't0', ignoreDuplicates: true})

// SQLite: INSERT OR IGNORE INTO as desired.
const tags = await Tag.bulkCreate(
  [
    {name: 't0'},
    {name: 't1'},
    {name: 't1'},
    {name: 't2'},
  ],
  {
    ignoreDuplicates: true,
  }
)
const tagsFound = await Tag.findAll({order: [['name', 'ASC']]})
assert.strictEqual(tagsFound[0].name, 't0')
assert.strictEqual(tagsFound[1].name, 't1')
assert.strictEqual(tagsFound[2].name, 't2')
assert.strictEqual(tagsFound.length, 3)

await sequelize.close();
})();
