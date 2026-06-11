// Don't @ me

// What is a key difference between container.at(index) and container[index] in C++?

/*
.at() performs bounds checking, while [] doesn't.

.at() is slower than operator[] because it performs a bounds check which adds overhead. Prefer to use .at() when safety matters and you want bounds checking with exceptions. Otherwise, use operator[] when performance matters and you can guarantee the index is valid.
*/