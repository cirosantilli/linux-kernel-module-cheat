#!/usr/bin/env node

// Trying to get everything in the database camel cased, columns lowercase, tables uppercase.
// The defaults documented on getting started documentation do uppercase foreign keys, an
// lowercase non-foreign keys. It's a mess.

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

// Create the tables.
const User = sequelize.define('User', {
  name: { type: DataTypes.STRING },
});
const Post = sequelize.define('Post', {
  body: { type: DataTypes.STRING },
});
const UserFollowsUser = sequelize.define('UserFollowsUser', {
  userId: {
    type: DataTypes.INTEGER,
    references: {
      model: User,
      key: 'id'
    }
  },
  followId: {
    type: DataTypes.INTEGER,
    references: {
      model: User,
      key: 'id'
    }
  },
});
User.belongsToMany(User, {
  through: UserFollowsUser,
  as: 'follows',
  foreignKey: 'userId',
  otherKey: 'followId',
});
User.hasMany(Post, {foreignKey: 'authorId'});
Post.belongsTo(User, {foreignKey: 'authorId'});
await sequelize.sync({force: true});

// Create data.
const users = await User.bulkCreate([
  {name: 'user0'},
  {name: 'user1'},
  {name: 'user2'},
  {name: 'user3'},
])

const posts = await Post.bulkCreate([
  {body: 'body00', authorId: users[0].id},
  {body: 'body01', authorId: users[0].id},
  {body: 'body10', authorId: users[1].id},
  {body: 'body11', authorId: users[1].id},
  {body: 'body20', authorId: users[2].id},
  {body: 'body21', authorId: users[2].id},
  {body: 'body30', authorId: users[3].id},
  {body: 'body31', authorId: users[3].id},
])
await users[0].addFollows([users[1], users[2]])

// Check case of auto-getters.

const user0Posts = await users[0].getPosts({order: [['body', 'ASC']]})
assert(user0Posts[0].body === 'body00')
assert(user0Posts[1].body === 'body01')
assert(user0Posts.length === 2)

const user1Posts = await users[1].getPosts({order: [['body', 'ASC']]})
assert(user1Posts[0].body === 'body10')
assert(user1Posts[1].body === 'body11')
assert(user1Posts.length === 2)

const post00User = await posts[0].getUser()
assert(post00User.name === 'user0')

const post01User = await posts[1].getUser()
assert(post01User.name === 'user0')

const post10User = await posts[2].getUser()
assert(post10User.name === 'user1')

const post11User = await posts[3].getUser()
assert(post11User.name === 'user1')

// Check case of as;
{
  const user0Follows = (await User.findByPk(users[0].id, {
    include: [
      {
        model: User,
        as: 'follows',
        include: [
          {
            model: Post,
          }
        ],
      },
    ],
  })).follows
  const postsFound = []
  for (const followedUser of user0Follows) {
    postsFound.push(...followedUser.Posts)
  }
  postsFound.sort((x, y) => { return x.body < y.body ? -1 : x.body > y.body ? 1 : 0 })
  assert(postsFound[0].body === 'body10')
  assert(postsFound[1].body === 'body11')
  assert(postsFound[2].body === 'body20')
  assert(postsFound[3].body === 'body21')
  assert(postsFound.length === 4)
}

await sequelize.close();
})();
