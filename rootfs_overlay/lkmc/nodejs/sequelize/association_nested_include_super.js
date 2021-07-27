#!/usr/bin/env node

// Like association_nested_include.js but with a super many to many.

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
const Post = sequelize.define('Post', {
  body: { type: DataTypes.STRING },
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

// Super many to many.
User.belongsToMany(User, {through: UserFollowUser, as: 'Follows'});
UserFollowUser.belongsTo(User)
User.hasMany(UserFollowUser)

User.hasMany(Post);
Post.belongsTo(User);

await sequelize.sync({force: true});

// Create data.
const users = await User.bulkCreate([
  {name: 'user0'},
  {name: 'user1'},
  {name: 'user2'},
  {name: 'user3'},
])
const posts = await Post.bulkCreate([
  {body: 'body0', UserId: users[0].id},
  {body: 'body1', UserId: users[1].id},
  {body: 'body2', UserId: users[2].id},
  {body: 'body3', UserId: users[3].id},
  {body: 'body4', UserId: users[0].id},
  {body: 'body5', UserId: users[1].id},
  {body: 'body6', UserId: users[2].id},
  {body: 'body7', UserId: users[3].id},
])
await users[0].addFollows([users[1], users[2]])

// Get all the posts by authors that user0 follows.
// without any post process sorting. We only managed to to this
// with a super many to many, because that allows us to specify
// a reversed order in the through table with `on`, since we need to
// match with `FollowId` and not `UserId`.
{
  const postsFound = await Post.findAll({
    order: [[
      'body',
      'DESC'
    ]],
    include: [
      {
        model: User,
        attributes: [],
        required: true,
        include: [
          {
            model: UserFollowUser,
            on: {
              FollowId: {[Op.col]: 'User.id' },
            },
            attributes: [],
            where: {UserId: users[0].id},
          }
        ],
      },
    ],
  })
  assert.strictEqual(postsFound[0].body, 'body6')
  assert.strictEqual(postsFound[1].body, 'body5')
  assert.strictEqual(postsFound[2].body, 'body2')
  assert.strictEqual(postsFound[3].body, 'body1')
  assert.strictEqual(postsFound.length, 4)
}

await sequelize.close();
})();
