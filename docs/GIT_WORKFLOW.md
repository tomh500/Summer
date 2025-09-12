# Git Workflow Guide

## Overview

This guide explains the Git workflow for contributing to the Summer/Autumn 3.0 project. Follow these practices to ensure clean, maintainable code and efficient collaboration.

## Fork Setup

### 1. Initial Fork Configuration

After forking the repository, set up your local development environment:

```bash
# Clone your fork
git clone https://github.com/YOUR_USERNAME/Summer.git
cd Summer

# Add upstream remote (original repository)
git remote add upstream https://github.com/ORIGINAL_OWNER/Summer.git

# Verify remotes
git remote -v
```

Expected output:
```
origin    https://github.com/YOUR_USERNAME/Summer.git (fetch)
origin    https://github.com/YOUR_USERNAME/Summer.git (push)
upstream  https://github.com/ORIGINAL_OWNER/Summer.git (fetch)
upstream  https://github.com/ORIGINAL_OWNER/Summer.git (push)
```

### 2. Keep Fork Synchronized

Before starting new work, always sync with upstream:

```bash
# Fetch latest changes from upstream
git fetch upstream

# Switch to main branch
git checkout main

# Merge upstream changes
git merge upstream/main

# Push updates to your fork
git push origin main
```

## Branch Strategy

### Branch Naming Conventions

Use descriptive branch names with prefixes:

- `docs/` - Documentation improvements
- `feature/` - New features
- `fix/` - Bug fixes
- `refactor/` - Code refactoring
- `test/` - Testing improvements
- `chore/` - Maintenance tasks

Examples:
```bash
git checkout -b docs/update-installation-guide
git checkout -b feature/audio-volume-control
git checkout -b fix/memory-leak-in-gsi
git checkout -b refactor/cleanup-installer-code
```

### Branch Lifecycle

1. **Create Feature Branch**: Always branch from latest `main`
2. **Work in Isolation**: Keep changes focused and atomic
3. **Regular Commits**: Commit early and often with descriptive messages
4. **Push to Fork**: Push feature branch to your fork
5. **Create Pull Request**: Open PR when ready for review
6. **Clean Up**: Delete feature branch after merge

## Commit Guidelines

### Commit Message Format

Follow conventional commit format:

```
type(scope): brief description

Detailed explanation of what was changed and why.
Include any breaking changes or important notes.

Fixes #123
Closes #456
```

### Commit Types

- `feat:` - New feature
- `fix:` - Bug fix
- `docs:` - Documentation changes
- `style:` - Code style changes (formatting, etc.)
- `refactor:` - Code refactoring
- `test:` - Adding or updating tests
- `chore:` - Maintenance tasks

### Good Commit Examples

```bash
# Feature addition
git commit -m "feat(audio): add volume control for bomb sounds

Implements adjustable volume levels for bomb countdown audio.
Adds slider control in settings UI and persistence in config.

Fixes #45"

# Bug fix
git commit -m "fix(gsi): resolve memory leak in MixChunkPtr

Properly manages SDL audio chunk lifecycle to prevent
use-after-free errors during bomb sound playback.

Closes #67"

# Documentation
git commit -m "docs: add comprehensive Git workflow guide

Includes fork setup, branch strategy, commit guidelines,
and pull request process for new contributors."
```

## Pull Request Process

### 1. Pre-PR Checklist

Before creating a pull request:

- [ ] Code compiles without errors
- [ ] All tests pass (if applicable)
- [ ] Code follows project style guidelines
- [ ] Documentation updated (if needed)
- [ ] Commit messages are clear and descriptive
- [ ] Branch is up-to-date with main

### 2. Creating the Pull Request

1. **Push to Fork**:
   ```bash
   git push origin feature/your-branch-name
   ```

2. **Open PR on GitHub**:
   - Navigate to your fork on GitHub
   - Click "Compare & pull request"
   - Fill out PR template completely
   - Add descriptive title and detailed description

### 3. PR Title and Description

**Good PR Title**:
```
feat(installer): add unified configuration installer system
```

**PR Description Template**:
```markdown
## Description
Brief summary of changes and motivation.

## Changes Made
- List specific changes
- Include file modifications
- Mention any breaking changes

## Testing
- How was this tested?
- What scenarios were covered?

## Screenshots (if applicable)
Include visual changes or new UI elements.

## Checklist
- [ ] Code compiles successfully
- [ ] Tests pass
- [ ] Documentation updated
- [ ] Breaking changes documented
```

## Code Review Process

### What to Expect

1. **Initial Review**: Maintainers will review within 48-72 hours
2. **Feedback**: Address comments and suggestions promptly
3. **Iteration**: Make requested changes in new commits
4. **Approval**: Once approved, PR will be merged
5. **Cleanup**: Delete feature branch after merge

### Responding to Feedback

```bash
# Make requested changes
vim src/file.cpp

# Commit changes
git add src/file.cpp
git commit -m "fix: address review feedback for memory management"

# Push updated branch
git push origin feature/your-branch-name
```

## Advanced Git Workflows

### Interactive Rebase

Clean up commit history before PR:

```bash
# Interactive rebase last 3 commits
git rebase -i HEAD~3

# In editor, you can:
# pick - keep commit as-is
# reword - change commit message
# squash - combine with previous commit
# drop - remove commit
```

### Handling Merge Conflicts

```bash
# Update your branch with latest main
git fetch upstream
git checkout feature/your-branch
git rebase upstream/main

# If conflicts occur:
# 1. Edit conflicted files
# 2. Stage resolved files
git add resolved-file.cpp

# 3. Continue rebase
git rebase --continue

# 4. Force push updated branch
git push --force-with-lease origin feature/your-branch
```

### Cherry-picking

Apply specific commits to another branch:

```bash
# Apply commit to current branch
git cherry-pick <commit-hash>

# Apply multiple commits
git cherry-pick <commit1> <commit2> <commit3>
```

## Best Practices

### Commit Practices

1. **Atomic Commits**: Each commit should represent a single logical change
2. **Clear Messages**: Write commit messages for future you
3. **Test Before Commit**: Ensure code compiles and tests pass
4. **Regular Commits**: Don't let changes accumulate too long

### Branch Management

1. **Keep Branches Small**: Easier to review and merge
2. **Regular Updates**: Sync with upstream frequently
3. **Clean History**: Use interactive rebase to clean up
4. **Delete Merged Branches**: Keep repository clean

### Collaboration

1. **Communicate Changes**: Discuss major changes before implementation
2. **Review Thoroughly**: Take time to understand others' code
3. **Be Constructive**: Provide helpful, actionable feedback
4. **Stay Updated**: Follow project discussions and updates

## Troubleshooting

### Common Issues

**Problem**: Merge conflicts
```bash
# Solution: Rebase instead of merge
git fetch upstream
git rebase upstream/main
```

**Problem**: Accidentally committed to wrong branch
```bash
# Solution: Move commits to correct branch
git log --oneline  # Find commit hash
git checkout correct-branch
git cherry-pick <commit-hash>
git checkout wrong-branch
git reset --hard HEAD~1  # Remove commit from wrong branch
```

**Problem**: Need to modify last commit
```bash
# Solution: Amend last commit
git add modified-files
git commit --amend
```

**Problem**: Pushed changes but need to modify
```bash
# Solution: Force push with lease (safer)
git commit --amend
git push --force-with-lease origin feature-branch
```

---

## Quick Reference

### Essential Commands

```bash
# Setup
git clone <fork-url>
git remote add upstream <original-url>

# Daily workflow
git fetch upstream
git checkout main
git merge upstream/main
git push origin main
git checkout -b feature/new-feature

# Commit and push
git add .
git commit -m "type(scope): description"
git push origin feature/new-feature

# Cleanup
git checkout main
git branch -d feature/new-feature
```

### Useful Aliases

Add to your `~/.gitconfig`:

```ini
[alias]
    co = checkout
    br = branch
    ci = commit
    st = status
    unstage = reset HEAD --
    last = log -1 HEAD
    visual = !gitk
    lg = log --oneline --graph --all
    up = pull --rebase upstream main
```

This workflow ensures clean, maintainable code and smooth collaboration. Happy coding! 🚀