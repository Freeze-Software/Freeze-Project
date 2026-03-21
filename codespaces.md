# codespace guide

This will guide explains how to build and run the Freeze OS project inside **GitHub Codespaces** using QEMU (non graphic).

---

## 1. Install Required Packages

Open the VS Code terminal inside Codespaces and run:

```bash
sudo apt update
sudo apt install build-essential grub-pc-bin grub-common xorriso qemu-system-x86 -y
