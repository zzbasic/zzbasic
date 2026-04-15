# ZzBasic Examples v0.8.0

## 1. hello_world.zz - Your First Program

The classic "Hello, World!":

```basic
print "Hello, World!" nl
```

---

## 2. variables.zz - Working with Variables

Learn how to create and use variables:

```basic
let name = "Alice"
let age = 25
let height = 1.65

print "Name: " name nl
print "Age: " age nl
print "Height: " height nl
```

---

## 3. arithmetic_operations.zz - Mathematical Calculations

Basic operations with numbers including the modulo operator:

```basic
let a = 10
let b = 3

print "Sum: " a + b nl
print "Subtraction: " a - b nl
print "Multiplication: " a * b nl
print "Division: " a / b nl
print "Modulo (remainder): " a % b nl
```

---

## 4. comparisons.zz - Comparing Values

Understand comparison operators (now also with strings):

```basic
let x = 10
let y = 20
let name1 = "Alice"
let name2 = "Bob"

print "x == y: " x == y nl
print "x != y: " x != y nl
print "x < y: " x < y nl
print "x > y: " x > y nl
print "x <= y: " x <= y nl
print "x >= y: " x >= y nl
print nl
print "name1 == name2: " name1 == name2 nl
print "name1 < name2: " name1 < name2 nl
```

---

## 5. if_else.zz - Conditional Control

Make decisions in your program:

```basic
let age = 20

if (age >= 18) then
    print "You are of legal age" nl
else
    print "You are underage" nl
end if
```

---

## 6. loop_while.zz - While Loop

Repeat actions while a condition is true:

```basic
let i = 1
while (i <= 5) do
    print "Iteration " i nl
    let i = i + 1
end while
```

---

## 7. loop_for.zz - For Loop - when you know the number of repetitions

Repeat a specific number of times:

```basic
for i = 1 to 5 do
    print "Number: " i nl
end for
```

---

## 8. do_end.zz - DO...END and DO...END WHILE Blocks

Create isolated scope blocks or loops that execute at least once:

```basic
# DO...END - isolated scope
let x = 10
do
    let y = 20
    print "Inside block: x=" x ", y=" y nl
end do
print "Outside block: x=" x nl
# print y nl  # ERROR! y doesn't exist outside the block

print nl

# DO...END WHILE - loop that executes at least once
let i = 0
do
    print "Iteration: " i nl
    let i = i + 1
end while (i < 5)
```

---

## 9. arrays_basic.zz - Introduction to Arrays

Work with data collections (now also with strings):

```basic
let numbers = array(5)
push(numbers, 10)
push(numbers, 20)
push(numbers, 30)

print "First: " numbers[0] nl
print "Second: " numbers[1] nl
print "Third: " numbers[2] nl
print "Size: " len(numbers) nl

print nl

# Arrays can also contain strings
let names = array(5)
push(names, "Alice")
push(names, "Bob")
push(names, "Charlie")
print "Names: " names nl
```

---

## 10. input_output.zz - User Interaction

Read data from the user:

```basic
input "Enter your name: " name
input "Enter your age: " age

print "Hello, " name "!" nl
print "You are " age " years old" nl
```

---

## 11. colors_formatting.zz - Colors and Formatting

Use colors and formatting in output:

```basic
print red "Red text" nl
print green "Green text" nl
print blue "Blue text" nl
nocolor # reset color
print nl

print width(30) right "Aligned text" nl
print width(30) center "Centered text" nl
print nl

print red "Red" nocolor " normal" nl
```

---

## 12. simple_functions.zz - Creating Functions

Learn how to create and use functions:

```basic
# Function with two parameters
function sum(a, b)
    return a + b
end function

# Function without parameters
function greeting()
    return "Hello, world!"
end function

# Function with conditional
function max(a, b)
    if (a > b) then
        return a
    else
        return b
    end if
end function

# Using the functions
let result = sum(3, 5)
print "Sum: " result nl

let msg = greeting()
print msg nl

let m = max(15, 8)
print "Max: " m nl
```

---

## 13. recursive_functions.zz - Recursive Functions

Functions that call themselves:

```basic
# Recursive factorial
function factorial(n)
    if (n <= 1) then
        return 1
    end if
    return n * factorial(n - 1)
end function

# Recursive fibonacci
function fibonacci(n)
    if (n <= 1) then
        return n
    end if
    return fibonacci(n - 1) + fibonacci(n - 2)
end function

let fact5 = factorial(5)
print "Factorial of 5: " fact5 nl

let fib10 = fibonacci(10)
print "Fibonacci(10): " fib10 nl
```

---

## 14. string_functions.zz - String Functions

Manipulate strings with the new built-in functions:

```basic
let text = "  Hello World!  "

print "Original: [" text "]" nl
print "Trim: [" trim(text) "]" nl
print "Upper: " upper(text) nl
print "Lower: " lower(text) nl
print "Substr(0,5): " substr(text, 0, 5) nl

print nl

# Split and Join
let data = "apple,banana,orange"
let fruits = split(data, ",")
print "Split: " fruits nl

let result = join(fruits, " - ")
print "Join: " result nl
```

---

## 15. bmi_calculator.zz - BMI Calculator

Calculate Body Mass Index:

```basic
# Data input
input "Enter your weight (kg) : " weight
input "Enter your height (m): " height

# Calculation
let bmi = weight / (height * height)

# Result
print "Your BMI is: " bmi nl

if (bmi < 18.5) then
    print "Underweight" nl
else
    if (bmi < 25) then
        print "Normal weight" nl
    else
        if (bmi < 30) then
            print "Overweight" nl
        else
            print "Obesity" nl
        end if
    end if
end if
```

---

## 16. times_table.zz - Times Table Generator

Generate the times table for any number:

```basic
input "Enter a number: " number

print "Times table of " number ": " nl

for i = 1 to 10 do
    let result = number * i
    print number " x" i " =" result nl
end for
```

---

## 17. fibonacci.zz - Fibonacci Sequence

Generate the first N Fibonacci numbers:

```basic
input "How many Fibonacci numbers? " n

let a = 0
let b = 1

print a nl
if (n > 1) then
    print b nl
end if

let i = 2
while (i < n) do
    let c = a + b
    print c nl
    let a = b
    let b = c
    let i = i + 1
end while
```

---

## 18. grade_manager.zz - Grade Manager

Store and analyze student grades:

```basic
# Input grades
print "Enter the grades (enter -1 to stop):" nl
let i = 1
let grade = 0
let grades = array(0)

while (grade != -1) do
    print "Grade" i 
    input ": " grade
    if (grade == -1) then
        break
    end if
    push(grades, grade)
    let i = i + 1
end while

print grades nl

# Calculate statistics
let sum = 0
let i = 0
while (i < len(grades)) do
    let sum = sum + grades[i]
    let i = i + 1
end while

let average = sum / len(grades)

# Display results
print "Average         : " average nl
print "Total grades: " len(grades) nl
```

**NOTE:** `input` only reads one value at a time.

```
# CORRECT
input "n: " n 
print n nl

# ERROR
input "n: " n "m: " m
print n "," m nl
```

---

## 19. temperature_converter.zz - Temperature Converter

Convert between Celsius and Fahrenheit:

```basic
print "Temperature Converter" nl
print "1. Celsius to Fahrenheit" nl
print "2. Fahrenheit to Celsius" nl
input "Choose (1 or 2): " option

if (option == 1) then
    input "Enter temperature in Celsius: " celsius
    let fahrenheit = (celsius * 9 / 5) + 32
    print celsius "°C = " fahrenheit "°F" nl
else
    if (option == 2) then
        input "Enter temperature in Fahrenheit: " fahrenheit
        let celsius = (fahrenheit - 32) * 5 / 9
        print fahrenheit "°F = " celsius "°C" nl
    else
        print "Invalid option" nl
    end if
end if
```

---

## 20. discount_calculator.zz - Discount Calculator

Calculate prices with discount:

```basic
input "Enter the original price: " price
input "Enter the discount percentage: " percentage

let discount = price * (percentage / 100)
let final_price = price - discount

print "Original price: $ " price nl
print "Discount (" percentage "%): $ " discount nl
print "Final price: $ " final_price nl
```

---

## 21. compound_interest_calculator.zz - Compound Interest Calculator

Calculate compound interest:

```basic
input "Initial capital: " capital
input "Annual interest rate (%): " rate
input "Number of years: " years

let amount = capital
let i = 0

while (i < years) do
    let amount = amount * (1 + rate / 100)
    let i = i + 1
end while

print "Initial capital: $ " capital nl
print "Rate: " rate "% p.a." nl
print "Period: " years " years" nl
print "Final amount: $ " amount nl
```

---

## 22. average_function.zz - Function to Calculate Average

Create a reusable function to calculate average:

```basic
function average(grades)
    let sum = 0
    let i = 0
    while (i < len(grades)) do
        let sum = sum + grades[i]
        let i = i + 1
    end while
    return sum / len(grades)
end function

# Using the function
let student_grades = array(0)
push(student_grades, 7.5)
push(student_grades, 8.0)
push(student_grades, 6.5)
push(student_grades, 9.0)

let result = average(student_grades)
print "Grades: " student_grades nl
print "Average: " result nl
```

---

## 23. calculator_with_functions.zz - Calculator with Functions

A complete calculator using functions:

```basic
function sum(a, b)
    return a + b
end function

function subtract(a, b)
    return a - b
end function

function multiply(a, b)
    return a * b
end function

function divide(a, b)
    if (b == 0) then
        print "Error: division by zero!" nl
        return 0
    end if
    return a / b
end function

function modulo(a, b)
    if (b == 0) then
        print "Error: modulo by zero!" nl
        return 0
    end if
    return a % b
end function

# Calculator menu
print "=== CALCULATOR ===" nl
print "1. Sum" nl
print "2. Subtraction" nl
print "3. Multiplication" nl
print "4. Division" nl
print "5. Modulo (remainder)" nl
input "Choose an option: " option

input "Enter the first number: " a
input "Enter the second number: " b

if (option == 1) then
    print a " + " b " = " sum(a, b) nl
else
    if (option == 2) then
        print a " - " b " = " subtract(a, b) nl
    else
        if (option == 3) then
            print a " * " b " = " multiply(a, b) nl
        else
            if (option == 4) then
                print a " / " b " = " divide(a, b) nl
            else
                if (option == 5) then
                    print a " % " b " = " modulo(a, b) nl
                else
                    print "Invalid option!" nl
                end if
            end if
        end if
    end if
end if
```

---

## 24. guess_game.zz - Guessing Game

A complete number guessing game with detailed feedback:

```basic
# --- Number Guessing Game ---

print cyan "=========================================" nocolor nl
print "===   Welcome to the Guessing Game   ===" nl
print cyan "=========================================" nocolor nl

# --- Game Settings ---
let secret_number = 42
let max_attempts = 7
let attempts_left = max_attempts
let guessed_correctly = 0

print "I thought of a number between 1 and 100." nl
print "You have " max_attempts " attempts to guess." nl
print "" nl

# --- Main Game Loop ---
while (attempts_left > 0) do
    input "What is your guess? " guess

    # --- Guess Validation ---
    if (guess < 1 or guess > 100) then
        print magenta "-> Invalid guess! Please,"
        print " enter a number between 1 and 100." nocolor nl
        continue
    end if

    # --- Decrements attempts ---
    let attempts_left = attempts_left - 1

    # --- Guess Comparison ---
    if (guess == secret_number) then
        let guessed_correctly = 1
        break
    else
        if (guess < secret_number) then
            print blue "-> Too low!" nocolor nl
        else
            print red "-> Too high!" nocolor nl
        end if
    end if

    # --- Shows remaining attempts ---
    if (attempts_left > 0) then
        print yellow "You still have " attempts_left " attempts." nocolor nl
    end if
end while

# --- End of Game ---
print "" nl
print cyan "=========================================" nocolor nl

if (guessed_correctly == 1) then
    # --- Victory Message ---
    print green "*** CONGRATULATIONS! YOU GOT IT RIGHT! ***" nocolor nl
    print "The secret number was " secret_number "." nl
else
    # --- Defeat Message ---
    print red "--- GAME OVER! YOU LOST! ---" nocolor nl
    print "The secret number was " secret_number "." nl
end if

print cyan "=========================================" nocolor nl
print "Thank you for playing!" nl
