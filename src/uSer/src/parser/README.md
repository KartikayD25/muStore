## Protocol Buffer Parser

### Limitations

* Does not support parsing of enums.

* The closing brace of a message must be in a new line, different from any of the lines containing the field.
For example, the following is not supported:
  ```
  message Employee {
    string name = 1;
    int age = 2;
    double salary = 3 }
  ```

* The opening brace should be in the same line as the message name declaration. For example, the following is not supported:
  ```
  message Employee 
  {
    string name = 1;
    int age = 2;
    double salary = 3
  }
  ```
  
  or even this is not supported:
  ```
  message Employee 
  {  string name = 1;
    int age = 2;
    double salary = 3
  }
  ```

* Each distinct field should be defined in a new line.

* No error checking on Nested fields