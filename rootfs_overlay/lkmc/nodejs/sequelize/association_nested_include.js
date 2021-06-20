#!/usr/bin/env node

// Find all posts by users that a given user follows.
// https://stackoverflow.com/questions/42632943/sequelize-multiple-where-clause

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
User.belongsToMany(User, {through: 'UserFollowUser', as: 'Follows'});
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

await users[0].addFollows([users[1], users[2]])

// Get all posts by authors that user0 follows.
// The posts are placed inside their respetive authors under .Posts
// so we loop to gather all of them.
{
  const user0Follows = (await User.findByPk(users[0].id, {
    include: [
      {
        model: User,
        as: 'Follows',
        include: [
          {
            model: Post,
          }
        ],
      },
    ],
  })).Follows
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

// With ordering, offset and limit.
// The posts are placed inside their respetive authors under .Posts
// The only difference is that posts that we didn't select got removed.

{
  const user0Follows = (await User.findByPk(users[0].id, {
    offset: 1,
    limit: 2,
    // TODO why is this needed? It does try to make a subquery otherwise, and then it doesn't work.
    // https://selleo.com/til/posts/ddesmudzmi-offset-pagination-with-subquery-in-sequelize-
    subQuery: false,
    include: [
      {
        model: User,
        as: 'Follows',
        include: [
          {
            model: Post,
          }
        ],
      },
    ],
  })).Follows
  assert(user0Follows[0].name === 'user1')
  assert(user0Follows[1].name === 'user2')
  assert(user0Follows.length === 2)
  const postsFound = []
  for (const followedUser of user0Follows) {
    postsFound.push(...followedUser.Posts)
  }
  postsFound.sort((x, y) => { return x.body < y.body ? -1 : x.body > y.body ? 1 : 0 })
  assert(postsFound[0].body === 'body11')
  assert(postsFound[1].body === 'body20')
  assert(postsFound.length === 2)

  // Same as above, but now with DESC ordering.
  {
    const user0Follows = (await User.findByPk(users[0].id, {
      order: [[
        {model: User, as: 'Follows'},
        Post,
        'body',
        'DESC'
      ]],
      offset: 1,
      limit: 2,
      subQuery: false,
      include: [
        {
          model: User,
          as: 'Follows',
          include: [
            {
              model: Post,
            }
          ],
        },
      ],
    })).Follows
    // Note how user ordering is also reversed from an ASC.
    // it likely takes the use that has the first post.
    assert(user0Follows[0].name === 'user2')
    assert(user0Follows[1].name === 'user1')
    assert(user0Follows.length === 2)
    const postsFound = []
    for (const followedUser of user0Follows) {
      postsFound.push(...followedUser.Posts)
    }
    // In this very specific data case, this would not be needed.
    // because user2 has the second post body and user1 has the first
    // alphabetically.
    postsFound.sort((x, y) => { return x.body < y.body ? 1 : x.body > y.body ? -1 : 0 })
    // Note that what happens is that some of the
    assert(postsFound[0].body === 'body20')
    assert(postsFound[1].body === 'body11')
    assert(postsFound.length === 2)
  }

  // Here user2 would have no post hits due to the limit,
  // so it is entirely pruned from the user list as desired.
  // Otherwise we would fetch a lot of unwanted user data
  // in a large database.
  const user0FollowsLimit2 = (await User.findByPk(users[0].id, {
    limit: 2,
    subQuery: false,
    include: [
      {
        model: User,
        as: 'Follows',
        include: [ { model: Post } ],
      },
    ],
  })).Follows
  assert(user0FollowsLimit2[0].name === 'user1')
  assert(user0FollowsLimit2.length === 1)

  // Get just the count of the posts authored by useres followed by user0.
  // attributes: [] excludes all other data from the SELECT of the querries
  // to optimize things a bit.
  // https://stackoverflow.com/questions/37817808/counting-associated-entries-with-sequelize
  {
    const user0Follows = await User.findByPk(users[0].id, {
      attributes: [
        [Sequelize.fn('COUNT', Sequelize.col('Follows.Posts.id')), 'count']
      ],
      include: [
        {
          model: User,
          as: 'Follows',
          attributes: [],
          through: {attributes: []},
          include: [{
              model: Post,
              attributes: [],
          }],
        },
      ],
    })
    assert.strictEqual(user0Follows.dataValues.count, 4);
  }

  // Case in which our post-sorting is needed.
  // TODO: possible to get sequelize to do this for us by returning
  // a flat array directly?
  // It's not big deal since the LIMITed result should be small,
  // but feels wasteful.
  // https://stackoverflow.com/questions/41502699/return-flat-object-from-sequelize-with-association
  // https://github.com/sequelize/sequelize/issues/4419
  {
    await Post.truncate({restartIdentity: true})
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
    const user0Follows = (await User.findByPk(users[0].id, {
      order: [[
        {model: User, as: 'Follows'},
        Post,
        'body',
        'DESC'
      ]],
      subQuery: false,
      include: [
        {
          model: User,
          as: 'Follows',
          include: [
            {
              model: Post,
            }
          ],
        },
      ],
    })).Follows
    assert(user0Follows[0].name === 'user2')
    assert(user0Follows[1].name === 'user1')
    assert(user0Follows.length === 2)
    const postsFound = []
    for (const followedUser of user0Follows) {
      postsFound.push(...followedUser.Posts)
    }
    // We need this here, otherwise we would get all user2 posts first:
    // body6, body2, body5, body1
    postsFound.sort((x, y) => { return x.body < y.body ? 1 : x.body > y.body ? -1 : 0 })
    assert(postsFound[0].body === 'body6')
    assert(postsFound[1].body === 'body5')
    assert(postsFound[2].body === 'body2')
    assert(postsFound[3].body === 'body1')
    assert(postsFound.length === 4)
  }

  //// This almost achieves the flat array return. We just have to understand the undocumented custom on:
  //// to specify from which side of the UserFollowsUser we are coming. The on:
  //// is ignored without super many to many unfortunately, the below just returns all posts.
  {
    const postsFound = await Post.findAll({
      order: [[
        'body',
        'DESC'
      ]],
      subQuery: false,
      include: [
        {
          model: User,
          //on: {idasdf: '$Post.User.FollowId$'},
          include: [
            {
              model: User,
              as: 'Follows',
              where: {id: users[0].id},
            }
          ],
        },
      ],
    })
    console.error(postsFound.length);
    //assert.strictEqual(postsFound[0].body, 'body6')
    //assert.strictEqual(postsFound[1].body, 'body5')
    //assert.strictEqual(postsFound[2].body, 'body2')
    //assert.strictEqual(postsFound[3].body, 'body1')
    assert.strictEqual(postsFound.length, 4)
  }
}

await sequelize.close();
})();
