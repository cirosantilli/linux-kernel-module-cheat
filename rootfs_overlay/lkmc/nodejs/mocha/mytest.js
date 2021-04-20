var assert = require('assert');

describe('describe0', function() {
  // Only runs before the current describe.
  before(async () => {
    console.error('before describe 0');
  });
  beforeEach(async () => {
    console.error('beforeEach describe 0');
  });
  it('it 0 0', function() {
    assert.equal(0, 0);
  });
  it('it 0 1', function() {
    assert.equal(0, 0);
  });

  describe('describe 0 0', function() {
    before(async () => {
      console.error('before describe 0 0');
    });
    beforeEach(async () => {
      console.error('beforeEach describe 0 0');
    });
    it('it 0 0 0', function() {
      assert.equal(0, 0);
    });
    it('it 0 0 1', function() {
      assert.equal(0, 0);
    });
  });

  describe('describe 0 1', function() {
    before(async () => {
      console.error('before describe 0 1');
    });
    beforeEach(async () => {
      console.error('beforeEach describe 0 1');
    });
    it('it 0 1 0', function() {
      assert.equal(0, 0);
    });
    it('it 0 1 1', function() {
      assert.equal(0, 0);
    });
  });
});
