// https://cirosantilli.com/linux-kernel-module-cheat#cpp-multithreading
//
// http://stackoverflow.com/questions/7686939/c-simple-return-value-from-stdthread
// http://stackoverflow.com/questions/28950835/c-error-no-type-named-type-in-class-stdresult-ofvoid-stdunordered
// http://stackoverflow.com/questions/21048906/stdthread-pass-by-reference-calls-copy-constructor
// http://stackoverflow.com/questions/8299545/passing-arguments-to-thread-function
// http://stackoverflow.com/questions/5116756/difference-between-pointer-and-reference-as-thread-parameter

#include <cassert>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

int myfunc(int i) {
    return i + 1;
}

void myfunc_reference(int& i) {
    i = myfunc(i);
}

int main(int argc, char **argv) {
    unsigned int nthreads;

    // CLI arguments.
    if (argc > 1) {
        nthreads = std::strtoll(argv[1], NULL, 0);
    } else {
        nthreads = 1;
    }

    // Setup inputs and expected outputs.
    std::vector<int> inputs;
    std::vector<int> outputs_expect;
    for (unsigned int i = 0; i < nthreads; ++i) {
        inputs.push_back(i);
        outputs_expect.push_back(myfunc(i));
    }

    // future and sync. Nirvana. When you are not fighting to death with types:
    // https://stackoverflow.com/questions/10620300/can-stdasync-be-use-with-template-functions
    {
        std::vector<std::future<int>> futures(nthreads);
        std::vector<int> outputs(nthreads);
        for (decltype(futures)::size_type i = 0; i < nthreads; ++i) {
            futures[i] = std::async(
                myfunc,
                inputs[i]
            );
        }
        for (decltype(futures)::size_type i = 0; i < nthreads; ++i) {
            outputs[i] = futures[i].get();
        }
        assert(outputs_expect == outputs);
    }

    // Reference arguments.
    //
    // Annoying because requires:
    //
    // - wrapping the return function to accept references
    // - keeping an array of outputs
    // - std::ref
    {
        std::vector<std::thread> threads(nthreads);
        std::vector<int> inouts(inputs);
        for (decltype(threads)::size_type i = 0; i < nthreads; ++i) {
            threads[i] = std::thread(myfunc_reference, std::ref(inouts[i]));
        }
        for (auto& thread : threads) {
            thread.join();
        }
        assert(outputs_expect == inouts);
    }
}
