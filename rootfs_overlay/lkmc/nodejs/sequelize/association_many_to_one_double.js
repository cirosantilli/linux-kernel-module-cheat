#!/usr/bin/env node

// Two associations between two models: posts
// now have the author, and a mandatory reviewer.
// Requires us to use as do disambiguate them.

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
}, {});
const Post = sequelize.define('Post', {
  body: { type: DataTypes.STRING },
}, {});

// TODO possible without specifying foreignKey? Feels like duplication.
// But without it, hasMany creates a column called UserId, in addition
// to the desired authorId, and then bad things happen.
User.hasMany(Post, {as: 'authoredPosts', foreignKey: 'authorId'});
Post.belongsTo(User, {as: 'author', foreignKey: 'authorId'});

User.hasMany(Post, {as: 'reviewedPosts', foreignKey: 'reviewerId'});
Post.belongsTo(User, {as: 'reviewer', foreignKey: 'reviewerId'});
await sequelize.sync({force: true});

// Create data.
const users = await User.bulkCreate([
  {name: 'user0'},
  {name: 'user1'},
  {name: 'user2'},
  {name: 'user3'},
])

const posts = await Post.bulkCreate([
  {body: 'body00', authorId: users[0].id, reviewerId: users[0].id},
  {body: 'body01', authorId: users[0].id, reviewerId: users[1].id},
  {body: 'body10', authorId: users[1].id, reviewerId: users[2].id},
  {body: 'body11', authorId: users[1].id, reviewerId: users[3].id},
  {body: 'body20', authorId: users[2].id, reviewerId: users[0].id},
  {body: 'body21', authorId: users[2].id, reviewerId: users[1].id},
  {body: 'body30', authorId: users[3].id, reviewerId: users[2].id},
  {body: 'body31', authorId: users[3].id, reviewerId: users[3].id},
])

// Get user from post and vice versa.

const user0AuthoredPosts = await users[0].getAuthoredPosts()
assert(user0AuthoredPosts[0].body === 'body00')
assert(user0AuthoredPosts[1].body === 'body01')
assert(user0AuthoredPosts.length === 2)

const user1AuthoredPosts = await users[1].getAuthoredPosts()
assert(user1AuthoredPosts[0].body === 'body10')
assert(user1AuthoredPosts[1].body === 'body11')
assert(user1AuthoredPosts.length === 2)

const user0ReviewedPosts = await users[0].getReviewedPosts()
assert(user0ReviewedPosts[0].body === 'body00')
assert(user0ReviewedPosts[1].body === 'body20')
assert(user0ReviewedPosts.length === 2)

const user1ReviewedPosts = await users[1].getReviewedPosts()
assert(user1ReviewedPosts[0].body === 'body01')
assert(user1ReviewedPosts[1].body === 'body21')
assert(user1ReviewedPosts.length === 2)

assert((await posts[0].getAuthor()).name === 'user0')
assert((await posts[1].getAuthor()).name === 'user0')
assert((await posts[2].getAuthor()).name === 'user1')
assert((await posts[3].getAuthor()).name === 'user1')

assert((await posts[0].getReviewer()).name === 'user0')
assert((await posts[1].getReviewer()).name === 'user1')
assert((await posts[2].getReviewer()).name === 'user2')
assert((await posts[3].getReviewer()).name === 'user3')

// Same as getAuthoredPosts but with the user ID instead of the model object.
{
  const user0AuthoredPosts = await Post.findAll({
    include: [{
      model: User,
      // We need the `as` here to disambiguate.
      as: 'author',
      where: {id: users[0].id},
    }],
    order: [['body', 'ASC']],
  })
  assert(user0AuthoredPosts[0].body === 'body00');
  assert(user0AuthoredPosts[1].body === 'body01');
  assert(user0AuthoredPosts.length === 2);
}

// Yet another way that can be more useful in nested includes.
{
  const user0AuthoredPosts = (await User.findOne({
    where: {id: users[0].id},
    include: [{
      model: Post,
      as: 'authoredPosts',
    }],
    order: [[{model: Post, as: 'authoredPosts'}, 'body', 'ASC']],
  })).authoredPosts
  assert(user0AuthoredPosts[0].body === 'body00');
  assert(user0AuthoredPosts[1].body === 'body01');
  assert(user0AuthoredPosts.length === 2);
}

await sequelize.close();
})();
