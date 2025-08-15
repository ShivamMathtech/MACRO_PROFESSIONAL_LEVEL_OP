## BY SHIVAM SINGH
````markdown
# MACRO in C – Deep Dive 🚀

## Overview
In C programming, **macros** are compile-time directives handled by the **preprocessor** before the actual compilation begins.  
They allow programmers to define constants, inline code fragments, and conditional compilation logic, improving **code efficiency** and **readability**.

Macros are not functions—they do **text substitution** before compilation.

---

## Table of Contents
1. [Macro Basics](#macro-basics)
2. [Types of Macros](#types-of-macros)
3. [Parameterized Macros](#parameterized-macros)
4. [Multiple Statement Macros](#multiple-statement-macros)
5. [Built-in Predefined Macros](#built-in-predefined-macros)
6. [Function Tracing with Macros](#function-tracing-with-macros)
7. [NASA-level Usage Guidelines](#nasa-level-usage-guidelines)
8. [Pros and Cons](#pros-and-cons)
9. [Best Practices](#best-practices)
10. [References](#references)

---

## Macro Basics
- Defined using `#define`
- Preprocessor replaces macro calls **before** compilation.
- Syntax:
```c
#define NAME replacement_text
````

Example:

```c
#define PI 3.141592653589793
#define AREA(r) (PI * (r) * (r))
```

---

## Types of Macros

1. **Object-like Macros**

   ```c
   #define VERSION 1.0
   ```

2. **Function-like Macros**

   ```c
   #define SQUARE(x) ((x) * (x))
   ```

3. **Parameterized Macros**

   * Accept arguments like functions but **no type checking**.

   ```c
   #define MAX(a,b) ((a) > (b) ? (a) : (b))
   ```

4. **Chained Macros**

   ```c
   #define A 10
   #define B A
   ```

5. **Multiple Statement Macros**

   * Use `do { ... } while(0)` to make them safe.

   ```c
   #define SWAP(a,b) do { int temp = a; a = b; b = temp; } while(0)
   ```

---

## Parameterized Macros

Example:

```c
#include <stdio.h>
#define MUL(a,b) ((a) * (b))

int main() {
    printf("%d\n", MUL(4, 5)); // Output: 20
}
```

**Advantages:**

* No function call overhead.
* Useful for small, frequently called operations.

---

## Multiple Statement Macros

**Problem:** Without a safety wrapper, `if/else` logic may break.
**Solution:** Wrap in `do { ... } while(0)`.

```c
#include <stdio.h>
#define PRINT_VALUES(a,b) do { \
    printf("A: %d\n", a); \
    printf("B: %d\n", b); \
} while(0)

int main() {
    PRINT_VALUES(5, 10);
}
```

---

## Built-in Predefined Macros

C provides several macros automatically:

| Macro      | Description                                      |
| ---------- | ------------------------------------------------ |
| `__DATE__` | Current compilation date (e.g., `"Aug 15 2025"`) |
| `__TIME__` | Current compilation time (e.g., `"10:45:30"`)    |
| `__FILE__` | Current filename                                 |
| `__LINE__` | Current line number                              |
| `__func__` | Current function name                            |
| `__STDC__` | Indicates if standard C is supported             |

Example:

```c
#include <stdio.h>
int main() {
    printf("Compiled on %s at %s\n", __DATE__, __TIME__);
    printf("File: %s, Line: %d\n", __FILE__, __LINE__);
}
```

---

## Function Tracing with Macros

Macros can auto-log function entry/exit points.

```c
#include <stdio.h>
#define TRACE_FUNC() \
    printf("[TRACE] Entering %s() at %s:%d\n", __func__, __FILE__, __LINE__)

void sampleFunction() {
    TRACE_FUNC();
    // Function logic
}

int main() {
    sampleFunction();
}
```

---

## NASA-level Usage Guidelines

In **safety-critical systems** like spacecraft software:

1. **Use macros only when absolutely necessary** — prefer `inline` functions for type safety.
2. **Document every macro** with purpose, usage constraints, and test cases.
3. **Never use macros for constants** unless they must be preprocessor-resolved.
4. **Avoid hidden side effects** — arguments in macros should never be evaluated more than once.
5. **Use uppercase names** for macros to distinguish from variables/functions.
6. **Review macros for portability** — some compilers may treat them differently.
7. **Enforce do-while(0) idiom** for multi-statement macros to avoid syntactic errors.
8. **Trace and log with macros** in debugging builds, remove in production.

---

## Pros and Cons

**Pros:**

* No runtime overhead
* Flexible and powerful
* Can integrate with compiler-specific features

**Cons:**

* No type checking
* Can cause unexpected behavior due to multiple evaluations
* Harder to debug than functions

---

## Best Practices

* Use macros only for:

  * Conditional compilation
  * Compile-time constants
  * Debug logging and tracing
* Prefer `const`, `enum`, or `inline` for everything else.
* Wrap complex macros in parentheses.
* Avoid naming conflicts by using a prefix.

---

## References

* ISO/IEC 9899:2018 (C17 Standard)
* NASA C Programming Guidelines
* GCC Preprocessor Manual
