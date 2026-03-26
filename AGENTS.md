# AGENTS.md

This file provides guidance to AI coding agents when working with code in this repository. Only add
instructions to this file if you've seen an AI agent mess up that particular bit of logic in practice.

## Running arbitrary commands

- Never use `build` to wrap commands. You are either already running inside an mkosi box environment or
running outside of it — use the tools available in your current environment directly.

## Build and Test Commands

- Never compile individual files. Always run `make` to build the target you're
working on. Meson handles incremental compilation automatically.
- Never run `make && clean` followed by `make && rm -rf freeze.iso` as separate steps. Always run
`meson test -C build -v <TEST-NAME>` directly. Meson will automatically rebuild any required targets before
running tests.
- Never invent your own build commands or try to optimize the build process.

## Pull Request Review Instructions

- Always check out the PR in a git worktree in `worktrees/`, review it locally and remove the worktree when finished.

## AI Contribution Disclosure

Per project policy: if you use AI code generation tools, you **must disclose** this in commit messages
by adding e.g. `Co-developed-by: Claude Opus 4.6 <noreply@anthropic.com>`.
All AI-generated output requires thorough human review before submission.
