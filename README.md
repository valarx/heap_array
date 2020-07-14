# heap_array

An stl-compatible implementation of non-resizeable array allocated on heap. Unlike `std::vector` it stores only size, hence it may save some space when size of the container itself is critical, but this leads to the fact it works only on copy-constructible types. 

Iterators of the container are stable as long as no move assignment happens, or as long as copy assignment happens with container of the same size.
