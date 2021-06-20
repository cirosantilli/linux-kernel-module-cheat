#!/usr/bin/env node

// Like association_many_to_many_same_model but with a super many to many,
// i.e. explicit through table relations).

const assert = require('assert');
const path = require('path');

const { Sequelize, DataTypes, Op } = require('sequelize');

const sequelize = new Sequelize({
  dialect: 'sqlite',
  storage: 'tmp.' + path.basename(__filename) + '.sqlite',
  define: {
    timestamps: false
  },
});

(async () => {

// Create the tables.
const User = sequelize.define('User', {
  name: { type: DataTypes.STRING },
});
const UserFollowUser = sequelize.define('UserFollowUser', {
    UserId: {
      type: DataTypes.INTEGER,
      references: {
        model: User,
        key: 'id'
      }
    },
    FollowId: {
      type: DataTypes.INTEGER,
      references: {
        model: User,
        key: 'id'
      }
    },
  }
);

// Super many to many. Only works with explicit table for some reason.
User.belongsToMany(User, {through: UserFollowUser, as: 'Follows'});
UserFollowUser.belongsTo(User)
User.hasMany(UserFollowUser)

await sequelize.sync({force: true});

// Create some users.

const user0 = await User.create({name: 'user0'})
const user1 = await User.create({name: 'user1'})
const user2 = await User.create({name: 'user2'})
const user3 = await User.create({name: 'user3'})
await user0.addFollows([user1, user2])
await user2.addFollow(user0)
await user3.addFollow(user0)

// Find all users that a user follows.
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

// Same but with explicit id.
{
  const user0Follows = (await User.findOne({
    where: {id: user0.id},
    attributes: [],
    include: [{
      model: User,
      as: 'Follows',
      through: {attributes: []},
    }],
  })).Follows
  assert(user0Follows[0].name === 'user1');
  assert(user0Follows[1].name === 'user2');
  assert(user0Follows.length === 2);
}

// Another method with the many-to-many reversed.
// Using the super many to many is the only way I know of doing this so far.
// which is a pain.
{
  const user0Follows = await User.findAll({
    include: [{
      model: UserFollowUser,
      attributes: [],
      on: {
        FollowId: { [Op.col]: 'User.id' },
      },
      where: {UserId: user0.id}
    }],
    order: [['name', 'ASC']],
  })
  assert(user0Follows[0].name === 'user1');
  assert(user0Follows[1].name === 'user2');
  assert(user0Follows.length === 2);
}

await sequelize.close();
})();
