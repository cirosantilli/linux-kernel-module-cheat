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

function myhelper() {
  console.error('myhelper');
}

exports.myhelper = myhelper;
