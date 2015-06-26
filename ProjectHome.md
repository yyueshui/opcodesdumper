OpcodesDumper is a PHP extension provides opcodes dumper function base on embed sapi to PHP scrpts

# Install #
## Build PHP with Emebed SAPI ##
```
$ cd php-src/
$ ./configure --enable-embed
$ make && make install
```

## Compile ##
```
$./configure --with-php-config=/path/to/php-config
$make 
```


# Example #

**test.php**
```
<?php
echo "laruence";
```

**output**
```
$ ./opcodes_dumper /tmp/test.php 
Script: /tmp/test.php
opnum   line                         opcode                                      op1                                      op2                                   result
    0      2                      ZEND_ECHO                               "laruence"                                                                                  
    1      3                    ZEND_RETURN                                        1                                                                                  
    2      3          ZEND_HANDLE_EXCEPTION                                    

```