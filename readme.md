# Instructions for Improving the Code

Your task is to analyze and refactor the given code to improve its encapsulation, adherence to the Law of Demeter, avoidance of God objects, applying "ask-don't-tell" and elimination of code duplication (DRY). 
Below are the specific instructions to guide your refactoring process:

## 1. Encapsulation

Ensure that each class encapsulates its data and provides controlled access through methods. No data members should be directly accessible outside the class.

## 2. Adherence to the Law of Demeter

Objective: Avoid accessing the internal structure of objects (e.g., avoid chained method calls like player.getThing.doSomthing()).

Move responsibilities for actions (e.g., attacking, equipping weapons) into the relevant classes (like Character or WeaponInventory).

## 3. Avoiding God Objects

Reduce the responsibilities of overly large classes by delegating responsibilities to other objects.

## 4 . Tell-Don’t-Ask Principle

Avoid extracting data from objects and making decisions in another class. Instead, tell the objects what to do by delegating responsibility to them.

## 5. Eliminating Code Duplication (DRY Principle)

Refactor repetitive code into reusable methods.

Look for duplicated logic, such as the repeated calls for "randomly equipping weapons" or "applying damage."

Extract these into helper methods in appropriate classes.

## 6. Improving Maintainability:

Ensure each class has a single responsibility and does not mix unrelated functionality.

