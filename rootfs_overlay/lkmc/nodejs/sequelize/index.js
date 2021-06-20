#!/usr/bin/env node

// https://cirosantilli.com/sequelize
//
// Before running this:
// * ensure that you can use peer authentication without password
//   from the command line, i.e. `psql` works
// * create the database for our test:
//   ``
//   createdb lkmc-nodejs
//   ``

const assert = require('assert');
const path = require('path');

const { Sequelize, DataTypes } = require('sequelize');

// To use the URI syntax, we need an explcit username and password.
// But the second constructor works with peer authentication.
// https://stackoverflow.com/questions/46207155/sequelize-and-peer-authentication-for-postgres
//
// Fails
//const sequelize = new Sequelize('postgres://user:password@localhost:5432/lkmc-nodejs')
//
// Works with peer authentication:
//const sequelize = new Sequelize('lkmc-nodejs', undefined, undefined, {
//  host: '/var/run/postgresql',
//  dialect: 'postgres',
//  logging: false,
//});
const sequelize = new Sequelize({
  dialect: 'sqlite',
  storage: 'tmp.' + path.basename(__filename) + '.sqlite',
});

// OMG fuck this asynchronous bullshit:
// https://stackoverflow.com/questions/39928452/execute-sequelize-queries-synchronously/43250120
(async () => {

// Connection sanity check.
// https://stackoverflow.com/questions/19429152/check-mysql-connection-in-sequelize/31115934
await sequelize.authenticate();

const IntegerNames = sequelize.define('IntegerNames', {
  value: {
    type: DataTypes.INTEGER,
    allowNull: false,
    unique: true,
  },
  name: {
    type: DataTypes.STRING,
  },
});

// Create the database defined by `sequelize.define`.
await IntegerNames.sync({force: true})
// After this:
//
// psql lkmc-nodejs -c '\dt'
//
// gives:
//
//            List of relations
//  Schema |     Name     | Type  | Owner
// --------+--------------+-------+-------
//  public | IntegerNames | table | ciro
// (2 rows)
//
// and:
//
// psql lkmc-nodejs -c '\d+ "IntegerNames"'
//
// gives:
//
//   Column   |           Type           | Collation | Nullable |                  Default                   | Storage  | Stats target | Description
// -----------+--------------------------+-----------+----------+--------------------------------------------+----------+--------------+-------------
//  id        | integer                  |           | not null | nextval('"IntegerNames_id_seq"'::regclass) | plain    |              |
//  value     | integer                  |           | not null |                                            | plain    |              |
//  name      | character varying(255)   |           |          |                                            | extended |              |
//  createdAt | timestamp with time zone |           | not null |                                            | plain    |              |
//  updatedAt | timestamp with time zone |           | not null |                                            | plain    |              |
// Indexes:
//     "IntegerNames_pkey" PRIMARY KEY, btree (id)

await IntegerNames.create({value: 2, name: 'two'});
await IntegerNames.create({value: 3, name: 'three'});
await IntegerNames.create({value: 5, name: 'five'});
// psql lkmc-nodejs -c 'SELECT * FROM "IntegerNames";'
//
// gives:
//
//  id | value | name  |         createdAt          |         updatedAt
// ----+-------+-------+----------------------------+----------------------------
//   1 |     2 | two   | 2021-03-19 19:12:08.436+00 | 2021-03-19 19:12:08.436+00
//   2 |     3 | three | 2021-03-19 19:12:08.436+00 | 2021-03-19 19:12:08.436+00
//   3 |     5 | five  | 2021-03-19 19:12:08.437+00 | 2021-03-19 19:12:08.437+00
// (3 rows)

const integerNames = await IntegerNames.findAll({
  where: {
    value: 2
  }
});
assert(integerNames[0].name === 'two');

// Truncate all tables.
// https://stackoverflow.com/questions/47816162/wipe-all-tables-in-a-schema-sequelize-nodejs/66985334#66985334
await sequelize.truncate();
assert((await IntegerNames.findAll()).length === 0);

// Datetime. Automatically converts to/from date objects.
const Dates = sequelize.define('Dates', {
  date: {
    type: DataTypes.DATE,
  },
}, {});
await Dates.sync({force: true})
let dateCreate = await Dates.create({date: new Date(2000, 0, 1, 2, 3, 4, 5)});
await Dates.create({date: new Date(2000, 0, 1, 2, 3, 4, 6)});
await Dates.create({date: new Date(2000, 0, 1, 2, 3, 4, 7)});
let date = await Dates.findOne({
  order: [
    ['date', 'ASC'],
  ],
});
assert(date.date.getTime() === new Date(2000, 0, 1, 2, 3, 4, 5).getTime());
assert(date.date.getTime() === dateCreate.date.getTime());

// Otherwise it hangs for 10 seconds, it seems that it keeps the connection alive.
// https://stackoverflow.com/questions/28253831/recreating-database-sequelizejs-is-slow
// https://github.com/sequelize/sequelize/issues/8468
await sequelize.close();
})();
