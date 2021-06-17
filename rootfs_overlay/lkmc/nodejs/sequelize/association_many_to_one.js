#!/usr/bin/env node

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
const Post = sequelize.define('Post', {
  body: { type: DataTypes.STRING },
}, {});
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
  {body: 'body00', UserId: users[0].id},
  {body: 'body01', UserId: users[0].id},
  {body: 'body10', UserId: users[1].id},
  {body: 'body11', UserId: users[1].id},
  {body: 'body20', UserId: users[2].id},
  {body: 'body21', UserId: users[2].id},
  {body: 'body30', UserId: users[3].id},
  {body: 'body31', UserId: users[3].id},
])

// Get user from post and vice versa.

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

await sequelize.close();
})();
