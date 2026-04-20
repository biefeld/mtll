## Project Overview

This is a C implementation of a multi-typed linked list (MTLL) data structure, which supports nested lists.

It includes a CLI tool which can be used to investigate the functionality of MTLL's without the need for programming. This is a powerful tool for those interested in learning data structures/algorithms but are not confident writing programs in C. See [CLI Usage](#cli-usage) for permitted commands.

This implementation uses dynamic memory allocation, supporting large instances of MTLL's, nested lists, and the need for dynamic editing of MTLL's and nodes.

The MTLL data structure itself has been implemented as a linked list, instead of a dynamic array, for simplicity, cohesion and dynamic memory allocation.


## CLI Usage

To use the CLI, first build an executable by running `make` in a terminal, followed by `./mtll`.

Equivelant function calls (and additional utilities) can be found within [mtll.c](src/mtll.c) or [mtll.h](include/mtll.h) for those wanting to utilize the data structure in a program.

### NEW \<length\>

Create and populate a new MTLL data structure of a given length.

```
> NEW 4
> 1.0
>
> baguette
> 5
List 4: 1.00 -> -> baguette -> 5
```

Supports nesting lists too, referenced using `{id}`.

```
> NEW 3
> first
> {1}
> last
Nested 2: first -> {List 1} -> last
```

### VIEW \<id\>

Print data for a MTLL of a given id.

```
> VIEW 0
hello -> 1 -> 2 -> 3.14 -> a
```

### TYPE \<id\>

Print datatypes for a MTLL of a given id.

```
> TYPE 0
string -> int -> int -> float -> char
```

### VIEW ALL

Print current MTLL data structure labels.

```
> VIEW ALL
Number of lists: 3
List 0
List 3
List 4
```

### REMOVE \<id\>

Remove a MTLL for a given id. 

```
> REMOVE 3
List 3 has been removed.
Number of lists: 2
List 0
List 4
```

Removal of a MTLL while it is referenced by an other list is not permitted.

```
> VIEW 0
first -> {List 1} -> last
> REMOVE 1
INVALID COMMAND: REMOVE
```


### INSERT \<id\> \<index\> \<value\>

Insert a value at an index for a selected MTLL. Negative indicies are supported.

```
> VIEW 1
a -> b -> c -> d
> INSERT 1 0 Baguettes
List 1: Baguettes -> a -> b -> c -> d
> INSERT 1 -1 Bread
List 1: Baguettes -> a -> b -> c -> d -> Bread
```

Supports dynamic nesting.

```
> VIEW 0
a -> b -> c -> d
> NEW 2
> first
> last
List 1: first -> last
> INSERT 1 1 {0}
Nested 1: first -> {List 0} -> last
```

### DELETE \<id\> \<index\>

Delete an element at an index for a selected MTLL. Negative indicies are supported.

```
> VIEW 1
a -> b -> c -> d
> DELETE 1 0
List 1: b -> c -> d
> DELETE 1 -1
List 1: b -> c
```


### VIEW-NESTED \<id\>

Prints data for a MTLL of a given id, including data from nested lists.

```
> VIEW 1
a -> b -> c -> d
> VIEW 2
first -> {List 1} -> last
> VIEW-NESTED 1
a -> b -> c -> d
> VIEW-NESTED 2
first -> {a -> b -> c -> d} -> las
```

## Testing

A comprehensive testing suite has been included within the [test](/tests/) folder, composed of positive, negative and edge cases, along with a [driver script](test.sh).

To run these tests for yourself, use either `make test` or `./test.sh`.

For Linux users, running `make debug` runs the program with Valgrind to detect memory leaks (of which there are no known issues).
