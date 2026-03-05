# ZzBasic Examples v0.7.0

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

Basic operations with numbers:

```basic
let a = 10
let b = 3

print "Sum: " a + b nl
print "Subtraction: " a - b nl
print "Multiplication: " a * b nl
print "Division: " a / b nl
```

---

## 4. comparisons.zz - Comparing Values

Understand comparison operators:

```basic
let x = 10
let y = 20

print "x == y: " x == y nl
print "x != y: " x != y nl
print "x < y: " x < y nl
print "x > y: " x > y nl
print "x <= y: " x <= y nl
print "x >= y: " x >= y nl
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

## 8. arrays_basic.zz - Introduction to Arrays

Work with data collections:

```basic
let numbers = array(5)
push(numbers, 10)
push(numbers, 20)
push(numbers, 30)

print "First: " numbers[0] nl
print "Second: " numbers[1] nl
print "Third: " numbers[2] nl
print "Size: " len(numbers) nl
```

---

## 9. input_output.zz - User Interaction

Read data from the user:

```basic
input "Enter your name: " name
input "Enter your age: " age

print "Hello, " name "!" nl
print "You are " age " years old" nl
```

---

## 10. colors_formatting.zz - Colors and Formatting

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

## 11. bmi_calculator.zz - BMI Calculator

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

## 12. times_table.zz - Times Table Generator

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

## 13. fibonacci.zz - Fibonacci Sequence

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

## 14. grade_manager.zz - Grade Manager

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

NOTE: `input` only reads one value at a time.

```
# CORRECT
input "n: " n 
print n nl

# ERROR
input "n: " n "m: " m
print n "," m nl
```

---

## 15. temperature_converter.zz - Temperature Converter

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

## 16. discount_calculator.zz - Discount Calculator

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

## 17. compound_interest_calculator.zz - Compound Interest Calculator

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

## 18. guess_game.zz - Guessing Game

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
```
