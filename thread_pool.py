#!/usr/bin/env python3

from typing import Any, Callable, Dict, Iterable, Union
import multiprocessing
import queue
import sys
import threading
import time

class ThreadPool:
    '''
    Start a pool of a limited number of threads to do some work.

    This is similar to the stdlib concurrent, but I could not find
    how to reach all my design goals with that implementation:

    - the input function does not need to be modified
    - limit the number of threads
    - queue sizes closely follow number of threads
    - if an exception happens, optionally stop soon afterwards

    Functional form and further discussion at:
    https://stackoverflow.com/questions/19369724/the-right-way-to-limit-maximum-number-of-threads-running-at-once/55263676#55263676

    This class form allows to use your own while loops with submit().

    Quick test with:

        ./thread_limit.py 2 -10 20 0
        ./thread_limit.py 2 -10 20 1
        ./thread_limit.py 2 -10 20 2
        ./thread_limit.py 2 -10 20 3

    These ensure that execution stops neatly on error.
    '''
    def __init__(
        self,
        func: Callable,
        handle_output: Union[Callable[[Any,Any,Exception],Any],None] = None,
        nthreads: Union[int,None] = None
    ):
        '''
        Start in a thread pool immediately.

        join() must be called afterwards at some point.

        :param func: main work function to be evaluated.
        :param handle_output: called on func return values as they
            are returned.

            Signature is: handle_output(input, output, exception) where:

            - input: input given to func
            - output: return value of func
            - exception: the exception that func raised, or None otherwise

            If this function returns non-None or raises, stop feeding
            new input and exit ASAP when all currently running threads
            have finished.

            Default: a handler that does nothing and just exits on exception.
        :param nthreads: number of threads to use. Default: nproc.
        '''
        self.func = func
        if handle_output is None:
            handle_output = lambda input, output, exception: exception
        self.handle_output = handle_output
        if nthreads is None:
            nthreads = multiprocessing.cpu_count()
        self.nthreads = nthreads
        self.error_output = None
        self.error_output_lock = threading.Lock()
        self.in_queue = queue.Queue(maxsize=nthreads)
        self.threads = []
        for i in range(self.nthreads):
            thread = threading.Thread(
                target=self._func_runner,
            )
            self.threads.append(thread)
            thread.start()

    def submit(self, work):
        '''
        Submit work. Block if there is already enough work scheduled (~nthreads).

        :return: if an error occurred in some previously executed thread, the error.
                 Otherwise, None. This allows the caller to stop submitting further
                 work if desired.
        '''
        self.in_queue.put(work)
        return self.error_output

    def join(self):
        '''
        Request all threads to stop after they finish currently submitted work.

        :return: same as submit()
        '''
        for thread in range(self.nthreads):
            self.in_queue.put(None)
        for thread in self.threads:
            thread.join()
        return self.error_output

    def _func_runner(self):
        while True:
            work = self.in_queue.get(block=True)
            if work is None:
                break
            try:
                exception = None
                out = self.func(**work)
            except Exception as e:
                exception = e
            try:
                handle_output_return = self.handle_output(work, out, exception)
            except Exception as e:
                self.error_output_lock.acquire()
                self.error_output = (work, out, e)
                self.error_output_lock.release()
            else:
                if handle_output_return is not None:
                    self.error_output_lock.acquire()
                    self.error_output = handle_output_return
                    self.error_output_lock.release()
            finally:
                self.in_queue.task_done()

if __name__ == '__main__':
    def my_func(i):
        '''
        The main function that will be evaluated.

        It sleeps to simulate an IO operation.
        '''
        time.sleep((abs(i) % 4) / 10.0)
        return 10.0 / i

    def get_work(min_, max_):
        '''
        Generate simple range work for my_func.
        '''
        for i in range(min_, max_):
            yield {'i': i}

    def handle_output_print(input, output, exception):
        '''
        Print outputs and exit immediately on failure.
        '''
        print('{!r} {!r} {!r}'.format(input, output, exception))
        return exception

    def handle_output_print_no_exit(input, output, exception):
        '''
        Print outputs, don't exit on failure.
        '''
        print('{!r} {!r} {!r}'.format(input, output, exception))

    out_queue = queue.Queue()
    def handle_output_queue(input, output, exception):
        '''
        Store outputs in a queue for later usage.
        '''
        global out_queue
        out_queue.put((input, output, exception))
        return exception

    def handle_output_raise(input, output, exception):
        '''
        Raise if input == 10, to test that execution
        stops nicely if this raises.
        '''
        print('{!r} {!r} {!r}'.format(input, output, exception))
        if input['i'] == 10:
            raise Exception

    # CLI arguments.
    argv_len = len(sys.argv)
    if argv_len > 1:
        nthreads = int(sys.argv[1])
        if nthreads == 0:
            nthreads = None
    else:
        nthreads = None
    if argv_len > 2:
        min_ = int(sys.argv[2])
    else:
        min_ = 1
    if argv_len > 3:
        max_ = int(sys.argv[3])
    else:
        max_ = 100
    if argv_len > 4:
        c = sys.argv[4][0]
    else:
        c = '0'
    if c == '1':
        handle_output = handle_output_print_no_exit
    elif c == '2':
        handle_output = handle_output_queue
    elif c == '3':
        handle_output = handle_output_raise
    else:
        handle_output = handle_output_print

    # Action.
    thread_pool = ThreadPool(
        my_func,
        handle_output,
        nthreads
    )
    for work in get_work(min_, max_):
        error = thread_pool.submit(work)
        if error is not None:
            break
    error = thread_pool.join()
    if error is not None:
        print('error: {!r}'.format(error))
    if handle_output == handle_output_queue:
        while not out_queue.empty():
            print(out_queue.get())
