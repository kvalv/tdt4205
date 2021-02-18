The source code for the first task is found in `task_a.l`. The source
code for the second task is found in `compilers-pe2.l`. Sorry for the
inconsistent names...

To run, simply do `make output.txt`. It should generate the output file.

To run only for task 2.1, you can do `make prg-2.1 && ./run-tests prg-2.1`. The
first argument in `run-tests` will specify which program you use, and `prg-2.2`
is the default if unspecified.

It's a bit unclear whether 'foo.bar.baz' is a single word or not; `wc` treats
it as a single word, whereas we do not (because the `.` is a separator). That
should give some difference for input containing e.g. e-mail addresses. That's
probably why `test_a5.txt` give different output.

Finally, for 2.2 we were not sure whether `""` was a word or not. We decided it
was not a word because inside of the string it was empty. If it was a word we
would also have to treat some other special sequences as words as well, e.g.
`""foo"` (because that is also a sequence of characters not separated by
whitespace or punctuation). So we just kept it simple and decided not to
consider it a word, as we assume that was also the intention in the task.
