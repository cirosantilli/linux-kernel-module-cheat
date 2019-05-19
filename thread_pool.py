#!/usr/bin/env python3

'''
This file is MIT Licensed because I'm posting it on Stack Overflow:
https://stackoverflow.com/questions/19369724/the-right-way-to-limit-maximum-number-of-threads-running-at-once/55263676#55263676
'''

from typing import Any, Callable, Dict, Iterable, Union
import os
import queue
import sys
import threading
import time
import traceback

class ThreadPoolExitException(Exception):
    '''
    An object of this class may be raised by output_handler_function to
    request early termination.

    It is also raised by submit() if submit_raise_exit=True.
    '''
    pass

class ThreadPool:
    '''
    Start a pool of a limited number of threads to do some work.

    This is similar to the stdlib concurrent, but I could not find
    how to reach all my design goals with that implementation:

    * the input function does not need to be modified
    * limit the number of threads
    * queue sizes closely follow number of threads
    * if an exception happens, optionally stop soon afterwards

    This class form allows to use your own while loops with submit().

    Exit soon after the first failure happens:

    ....
    python3 thread_pool.py 2 -10 20 handle_output_print
    ....

    Sample output:

    ....
    {'i': -9} -1.1111111111111112 None
    {'i': -8} -1.25 None
    {'i': -10} -1.0 None
    {'i': -6} -1.6666666666666667 None
    {'i': -7} -1.4285714285714286 None
    {'i': -4} -2.5 None
    {'i': -5} -2.0 None
    {'i': -2} -5.0 None
    {'i': -3} -3.3333333333333335 None
    {'i': 0} None ZeroDivisionError('float division by zero')
    {'i': -1} -10.0 None
    {'i': 1} 10.0 None
    {'i': 2} 5.0 None
    work_function or handle_output raised:
    Traceback (most recent call last):
      File "thread_pool.py", line 181, in _func_runner
        work_function_return = self.work_function(**work_function_input)
      File "thread_pool.py", line 281, in work_function_maybe_raise
        return 10.0 / i
    ZeroDivisionError: float division by zero
    work_function_input: {'i': 0}
    work_function_return: None
    ....

    Don't exit after first failure, run until end:

    ....
    python3 thread_pool.py 2 -10 20 handle_output_print_no_exit
    ....

    Store results in a queue for later inspection instead of printing immediately,
    then print everything at the end:

    ....
    python3 thread_pool.py 2 -10 20 handle_output_queue
    ....

    Exit soon after the handle_output raise.

    ....
    python3 thread_pool.py 2 -10 20 handle_output_raise
    ....

    Relying on this interface to abort execution is discouraged, this should
    usually only happen due to a programming error in the handler.

    Test that the argument called "thread_id" is passed to work_function and printed:

    ....
    python3 thread_pool.py 2 -10 20 handle_output_print thread_id
    ....

    Test with, ThreadPoolExitException and submit_raise_exit=True, same behaviour handle_output_print
    except for the different exit cause report:

    ....
    python3 thread_pool.py 2 -10 20 handle_output_raise_exit_exception
    ....
    '''
    def __init__(
        self,
        work_function: Callable,
        handle_output: Union[Callable[[Any,Any,Exception],Any],None] = None,
        nthreads: Union[int,None] = None,
        thread_id_arg: Union[str,None] = None,
        submit_raise_exit: bool = False,
        submit_skip_exit: bool = False,
    ):
        '''
        Start in a thread pool immediately.

        join() must be called afterwards at some point.

        :param work_function: main work function to be evaluated.
        :param handle_output: called on work_function return values as they
            are returned.

            The function signature is:

            ....
            handle_output(
                work_function_input: Union[Dict,None],
                work_function_return,
                work_function_exception: Exception
            ) -> Union[Exception,None]
            ....

            where work_function_exception the exception that work_function raised,
            or None otherwise

            The first non-None return value of a call to this function is returned by
            submit(), get_handle_output_result() and join().

            The intended semantic for this, is to return:

            *   on success:
            ** None to continue execution
            ** ThreadPoolExitException() to request stop execution
            * if work_function_input or work_function_exception raise:
            ** the exception raised

            The ThreadPool user can then optionally terminate execution early on error
            or request with either:

            * an explicit submit() return value check + break if a submit loop is used
            * `with` + submit_raise_exit=True

            Default: a handler that just returns `exception`, which can normally be used
            by the submit loop to detect an error and exit immediately.
        :param nthreads: number of threads to use. Default: nproc.
        :param thread_id_arg: if not None, set the argument of work_function with this name
            to a 0-indexed thread ID. This allows function calls to coordinate
            usage of external resources such as files or ports.
        :param submit_raise_exit: if True, submit() raises ThreadPoolExitException() if
            get_handle_output_result() is not None.
        :param submit_skip_exit: if True, submit() does nothing if
            get_handle_output_result() is not None.

            You should avoid this interface if
            you can use use submit_raise_exit with `with` instead ideally.

            However, when you can't work with with and are in a deeply nested loop,
            it might just be easier to set this.
        '''
        self.work_function = work_function
        if handle_output is None:
            handle_output = lambda input, output, exception: exception
        self.handle_output = handle_output
        if nthreads is None:
            nthreads = len(os.sched_getaffinity(0))
        self.thread_id_arg = thread_id_arg
        self.submit_raise_exit = submit_raise_exit
        self.submit_skip_exit = submit_skip_exit
        self.nthreads = nthreads
        self.handle_output_result = None
        self.handle_output_result_lock = threading.Lock()
        self.in_queue = queue.Queue(maxsize=nthreads)
        self.threads = []
        for i in range(self.nthreads):
            thread = threading.Thread(
                target=self._func_runner,
                args=(i,)
            )
            self.threads.append(thread)
            thread.start()

    def __enter__(self):
        '''
        __exit__ automatically calls join() for you.

        This is cool because it automatically ends the loop if an exception occurs.

        But don't forget that errors may happen after the last submit was called, so you
        likely want to check for that with get_handle_output_result() after the with.
        '''
        return self

    def __exit__(self, exception_type, exception_value, exception_traceback):
        self.join()
        return exception_type is ThreadPoolExitException

    def _func_runner(self, thread_id):
        while True:
            work_function_input = self.in_queue.get(block=True)
            if work_function_input is None:
                break
            if self.thread_id_arg is not None:
                work_function_input[self.thread_id_arg] = thread_id
            try:
                work_function_exception = None
                work_function_return = self.work_function(**work_function_input)
            except Exception as e:
                work_function_exception = e
                work_function_return = None
            handle_output_exception = None
            try:
                handle_output_return = self.handle_output(
                    work_function_input,
                    work_function_return,
                    work_function_exception
                )
            except Exception as e:
                handle_output_exception = e
            handle_output_result = None
            if handle_output_exception is not None:
                handle_output_result = handle_output_exception
            elif handle_output_return is not None:
                handle_output_result = handle_output_return
            if handle_output_result is not None and self.handle_output_result is None:
                with self.handle_output_result_lock:
                    self.handle_output_result = (
                        work_function_input,
                        work_function_return,
                        handle_output_result
                    )
            self.in_queue.task_done()

    @staticmethod
    def exception_traceback_string(exception):
        '''
        Helper to get the traceback from an exception object.
        This is usually what you want to print if an error happens in a thread:
        https://stackoverflow.com/questions/3702675/how-to-print-the-full-traceback-without-halting-the-program/56199295#56199295
        '''
        return ''.join(traceback.format_exception(
            None, exception, exception.__traceback__)
        )

    def get_handle_output_result(self):
        '''
        :return: if a handle_output call has raised previously, return a tuple:

            ....
            (work_function_input, work_function_return, exception_raised)
            ....

            corresponding to the first such raise.

            Otherwise, if a handle_output returned non-None, a tuple:

            (work_function_input, work_function_return, handle_output_return)

            Otherwise, None.
        '''
        return self.handle_output_result

    def join(self):
        '''
        Request all threads to stop after they finish currently submitted work.

        :return: same as get_handle_output_result()
        '''
        for thread in range(self.nthreads):
            self.in_queue.put(None)
        for thread in self.threads:
            thread.join()
        return self.get_handle_output_result()

    def submit(
        self,
        work_function_input: Union[Dict,None] =None
    ):
        '''
        Submit work. Block if there is already enough work scheduled (~nthreads).

        :return: the same as get_handle_output_result
        '''
        handle_output_result = self.get_handle_output_result()
        if handle_output_result is not None:
            if self.submit_raise_exit:
                raise ThreadPoolExitException()
            if self.submit_skip_exit:
                return handle_output_result
        if work_function_input is None:
            work_function_input = {}
        self.in_queue.put(work_function_input)
        return handle_output_result

if __name__ == '__main__':
    def get_work(min_, max_):
        '''
        Generate simple range work for work_function.
        '''
        for i in range(min_, max_):
            yield {'i': i}

    def work_function_maybe_raise(i):
        '''
        The main function that will be evaluated.

        It sleeps to simulate an IO operation.
        '''
        time.sleep((abs(i) % 4) / 10.0)
        return 10.0 / i

    def work_function_get_thread(i, thread_id):
        time.sleep((abs(i) % 4) / 10.0)
        return thread_id

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
        Raise if input == 0, to test that execution
        stops nicely if this raises.
        '''
        print('{!r} {!r} {!r}'.format(input, output, exception))
        if input['i'] == 0:
            raise Exception

    def handle_output_raise_exit_exception(input, output, exception):
        '''
        Return a ThreadPoolExitException() if input == -5.
        Return the work_function exception if it raised.
        '''
        print('{!r} {!r} {!r}'.format(input, output, exception))
        if exception:
            return exception
        if output == 10.0 / -5:
            return ThreadPoolExitException()

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
        handle_output_funtion_string = sys.argv[4]
    else:
        handle_output_funtion_string = 'handle_output_print'
    handle_output = eval(handle_output_funtion_string)
    if argv_len > 5:
        work_function = work_function_get_thread
        thread_id_arg = sys.argv[5]
    else:
        work_function = work_function_maybe_raise
        thread_id_arg = None

    # Action.
    if handle_output is handle_output_raise_exit_exception:
        # `with` version with implicit join and submit raise
        # immediately when desired with ThreadPoolExitException.
        #
        # This is the more safe and convenient and DRY usage if
        # you can use `with`, so prefer it generally.
        with ThreadPool(
            work_function,
            handle_output,
            nthreads,
            thread_id_arg,
            submit_raise_exit=True
        ) as my_thread_pool:
            for work in get_work(min_, max_):
                my_thread_pool.submit(work)
        handle_output_result = my_thread_pool.get_handle_output_result()
    else:
        # Explicit error checking in submit loop to exit immediately
        # on error.
        my_thread_pool = ThreadPool(
            work_function,
            handle_output,
            nthreads,
            thread_id_arg,
        )
        for work_function_input in get_work(min_, max_):
            handle_output_result = my_thread_pool.submit(work_function_input)
            if handle_output_result is not None:
                break
        handle_output_result = my_thread_pool.join()
    if handle_output_result is not None:
        work_function_input, work_function_return, exception = handle_output_result
        if type(exception) is ThreadPoolExitException:
            print('Early exit requested by handle_output with ThreadPoolExitException:')
        else:
            print('work_function or handle_output raised:')
            print(ThreadPool.exception_traceback_string(exception), end='')
        print('work_function_input: {!r}'.format(work_function_input))
        print('work_function_return: {!r}'.format(work_function_return))
    if handle_output == handle_output_queue:
        while not out_queue.empty():
            print(out_queue.get())
