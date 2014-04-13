msg_queue
=========

What is msg_queue?

msg_queue is an open source framework that allows developers to perform fully asynchronous tasks to leverage their C++ applications to take advantage of the parallelism that is allowed by modern computers. 

msg_queue offers a type-safe and easy to use interface for passing messages (anything that is callable) with any numbers of parameters, similar to the parameter scheme for std::async. Regarding complexity, the msg_queue class is fairly simple, and offers only a handful of functions that include adding a message, checking the amount of jobs left, and implementing a workaround for the currently missing ( hopefully to be added in C++14) , future.then() call.

Why use msg_queue?

The answer is simple -- your programs should avoid blocking whenever possible. By passing your functions to this thread-safe message queue not only do you get a std::future in return ( much like calling std::async) thus allowing for this to be integrated nicely to C++11 concurrency comforing code, but also provides a fully functional workaround for the future.then() call, so your caller thread never blocks, even while waiting for the std::future to be filled. The workaround for future.then() accepts a future ( that has not been .get()'d) and a lambda that accepts as a parameter decltype(future.get()). The lambda is executed asynchronously, so your caller thread can specify exactly what to do with the return value from future.get() without incurring the penalty of waiting.

msg_queue also implements orderly shutdown, so any messages still in the queue will be executed/launched before the msg_queue object is destroyed, thus preventing forced shutdown and possible data corruption.

