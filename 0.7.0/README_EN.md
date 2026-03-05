# 📚 ZzBasic v0.7.0 

**Version:** 0.7.0  
**Status:** Stable  
**Date:** March 04, 2026

---

## Table of Contents

1. [Data Types](#data-types)
2. [Arithmetic Operations](#arithmetic-operations)
3. [Variables and Assignment](#variables-and-assignment)
4. [Print Command](#print-command)
5. [Input Command](#input-command)
6. [Control Structures](#control-structures)
7. [Comparison Operators](#comparison-operators)
8. [Logical Operators](#logical-operators)
9. [Arrays](#arrays)
10. [Built-in Functions](#built-in-functions)
11. [Modules](#modules)
12. [Text Type](#text-type)
13. [Scopes and Local Variables](#scopes-and-local-variables)
14. [Resource Summary](#resource-summary)
15. [Known Limitations](#known-limitations)

---

## Data Types

- Number
- String
- Boolean
- Text

### NUMBER (Numeric)

**Description:** Represents floating-point numeric values (double).

**Characteristics:**
- Supports integers and decimals
- Range: According to IEEE 754 specification (double)
- Examples: `3.14`, `-42`, `0`
- Does not support scientific notation (`1.5e-10`)

**Example:**
```basic
let pi = 3.14159
let age = 25
let balance = -100.50
```

**LIMITATIONS**
- The number must have a maximum of 24 characters.

Code:
```
let n = 123456789012345678901234
print "n : " n nl
```

Output:
```
n :  1.23456789
```

But, if we try to declare a number with more than 24 characters:
```
let n = 1234567890123456789012345
print "n : " n nl
```

Output:
```
[9:9] Lexer error: buffer overflow while reading integer part
  Line 9: let n = 1234567890123456789012345
               ↑
```

**Real Limits:**
- **Largest positive number**: `1.7976931348623e+308` (≈1.8×10³⁰⁸)
- **Smallest positive number**: `2.2250738585072e-308` (≈2.2×10⁻³⁰⁸)
- **Largest negative number**: `-1.7976931348623e+308`
- **Smallest negative number**: `-2.2250738585072e-308`

### STRING (Text)

**Description:** Represents sequences of characters (text).

**Characteristics:**
- Delimited by double quotes (`"`)
- Maximum size: 128 characters
- Supports spaces and special characters

**Example:**
```basic
let name = "Zurg Evil Emperor"
let message = "Hello, world!"
let empty = ""
```

### BOOL (Boolean)

**Description:** Represents logical values true or false.

**Characteristics:**
- Values: `true` or `false`
- Results of comparison operations
- Results of logical operations

**Example:**
```basic
let active = true
let disabled = false
let result = 5 > 3  # result = true
```

### TEXT 

**Description:** Custom type for text manipulation with advanced operations.

**Characteristics:**
- Dynamic allocation on heap
- Automatic growth (factor 1.5x)
- File read/write operations
- **NEW v0.7.0:** Automatic deallocation when exiting scope
- **NEW v0.7.0:** Allocation tracking with Object Tracker

**Example:**
```basic
let content = load("file.txt")
save content "output.txt"

let text = text("Hello, world!")
print text nl
```

---

## Arithmetic Operations

### Basic Operators

| Operator | Description | Example | Result |
|----------|-------------|---------|--------|
| `+` | Addition | `5 + 3` | `8` |
| `-` | Subtraction | `10 - 4` | `6` |
| `*` | Multiplication | `6 * 7` | `42` |
| `/` | Division | `20 / 4` | `5` |

### Operator Precedence

**Order of execution (from highest to lowest precedence):**

1. Parentheses `( )`
2. Unary `+`, `-`
3. Multiplication `*`, Division `/`
4. Addition `+`, Subtraction `-`

**Examples:**
```basic
let result = 2 + 3 * 4        # = 14 (not 20)
let result = (2 + 3) * 4      # = 20
let result = 10 - 5 - 2       # = 3 (left to right)
let result = -5 + 3           # = -2
```

### Unary Operator

**Description:** Operator applied to a single operand.

| Operator | Description | Example | Result |
|----------|-------------|---------|--------|
| `+` | Positive | `+5` | `5` |
| `-` | Negative | `-5` | `-5` |

**Example:**
```basic
let number = 10
let negative = -number  # = -10
let positive = +number  # = 10
```

### Division by Zero

**Behavior:** Returns error and interrupts execution.

```basic
let result = 10 / 0  # ❌ Error: division by zero
```

---

## Variables and Assignment

### LET Command

**Description:** Creates and assigns a value to a variable.

**Syntax:**
```
let <identifier> = <expression>
```

**Characteristics:**
- Identifier: Starts with letter or `_`, followed by letters, numbers or `_`
- Maximum identifier size: 32 characters
- Dynamic typing (type determined by assigned value)
- Once a type is assigned, the variable will be of that type and cannot receive values of another type
- **NEW v0.7.0:** Local scope within blocks (IF, WHILE, FOR)

**Examples:**
```basic
let name = "Alice"
print name nl

let active = true
print active nl

let _private = 42
print _private nl

let result = 5 + 3 * 2 # 11
print result nl

let y = 20
print y nl

let y = 30.75
print y nl

let y = "asd" # ERROR - TRIED TO CHANGE TYPE OF y
print y nl
```

Output:

```
Alice
true
42
11
20
30.75
[35:9] Evaluator error: assigning string to 'y'
  Line 35: let y = "asd" # TRIED TO CHANGE TYPE OF y
                    ↑
```

### Variable Types

**Number Variables:**
```basic
let age = 25
let height = 1.75
let balance = -500.00
```

**String Variables:**
```basic
let name = "John"
let address = "Flower Street, 123"
```

**Boolean Variables:**
```basic
let active = true
let connected = false
```

**Array Variables:**
```basic
let list = array(0)
let numbers = array(5)
```

**Text Variables:**
```basic
let text = text("Content")
let file = load("data.txt")
```

---

## Print Command

### Basic Syntax

**Description:** Displays values on screen.

**Syntax:**
```
print <expression1> [<expression2> ...] [nl]
```

**Characteristics:**
- Accepts multiple expressions separated by space
- Supports colors and formatting
- `nl` at end: line break
- `print` alone on line skips line; same as `print nl`

**Examples:**
```basic
print "Hello, world!"
print 42
print "The result is: " 5 + 3
print "Line 1" nl 
```

### Shortcut with ?

**Description:** Abbreviated form of `print`.

**Example:**
```basic
? "Test"  # Equivalent to: print "Test"
```

### Print Formatting

#### WIDTH Command

**Description:** Sets the field width for display.

**Syntax:**
```
print width(<number>) <expression>
```

**Characteristics:**
- Value: Field width in characters
- If the value to be displayed has more characters than the field width, the value will be displayed and the field width will not be respected.

**Example:**
```basic
print width(10) "Test"    # "Test     "
print width(5) 42         # "42   "
```

#### ALIGNMENT Command

**Description:** Sets text alignment.

**Options:**
- `left` - Left aligned (default)
- `right` - Right aligned
- `center` - Centered

**Syntax:**
```
print <alignment> width(<number>) <expression>
```

**Example:**
```basic
print width(10) left "Test"    # "Test     "
print width(10) right "Test"   # "     Test"
print width(10) center "Test"  # "  Test   "
```

### Colors in Print

**Description:** Adds colors to displayed text.

**Colors:**
- `black` - Black
- `red` - Red
- `green` - Green
- `yellow` - Yellow
- `blue` - Blue
- `magenta` - Magenta
- `cyan` - Cyan
- `white` - White

**Bright Colors:**
- `bred` - Bright red
- `bgreen` - Bright green
- `byellow` - Bright yellow
- `bblue` - Bright blue
- `bmagenta` - Bright magenta
- `bcyan` - Bright cyan
- `bwhite` - Bright white

**Background Colors**
- `bgblack` - Black background
- `bgred` - Red background
- `bggreen` - Green background
- `bgyellow` - Yellow background
- `bgblue` - Blue background
- `bgmagenta` - Magenta background
- `bgcyan` - Cyan background
- `bgwhite` - White background

**NOCOLOR Command:**

**Description:** Disables colors for the rest of the line.

**Syntax:**
```
print <color> <expression> nocolor [<expression>]
```

**Example:**
```basic
print red "ERROR:" nocolor " File not found"
print green "Success"
print blue width(20) center "TITLE"
```

### Line Break

**Command `nl`:**

**Description:** Inserts line break.

**Syntax:**
```
print <expression> nl
```

**Example:**
```basic
print "Line 1" nl
print "Line 2" nl
print "Line 3"
```

---

## Input Command

### Basic Syntax

**Description:** Reads user input from keyboard.

**Syntax:**
```
input [<formatting>] "<prompt>" <variable>
```

**Characteristics:**
- Prompt: Message displayed before reading input (optional)
- Prompt accepts colors and formatting, like `print`
- Variable: Name of variable to store value (required)
- Automatically detects type

**Examples:**
```basic
# Simple input
input "Enter your name: " name
input "Enter your age: " age

# Input with colors and formatting
input green width(50) "User: " user
input red "ERROR - Try again: " nocolor value
```

---

## Control Structures

### IF-THEN-ELSE Command

**Description:** Executes code conditionally.

**Syntax:**
```
if <condition> then
    <body_then>
else
    <body_else>
end if
```

**Characteristics:**
- `else` is optional
- Condition: Boolean expression; MUST BE IN PARENTHESES
- Body: One or more statements
- `end if` is required
- **NEW v0.7.0:** Supports local scopes

**Examples:**
```basic
# simple if
if (n >= 0) then
  print "Positive number" nl
end if

# if...else
if (age >= 18) then
    print green "You are of age" nocolor nl
else
    print red "You are underage" nocolor nl
end if

# if...else if...else
if (grade >= 9) then
    print "Grade: A (Excellent)" nl
else if (grade >= 7) then
    print "Grade: B (Good)" nl
else if (grade >= 5) then
    print "Grade: C (Satisfactory)" nl
else
    print "Grade: D (Insufficient)" nl
end if
```

### WHILE Command

**Description:** Executes code repeatedly while a condition is true.

**Syntax:**
```
while <condition> do
    <body>
end while
```

**Characteristics:**
- Checks condition before each iteration
- Condition: Boolean expression; MUST BE IN PARENTHESES
- `do` is required
- `end while` is required
- Supports `break` and `continue`
- **NEW v0.7.0:** Supports local scopes

**Examples:**
```basic
# Simple counting
let i = 1
while (i <= 5) do
    print i nl
    let i = i + 1
end while

# Loop with break
let counter = 0
while (true) do
    let counter = counter + 1
    if (counter > 10) then
        break
    end if
    print counter nl
end while
```

### FOR Command

**Description:** Executes code a determined number of times.

**Syntax:**
```
for <variable> = <start> to <end> [step <step>] do
    <body>
end for
```

**Characteristics:**
- Variable: Loop counter
- Start: Initial value (inclusive)
- End: Final value (inclusive)
- Step: Increment per iteration (optional; default: 1)
- `end for` is required
- Supports `break` and `continue`
- **NEW v0.7.0:** Supports local scopes

**Examples:**
```basic
# Simple FOR
for i = 1 to 5 do
    print i nl
end for

# FOR with step
for i = 0 to 10 step 2 do
    print i ", "   # 0, 2, 4, 6, 8, 10, 
end for
print 

# Decreasing FOR
for i = 10 to 1 step -1 do
    print i ", "  # 10 , 9 , 8 , 7 , 6 , 5 , 4 , 3 , 2 , 1 , 
end for
print

# FOR with break
for i = 1 to 100 do
    print i ", " # 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10 , 
    if (i == 10) then
        break
    end if
end for
print

# FOR with continue
for i = 1 to 10 do
    if (i == 5) then
        continue
    end if
    print i ", " # 1, 2, 3, 4, 6, 7, 8, 9, 10
end for
print
```

### BREAK Command

**Description:** Exits the loop (while or for) immediately.

**Syntax:**
```
break
```

### CONTINUE Command

**Description:** Skips to the next iteration of the loop.

**Syntax:**
```
continue
```

---

## Comparison Operators

**Description:** Compare two values and return boolean.

| Operator | Description | Example | Result |
|----------|-------------|---------|--------|
| `==` | Equal | `5 == 5` | `true` |
| `!=` | Not equal | `5 != 3` | `true` |
| `<` | Less than | `3 < 5` | `true` |
| `>` | Greater than | `5 > 3` | `true` |
| `<=` | Less than or equal | `5 <= 5` | `true` |
| `>=` | Greater than or equal | `5 >= 3` | `true` |

---

## Logical Operators

### AND Operator

**Description:** Returns `true` if both conditions are true.

**Syntax:**
```
<condition1> and <condition2>
```

**Truth Table:**

| A | B | A AND B |
|---|---|---------|
| T | T | T |
| T | F | F |
| F | T | F |
| F | F | F |

**Example:**
```basic
let age = 25
let income = 3000

if (age >= 18 and income >= 2000) then
    print "Loan approved" nl
end if
```

### OR Operator

**Description:** Returns `true` if at least one condition is true.

**Syntax:**
```
<condition1> or <condition2>
```

**Truth Table:**

| A | B | A OR B |
|---|---|--------|
| T | T | T |
| T | F | T |
| F | T | T |
| F | F | F |

**Example:**
```basic
let sunday = 1
let saturday = 7
let day = saturday
let holiday = true

if (day == saturday or day == sunday or holiday) then
    print "It's a day off" nl
end if
```

### NOT Operator

**Description:** Inverts the boolean value.

**Syntax:**
```
not <condition>
```

or

```
! <condition>
```

**Truth Table:**

| A | NOT A |
|---|-------|
| T | F |
| F | T |

**Example:**
```basic
let active = false

if (not active) then
    print "System disabled" nl
end if

# Equivalent with !
if (! active) then
    print "System disabled" nl
end if
```

### Complex Combinations

**Example:**
```basic
let age = 25
let income = 3000
let employed = true

if ( (age >= 18 and income >= 2000) or employed ) then
    print "Eligible for loan" nl
end if

if (not (age < 18 or income < 1000) ) then
    print "Criteria met" nl
end if
```

---

## Arrays

### Array Creation

**Syntax:**
```
let <variable> = array(<number of elements>)
```

**Characteristics:**
- Arrays are dynamic
- In v0.7.0 can only contain numbers
- Index starts at 0
- Support built-in functions

**Examples:**
```basic
let numbers = array(5)

let numbers[0] = 3
let numbers[1] = 4
let numbers[2] = 5

print numbers[1] nl # 4

print numbers nl # [3, 4, 5]
```

### Element Access

**Syntax:**
```
<variable>[<index>]
```

**Example:**
```basic
print numbers[0]  # 3
print numbers[1]  # 4
print numbers[2]  # 5
```

### Built-in Functions

#### push()

**Description:** Adds an element to the end of the array.

**Syntax:**
```
push(<array>, <value>)
```

**Example:**
```basic
push(numbers, 13)
print numbers nl # [3, 4, 5, 13]
```

#### pop()

**Description:** Removes and returns the last element of the array.

**Syntax:**
```
pop(<array>)
```

**Example:**
```basic
let last = pop(numbers)
print last nl # 13
```

#### len()

**Description:** Returns the number of elements in the array.

**Syntax:**
```
len(<array>)
```

**Example:**
```basic
print len(numbers) nl # 3
```

#### get()

**Description:** Gets an element at a specific index.

**Syntax:**
```
get(<array>, <index>)
```

**Example:**
```basic
let m = get(numbers, 1)
print m nl # 4 
```

#### set()

**Description:** Sets an element at a specific index.

**Syntax:**
```
set(<array>, <index>, <value>)
```

**Example:**
```basic
set(numbers, 1, 13)
print numbers nl # [3, 13, 5]
```

#### insert()

**Description:** Inserts an element at a specific index.

**Syntax:**
```
insert(<array>, <index>, <value>)
```

**Example:**
```basic
insert(numbers, 1, 89)
print numbers nl # [3, 89, 13, 5]
```

#### remove()

**Description:** Removes an element at a specific index.

**Syntax:**
```
remove(<array>, <index>)
```

**Example:**
```basic
remove(numbers, 2)
print numbers nl # [3, 89, 5]
```

#### is_empty()

**Description:** Checks if the array is empty.

**Syntax:**
```
is_empty(<array>)
```

**Example:**
```basic
print is_empty(numbers) nl # false
```

### Array Functions Summary

- `push()` - Adds element
- `pop()` - Removes last element
- `len()` - Returns size
- `get()` - Gets element
- `set()` - Sets element
- `insert()` - Inserts element
- `remove()` - Removes element
- `is_empty()` - Checks if array is empty

---

## Modules 

### IMPORT Command

**Description:** Imports functions from an external module.

**Syntax:**
```
import <module_name> 
from <module_name> import <function1>, <function2>, ... 
```

**Example:**
```basic
import math 

from math import sqrt, pow
```

NOTE: In v0.7.0 only displays message

```
Module 'math' loaded successfully
Module 'math' loaded with selected functions.TODO: Import specific functions
```

--- 

### LOAD Command

**Description:** Loads file content as Text type.

**Syntax:**
```
let <variable> = load("<path>")
```

**Characteristics:**
- Returns Text type
- Loads entire file into memory
- Error if file does not exist
- File path must be in double quotes

**Example:**
```basic
let text = load("tests/error.zz")
print text nl
```

Output (content of file "tests/error.zz"):

```
let x = 10 / 0
print "-5 + 10 = " -5 + 10 nl
``` 

---

### SAVE Command

**Description:** Saves content to a file.

**Syntax:**
```
save <expression> "<path>"
```

**Characteristics:**
- Saves Text type to file
- Overwrites existing file
- Error if unable to write

**Example:**
```basic
let data = "Important content"
save data "backup.txt"
```

---

## Text Type

### Description

TEXT is a custom type for text manipulation with support for file read/write operations. Unlike STRING (which has fixed size), TEXT is dynamic and grows as needed.

### Characteristics

**1. Dynamic Allocation on Heap**

The TEXT type is stored on the **heap** (dynamic memory), not on the stack.

**2. Automatic Growth**

TEXT grows automatically as needed using a growth factor of 1.5x. This allows storing files of any size without predefined limits.

**3. Safe File Reading**

When loading a file with `load()`, TEXT:
- Reads file in 4096 byte chunks
- Expands capacity as needed
- Works with files of any size
- Preserves binary content

**4. Automatic Memory Deallocation (NEW v0.7.0)**

**IMPORTANT:** In v0.7.0, memory deallocation is **AUTOMATIC**. When TEXT exits scope, memory is automatically freed. This was implemented with the local scopes system.

**5. Allocation Tracking (NEW v0.7.0)**

The **Object Tracker** module allows tracking all Text allocations during development:
- View which Text objects are allocated
- Check when they are freed
- Detect memory leaks
- Debug scope issues

**6. Functions for Text manipulation**
- `load(<pathfile>)` - Loads file as Text
- `save(Text, pathfile)` - Saves Text to file
- `text(<string>)` - Creates new Text from string

NOTE: Observe that the first parameter of `save()` function is a Text object, not a string.

### Usage Example

```basic
let text = load("tests/error.zz")
print text nl
print

save(text("testing 1, 2, 3"), "testing.txt")
let testing = load("testing.txt")
print testing nl
print

# NEW v0.7.0: Local scopes
if (true) then
    let local_text = text("Local text")
    print local_text nl
    # local_text is automatically freed here
end if
```

---

## Scopes and Local Variables

### NEW v0.7.0: Local Scope Support

**Description:** Variables created within blocks (IF, WHILE, FOR) now have local scope.

**Characteristics:**
- Global variables: Created outside any block
- Local variables: Created within blocks
- Automatic deallocation: When exiting the block
- Support for nested scopes

**Example:**
```basic
let global = "Global"
print global nl

if (true) then
    let local = "Local"
    print local nl
    # local is freed here
end if

# Error: local no longer exists
# print local nl  # ❌ Error: unknown 'local' variable

print global nl  # ✅ Works
```

### Nested Scopes

```basic
let x = "global"

if (true) then
    let x = "if local"
    print x nl
    
    while (true) do
        let x = "while local"
        print x nl
        break
    end while
    
    print x nl  # Back to "if local"
end if

print x nl  # Back to "global"
```

### Text Deallocation When Exiting Scope

```basic
print "Before:" nl
let global_text = text("Global")

if (true) then
    let local_text = text("Local")
    print local_text nl
    # local_text is freed here
end if

print global_text nl
# global_text will be freed at end of program
```

---

## Resource Summary

| Resource | Status | Version |
|----------|--------|---------|
| Arithmetic Operations | ✅ Complete | 0.1.0 |
| Variables (LET) | ✅ Complete | 0.1.0 |
| Print with Colors | ✅ Complete | 0.3.0 |
| Input | ✅ Complete | 0.4.0 |
| IF-THEN-ELSE | ✅ Complete | 0.2.0 |
| WHILE | ✅ Complete | 0.4.0 |
| FOR | ✅ Complete | 0.4.0 |
| BREAK/CONTINUE | ✅ Complete | 0.4.0 |
| Comparison Operators | ✅ Complete | 0.2.0 |
| Logical Operators | ✅ Complete | 0.3.0 |
| Arrays | ✅ Complete | 0.5.0 |
| Built-in Functions | ✅ Complete | 0.5.0 |
| Module Import | ✅ Started | 0.5.0 |
| Load/Save | ✅ Complete | 0.6.0 |
| Text Type | ✅ Complete | 0.7.0 |
| **Local Scopes** | **✅ New** | **0.7.0** |
| **Object Tracker** | **✅ New** | **0.7.0** |
| **Automatic Deallocation** | **✅ New** | **0.7.0** |
| Custom Functions | ⏳ Future | - |
| Classes/Objects | ⏳ Future | - |

---

## Known Limitations

1. Input prompt: maximum 256 characters
2. Program line: maximum 128 characters
3. String type: maximum 128 characters
4. Variable name: maximum 32 characters
5. Number: maximum 24 characters
6. Number of program lines in REPL mode: maximum 50 lines
7. Object Tracker: maximum 1000 tracked objects simultaneously
8. Object Tracker info field: maximum 128 characters

---

## v0.7.0 Highlights

### ✨ Main Changes

1. **Local Scopes**
   - Variables created in blocks (IF, WHILE, FOR) now have local scope
   - Automatic deallocation when exiting block
   - Support for nested scopes

2. **Automatic Text Deallocation**
   - Text is now automatically freed when exiting scope
   - No memory leaks
   - Safe to use in loops

3. **Object Tracker Module**
   - Generic module for tracking allocations
   - View which objects are allocated
   - Detect memory leaks
   - Reusable for new data types

4. **Stability Improvements**
   - Critical bug fix in `execute_stmt_list_with_ctx()`
   - Better error handling
   - More robust code

---

ZzBasic v0.7.0

arataca89@gmail.com

