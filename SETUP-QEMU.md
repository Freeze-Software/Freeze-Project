# codespace guide

This will guide explains how to build and run the Freeze OS project using **QEMU**.

---

## 1. Install Required Packages

Open qemu and run:

```bash
sudo apt update
sudo apt install build-essential grub-pc-bin grub-common xorriso qemu-system-x86 -y
```

## 2. Build the Project
From the root of the repo:
```bash
make
```

if you want you may check for the iso file:
```bash
ls
```

> Thanks for listening
