<img src='./GCAT.png' width=100% alt="GCAT logo">

# GCAT Memory Allocator

Automate memory allocation in C so that you can think less about how your program runs on your targets, and think more about your program.

## How GCAT Works

GCAT uses two reference counters, one tracking references within its heap, one for tracking references outside of it. The two functions gall and burr_stack, when used alone, are a special case of this algorithm which is equal to malloc and free when used normally. However, using hew_stack and hew_heap, as well as matching uses of burr_stack and burr_heap, memory management can be automated even with difficult, cyclic dependencies. A finalizer system can ensure that pointers in your requested memory have their references correctly tracked, and finalizers can also automate other tasks such as closing file descriptors when something hiding them is no longer in use.

Some unique aspects of GCAT are that on top of allowing full automation of freeing memory which would leak, it will try allocating memory within the most recently freed block first, improving spatial locality. It also is modular, and uses dependency injection to increase portability.

This standalone allocation system can be built into your application directly to benefit from optimizations and modify to your needs, or it can be used as a redistributable dynamically linked library.
