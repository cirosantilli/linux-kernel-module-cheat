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
User.belongsToMany(User, {through: 'UserFollowUser', as: 'Follows'});
await sequelize.sync({force: true});

// Create some users.

const user0 = await User.create({name: 'user0'})
const user1 = await User.create({name: 'user1'})
const user2 = await User.create({name: 'user2'})
const user3 = await User.create({name: 'user3'})

// Make user0 follow user1 and user2
await user0.addFollows([user1, user2])
// Make user2 and user3 follow user0
await user2.addFollow(user0)
await user3.addFollow(user0)

// Check that the follows worked.
const user0Follows = await user0.getFollows({order: [['name', 'ASC']]})
assert(user0Follows[0].name === 'user1');
assert(user0Follows[1].name === 'user2');
assert(user0Follows.length === 2);

const user1Follows = await user1.getFollows({order: [['name', 'ASC']]})
assert(user1Follows.length === 0);

const user2Follows = await user2.getFollows({order: [['name', 'ASC']]})
assert(user2Follows[0].name === 'user0');
assert(user2Follows.length === 1);

const user3Follows = await user3.getFollows({order: [['name', 'ASC']]})
assert(user3Follows[0].name === 'user0');
assert(user3Follows.length === 1);

// Same but with ID instead of object.
{
  const user0Follows = (await User.findOne({
    where: {id: user0.id},
    include: [{model: User, as: 'Follows'}],
  })).Follows
  assert(user0Follows[0].name === 'user1');
  assert(user0Follows[1].name === 'user2');
  assert(user0Follows.length === 2);
}

// has methods
assert(!await user0.hasFollow(user0))
assert(!await user0.hasFollow(user0.id))
assert( await user0.hasFollow(user1))
assert( await user0.hasFollow(user2))
assert(!await user0.hasFollow(user3))

// Count method
assert(await user0.countFollows() === 2)

await sequelize.close();
})();
