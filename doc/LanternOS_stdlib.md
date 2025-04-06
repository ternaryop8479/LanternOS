# Document of LanternOS_stdlib

## Introduce
    LanternOS_stdlib is a library redevelop from stdlib, we organized some API from stdlib and use it to develop the Lantern OS.

## APIs
1. `printf(const char *format, ...)`(in `kernel/stdio.h`)
    * The function to output format string to stdio
    * format string:<br>
        Just like printf from stdlib, `%` stands for the beginning of the format parameters, but the formatting parameters are not exactly same as printf() from stdlib:<br>
        To explain out formatting parameters, first we need to understand our two types of formatting characters: ***type indentifider*** and ***radix indentifider***.<br>
        1. ***Type indentifider***:<br>
            * We use the type indentifider to appoint the variable type that we use, and there is three type indentifider of our printf(): `u`, `l` and `f`.
            * `u` means `unsigned`, add this parameter means target type is a `unsigned` type (so this parameter is not applicable to `float` or `double` type).
            * `l` means `long`, add this parameter means target type is a `(unsigned, depends on the `u` parameter) long` type or (unsigned depends on the `u` parameter too)`long long` type, and the number of times `l` appears in a character represents the number of times "long" appears in target type(e.g. `"ll"` means `long long`, `"ul"` means `unsigned long`)
            * `f` means `float` or `double`, it's easy to understand that add this parameter means target type is a `float` type or a (long, depends on if you add the `l` to the parameters, but only supports one `l` in format parameters because there is no `long long double` type in C)`double` type. (P.S. This parameter can not coexists `u` parameter)
            * And these parameters are not required to be arranged in the specified order, so you can combine the parameters freely(e.g. if you want to represent `unsigned long long`, you can use parameter `ull` or `llu`, even `lul`).
        2. ***Radix indentifider***:<br>
            * We use the radix indentifider to appoint the radix that we would like, there is three radix indentifider of our printf(): `b`, `d` and `x`.
            * `b` means binary, if this parameter is added, the output will be binary.
            * `d` means decimal, if this parameter is added, the output will be decimal.
            * `x` means hexadecimal, if this parameter is added, the output will be hexadecimal.
            * There can only be one radix indentifider in a formatting parameter, **and the indentifider must at the end of all of the parameters**, like `"%lld"`(stands for long long int, decimal mode), instead, if you use `"%ldl"`, the function will analyze the parameter as *a long type in decimal mode and a character 'l'*.

2. `memcpy(void *dest, const void *src, size_t n)`(in `kernel/string.h`)
    * Just like memcpy() in stdlib.