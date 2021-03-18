#### Notes on 4.1

* We remove the `VARIABLE_LIST` token, which occurs on e.g. `var a,b`, because
  the children of `DECLARATION` must be one or more `IDENTIFIER`'s.
  ```diff
    DECLARATION
  -  VARIABLE_LIST
      IDENTIFIER_DATA(a)
      IDENTIFIER_DATA(b)
  ```

* We remove the `VARIABLE_LIST` if parent is `PARAMETER_LIST`, since only
  `VARIABLE_LIST` (or nothing) is possible.
  ```diff
    PARAMETER_LIST
  -  VARIABLE_LIST
      IDENTIFIER_DATA(a)
      IDENTIFIER_DATA(b)
  ```

* `EXPRESSION` with a single child; fairly obvious (though multiple children
  should be kept, e.g. `gcd (a, b)` is an expression.)

* `GLOBAL` and `GLOBAL_LIST` are also removed because it's just a container for
  functions and declarations.

* `STATEMENT` is removed since it's a container type for more specific types of
  statement (e.g. `RETURN_STATEMENT`)

* `DECLARATION_LIST` can also be removed, and we can just set all
  `DECLARATION`s to be children of the parent of `DECLARATION_LIST`.

