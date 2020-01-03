# bytecode-scanner
This program scans Java bytecode for instances of APIs. For example, specifying `java/io/PrintStream`, will scan the given classfile for calls to constructors and methods of that class, also providing the line number in the sourcefile of where this call takes place. An example application of this is to scan Java programs for malicious intent such as issuing file operations or opening network connections.

## Example
Given the sourcefile as Test.java and the compiled classfile Test.class:
```java
public class Test {
  public static void main(String[] args) {
    ArrayList<Integer> list = new ArrayList<>(Arrays.asList(1, 2, 3));
    System.out.println("Hello world!");
  }
}
```
Output:
```
> ./bytecode-scanner -s "java.io.PrintStream,java.util.ArrayList" Test.class
Found the following API calls in Test.class:
	java/io/PrintStream.println in method main on line 4
	java/util/ArrayList.<init> in method main on line 3
```

## Limitations
A few current limitations with this program are:
 * Storing Java strings in `cp_utf8_value` is (probably?) using the wrong string format.
 * Skips annotation information.
 * Not very complete API-wise.
 * It may be possible to get around this using Reflection.

## License
MIT

## Author
Anthony Calandra
