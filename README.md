harkness is a simple, bad, _colorful_ unit testing framework designed to make
me happy.

Using it is pretty simple. To create a unit test, first create a function
that looks like:

> void MY_FUNCTION(h_context \*ctx) {...}
> MY_FUNCTION can be whatever function name you want, but everything else must
> be exactly the same (yes the name of the context parameter is load-bearing,
> what of it?).

Then, annotate the function to add it to the list of testable functions:

> h_test(test_name, function name);
> void MY_FUNCTION(h_context \*ctx) {...}
> test_name is the name of the test (used for granular test control with
> comptime flags), and function_name is the name of the function you just made.

Finally, place the line `run_tests()` at the top of your main function.

To run your tests, you'll use specific compiler flags, the most basic being:
gcc my_program -D TEST_ALL

For more more information, see the rest of the documentation in this file
(particularly on `run_tests()`). You can also check out "example.c" for
examples of most functions in the library being used.

Happy testing!
