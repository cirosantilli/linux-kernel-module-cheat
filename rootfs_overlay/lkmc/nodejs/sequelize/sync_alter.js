#!/usr/bin/env node

// https://stackoverflow.com/questions/17708620/sequelize-changing-model-schema-on-production

const assert = require('assert');
const path = require('path');

const { Sequelize, DataTypes } = require('sequelize');

(async () => {
{
  const sequelize = new Sequelize({
    dialect: 'sqlite',
    storage: 'tmp.' + path.basename(__filename) + '.sqlite',
  });
  const IntegerNames = sequelize.define('IntegerNames', {
    value: { type: DataTypes.INTEGER, },
    name: { type: DataTypes.STRING, },
  }, {});
  await IntegerNames.sync({force: true})
  await IntegerNames.create({value: 2, name: 'two'});
  await IntegerNames.create({value: 3, name: 'three'});
  await sequelize.close();
}

// Alter by adding column..
{
  const sequelize = new Sequelize({
    dialect: 'sqlite',
    storage: 'tmp.' + path.basename(__filename) + '.sqlite',
  });
  const IntegerNames = sequelize.define('IntegerNames', {
    value: { type: DataTypes.INTEGER, },
    name: { type: DataTypes.STRING, },
    nameEs: { type: DataTypes.STRING, },
  }, {});
  await IntegerNames.sync({alter: true})
  await IntegerNames.create({value: 5, name: 'five' , nameEs: 'cinco'});
  await IntegerNames.create({value: 7, name: 'seven', nameEs: 'siete'});
  const integerNames = await IntegerNames.findAll({
    order: [['value', 'ASC']],
  });
  assert(integerNames[0].value  === 2);
  assert(integerNames[0].name   === 'two');
  assert(integerNames[0].nameEs === null);
  assert(integerNames[1].name   === 'three');
  assert(integerNames[1].nameEs === null);
  assert(integerNames[2].name   === 'five');
  assert(integerNames[2].nameEs === 'cinco');
  assert(integerNames[3].name   === 'seven');
  assert(integerNames[3].nameEs === 'siete');
  await sequelize.close();
}

// Alter by removing column. undefined instead of null,
// because the values really aren't present in the database anymore.
{
  const sequelize = new Sequelize({
    dialect: 'sqlite',
    storage: 'tmp.' + path.basename(__filename) + '.sqlite',
  });
  const IntegerNames = sequelize.define('IntegerNames', {
    value: { type: DataTypes.INTEGER, },
    name: { type: DataTypes.STRING, },
  }, {});
  await IntegerNames.sync({alter: true})
  const integerNames = await IntegerNames.findAll({
    order: [['value', 'ASC']],
  });
  assert(integerNames[0].value  === 2);
  assert(integerNames[0].name   === 'two');
  assert(integerNames[0].nameEs === undefined);
  assert(integerNames[1].name   === 'three');
  assert(integerNames[1].nameEs === undefined);
  assert(integerNames[2].name   === 'five');
  assert(integerNames[2].nameEs === undefined);
  assert(integerNames[3].name   === 'seven');
  assert(integerNames[3].nameEs === undefined);
  await sequelize.close();
}

// Alter a type.
// Hmm, docs suggest data would get dropped, but I only see typecast without alter.drop,
// so shy does it work still?. sqlite from CLI does confirm that it is now a VARCHAR(255)
// column.
{
  const sequelize = new Sequelize({
    dialect: 'sqlite',
    storage: 'tmp.' + path.basename(__filename) + '.sqlite',
  });
  const IntegerNames = sequelize.define('IntegerNames', {
    value: { type: DataTypes.STRING, },
    name: { type: DataTypes.STRING, },
  }, {});
  await IntegerNames.sync({alter: true})
  const integerNames = await IntegerNames.findAll({
    order: [['value', 'ASC']],
  });
  assert(integerNames[0].value === '2');
  assert(integerNames[0].name  === 'two');
  assert(integerNames[1].value === '3');
  assert(integerNames[1].name  === 'three');
  assert(integerNames[2].value === '5');
  assert(integerNames[2].name  === 'five');
  assert(integerNames[3].value === '7');
  assert(integerNames[3].name  === 'seven');
  await sequelize.close();
}

})();
