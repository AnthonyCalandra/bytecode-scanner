# bytecode-scanner
This program scans Java bytecode for instances of APIs. For example, specifying `java/io/PrintStream`, will scan the given classfile for calls to constructors and methods of that class, also providing the line number in the sourcefile of where this call takes place. An example application of this is to scan Java programs for malicious intent such as issuing file operations or opening network connections.

## Example
Given the sourcefile as Test.java and the compiled classfile Test.class:
```java
import java.util.*;

public class Test {
  public static void main(String[] args) {
    ArrayList<Integer> list = new ArrayList<>(Arrays.asList(1, 2, 3));
    System.out.println("Hello world!");
  }
}
```

Find instances of APIs using the `-s` (scan) flag, which is a comma-separated list of packages:
```
> ./bytecode-scanner -s "java.io.PrintStream,java.util.ArrayList" Test.class
Found the following API calls in Test.class:
	java/io/PrintStream.println in method main on line 4
	java/util/ArrayList.<init> in method main on line 3
```

Get a full dump of the constant pool using `-c` (constant-pool):
```
> ./bytecode-scanner -c Test.class
#1 = MethodRef    #11:#20                               -> java/lang/Object."<init>":()V
#2 = Class        #21                                   -> java/util/ArrayList
#3 = Class        #22                                   -> java/lang/Integer
#4 = MethodRef    #3:#23                                -> java/lang/Integer.valueOf:(I)Ljava/lang/Integer;
#5 = MethodRef    #24:#25                               -> java/util/Arrays.asList:([Ljava/lang/Object;)Ljava/util/List;
#6 = MethodRef    #2:#26                                -> java/util/ArrayList."<init>":(Ljava/util/Collection;)V
#7 = FieldRef     #27:#28                               -> java/lang/System.out:Ljava/io/PrintStream;
#8 = String       #29                                   -> 哈 哈!
#9 = MethodRef    #30:#31                               -> java/io/PrintStream.println:(Ljava/lang/String;)V
#10 = Class       #32                                   -> ReadmeTest
#11 = Class       #33                                   -> java/lang/Object
#12 = Utf8        <init>
#13 = Utf8        ()V
#14 = Utf8        Code
#15 = Utf8        LineNumberTable
#16 = Utf8        main
#17 = Utf8        ([Ljava/lang/String;)V
#18 = Utf8        SourceFile
#19 = Utf8        ReadmeTest.java
#20 = NameAndType #12:#13                               -> "<init>":()V
#21 = Utf8        java/util/ArrayList
#22 = Utf8        java/lang/Integer
#23 = NameAndType #34:#35                               -> valueOf:(I)Ljava/lang/Integer;
#24 = Class       #36                                   -> java/util/Arrays
#25 = NameAndType #37:#38                               -> asList:([Ljava/lang/Object;)Ljava/util/List;
#26 = NameAndType #12:#39                               -> "<init>":(Ljava/util/Collection;)V
#27 = Class       #40                                   -> java/lang/System
#28 = NameAndType #41:#42                               -> out:Ljava/io/PrintStream;
#29 = Utf8        哈 哈!
#30 = Class       #43                                   -> java/io/PrintStream
#31 = NameAndType #44:#45                               -> println:(Ljava/lang/String;)V
#32 = Utf8        ReadmeTest
#33 = Utf8        java/lang/Object
#34 = Utf8        valueOf
#35 = Utf8        (I)Ljava/lang/Integer;
#36 = Utf8        java/util/Arrays
#37 = Utf8        asList
#38 = Utf8        ([Ljava/lang/Object;)Ljava/util/List;
#39 = Utf8        (Ljava/util/Collection;)V
#40 = Utf8        java/lang/System
#41 = Utf8        out
#42 = Utf8        Ljava/io/PrintStream;
#43 = Utf8        java/io/PrintStream
#44 = Utf8        println
#45 = Utf8        (Ljava/lang/String;)V
```

## Limitations
A few current limitations with this program are:
 * Skips annotation information.
 * Not very complete API-wise.
 * It may be possible to get around this using Reflection.

## License
MIT

## Author
Anthony Calandra
