To run, simply do `make output.txt`. It should generate the output file.

It's a bit unclear whether 'foo.bar.baz' is a single word or not; `wc` treats
it as a single word, whereas we do not (because the `.` is a separator). That
should give some difference for input containing e.g. e-mail addresses. That's
probably why `test_a5.txt` give different output.
