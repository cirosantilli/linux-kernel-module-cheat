#!/usr/bin/env node

const assert = require('assert');
const path = require('path');

const { Sequelize, DataTypes } = require('sequelize');

const sequelize = new Sequelize({
  dialect: 'sqlite',
  storage: 'tmp.' + path.basename(__filename) + '.sqlite',
});

(async () => {
const Comment = sequelize.define('Comment', {
  body: { type: DataTypes.STRING },
}, {});
const User = sequelize.define('User', {
  name: { type: DataTypes.STRING },
}, {});
User.hasMany(Comment)
Comment.belongsTo(User)
await sequelize.sync({force: true});
const u0 = await User.create({name: 'u0'})
const u1 = await User.create({name: 'u1'})
await Comment.create({body: 'u0c0', UserId: u0.id});
await Comment.create({body: 'u0c1', UserId: u0.id});
await Comment.create({body: 'u1c0', UserId: u1.id});

// Direct way.
{
  const u0Comments = await Comment.findAll({
    where: { UserId: u0.id },
    order: [['id', 'ASC']],
  });
  assert(u0Comments[0].body === 'u0c0');
  assert(u0Comments[1].body === 'u0c1');
  assert(u0Comments[0].UserId === u0.id);
  assert(u0Comments[1].UserId === u0.id);
  // Not added as an object by default. Would require extra query.
  assert(u0Comments[0].User === undefined);
  assert(u0Comments[1].User === undefined);
}

// Include data from the other side of the association in the query.
{
  const u0Comments = await Comment.findAll({
    where: { UserId: u0.id },
    order: [['id', 'ASC']],
    include: User,
    // Equivalent alternatives in this case.
    //include: [User],
    //include: [{ model: User }],
  });
  assert(u0Comments[0].body === 'u0c0');
  assert(u0Comments[1].body === 'u0c1');
  assert(u0Comments[0].UserId === u0.id);
  assert(u0Comments[1].UserId === u0.id);
  // These did get added now.
  assert(u0Comments[0].User.name === 'u0');
  assert(u0Comments[1].User.name === 'u0');
}

// Nicer higher level way.
{
  console.log(Object.getOwnPropertyNames(u0));
  const u0Comments = await u0.getComments({
    include: [{ model: User }],
    order: [['id', 'ASC']],
  });
  assert(u0Comments[0].body === 'u0c0');
  assert(u0Comments[1].body === 'u0c1');
  assert(u0Comments[0].User.name === 'u0');
  assert(u0Comments[1].User.name === 'u0');
}

// No way to create new item with association without explicit foreign key??
// https://stackoverflow.com/questions/34059081/how-do-i-reference-an-association-when-creating-a-row-in-sequelize-without-assum
// This does not work as we would like:
{
  await Comment.create({body: 'u0c2', User: u0});
  // We'd want 3 here.
  assert((await Comment.findAll({
    where: { UserId: u0.id },
  })).length === 2);
}

// Removal auto-cascades.
{
  const u0id = u0.id
  await u0.destroy()
  assert((await Comment.findAll({
    where: { UserId: u0id },
  })).length === 0);
  assert((await Comment.findAll({
    where: { UserId: u1.id },
  })).length === 1);
}

// as aliases.
// Allows us to use a nicer name for a relation rather than the exact class name.
// E.g. here we name the User of a Comment as a "author".
// And mandatory do diambiguate multiple associations with a single type.
{
  const CommentAs = sequelize.define('CommentAs', {
    body: { type: DataTypes.STRING },
  }, {});
  const UserAs = sequelize.define('UserAs', {
    name: { type: DataTypes.STRING },
  }, {});
  UserAs.hasMany(CommentAs)
  CommentAs.belongsTo(UserAs, {as: 'author'})
  await sequelize.sync({force: true});
  const u0 = await UserAs.create({name: 'u0'})
  const u1 = await UserAs.create({name: 'u1'})
  await CommentAs.create({body: 'u0c0', authorId: u0.id});
  await CommentAs.create({body: 'u0c1', authorId: u0.id});
  await CommentAs.create({body: 'u1c0', authorId: u1.id});

  {
    const u0Comments = await CommentAs.findAll({
      where: { authorId: u0.id },
      order: [['id', 'ASC']],
      // Instead of include: UserAs
      include: 'author',
    });
    assert(u0Comments[0].body === 'u0c0');
    assert(u0Comments[1].body === 'u0c1');
    assert(u0Comments[0].authorId === u0.id);
    assert(u0Comments[1].authorId === u0.id);
    assert(u0Comments[0].author.name === 'u0');
    assert(u0Comments[1].author.name === 'u0');
  }

  // Trying with the higher level getter.
  {
    // TODO
    // u0.getComments is not a function
    //const u0Comments = await u0.getComments({
    //  include: 'author',
    //});
    //assert(u0Comments[0].body === 'u0c0');
    //assert(u0Comments[1].body === 'u0c1');
    //assert(u0Comments[0].author.name === 'u0');
    //assert(u0Comments[1].author.name === 'u0');
  }
}
await sequelize.close();
})();
