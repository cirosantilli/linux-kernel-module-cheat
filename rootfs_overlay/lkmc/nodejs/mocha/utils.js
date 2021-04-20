// Global fixture.
exports.mochaGlobalSetup = async function() {
  console.log('mochaGlobalSetup');
};

// Root hook.
exports.mochaHooks = {
  before(done) {
    console.log('mochaHooks.before');
    done();
  },
  beforeEach(done) {
    console.log('mochaHooks.beforeEach');
    done();
  },
};

// TODO don't know a better way to make this available
// to test files than just requiring it by relative path.
function myhelper() {
  console.error('myhelper');
}
exports.myhelper = myhelper;
