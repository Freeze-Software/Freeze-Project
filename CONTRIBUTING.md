# Contributing to FreezeOS (The Freeze Project)

Thank you for your interest in contributing to FreezeOS!

FreezeOS is a hobby operating system focused on simplicity. Whether you're fixing bugs, adding features, or improving documentation, your help is appreciated.

---

## Getting Started

Before contributing, make sure you:

* Understand the basics of C and low-level programming
* Have a working development environment (GCC, Make, QEMU or real hardware)
* Can build and run the OS locally

### Build & Run

```bash
make
```

---

## Ways to Contribute

You can contribute in several ways:

### 1. Bug Fixes

* Fix crashes, undefined behavior, or boot issues
* Improve processing safety and stability

### 2. Features

* Shell improvements 
* Drivers 

### 3. Documentation

* Improve README.md
* Add guides for beginners
* Document kernel subsystems

### 4. Code Cleanup

* Refactor messy code
* Improve structure and readability

---

## Coding Guidelines

Please follow these rules:

* Use clear and consistent naming
* Avoid unnecessary complexity
* No unused code or commented-out blocks

---

## Commit Guidelines

Use clear commit messages:

```
feat: blah blah blah
fix: blah blah blah
refactor: blah blah blah blah blah blah
```
(this is an example, you may structure your commit messages like this if you want)
---

## Pull Requests

When submitting a PR:

* describe what you changed
* explain why the change is needed
* ensure the OS builds and runs

---

## Important Notes

* please  do NOT break boot functionality
* Test your changes in QEMU before submitting

---

## Questions?

If you're unsure about something, open an issue or ask before starting work.

---

Thanks for listening, Clash.
