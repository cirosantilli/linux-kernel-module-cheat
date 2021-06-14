#!/usr/bin/env node

// https://stackoverflow.com/questions/22958683/how-to-implement-many-to-many-association-in-sequelize/67973948#67973948

const assert = require('assert');
const path = require('path');

const { Sequelize, DataTypes } = require('sequelize');

const sequelize = new Sequelize({
  dialect: 'sqlite',
  storage: 'tmp.' + path.basename(__filename) + '.sqlite',
});

(async () => {

// Create the tables.
const User = sequelize.define('User', {
  name: { type: DataTypes.STRING },
}, {});
User.belongsToMany(User, {through: 'UserFollowsUser', as: 'Follow'});
await sequelize.sync({force: true});

// Create some users.

const user0 = await User.create({name: 'user0'})
const user1 = await User.create({name: 'user1'})
const user2 = await User.create({name: 'user2'})
const user3 = await User.create({name: 'user3'})

// Make user0 follow user1 and user2
await user0.addFollow(user1)
await user0.addFollow(user2)
const user0Follows = await user0.getFollow({order: [['name', 'ASC']]})
assert(user0Follows[0].name === 'user1');
assert(user0Follows[1].name === 'user2');
assert(user0Follows.length === 2);

await sequelize.close();
})();
