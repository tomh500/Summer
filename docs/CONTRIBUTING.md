# 🤝 Contributing to Summer/Autumn 3.1

Thank you for your interest in contributing to Summer/Autumn! This guide will help you understand our development process, coding standards, and how to contribute effectively.

## 📋 Table of Contents

- [Development Environment Setup](#-development-environment-setup)
- [Code Style Guidelines](#-code-style-guidelines)
- [Module Development](#-module-development)
- [Testing Guidelines](#-testing-guidelines)
- [Pull Request Process](#-pull-request-process)
- [Issue Reporting](#-issue-reporting)
- [Documentation Standards](#-documentation-standards)

## 🛠️ Development Environment Setup

### Prerequisites

#### Windows Development

```bash
# Install Visual Studio 2019 or later
# Install CMake 3.16 or later
# Install vcpkg for dependency management

# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Install dependencies
.\vcpkg install sdl2 sdl2-mixer nlohmann-json cpp-httplib simdjson
```

#### Linux Development

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake git
sudo apt-get install libsdl2-dev libsdl2-mixer-dev
sudo apt-get install nlohmann-json3-dev cpp-httplib-dev
sudo apt-get install libsimdjson-dev

# Fedora/RHEL
sudo dnf install gcc-c++ cmake git
sudo dnf install SDL2-devel SDL2_mixer-devel
sudo dnf install json-devel cpp-httplib-devel
sudo dnf install simdjson-devel
```

### Repository Setup

```bash
# Fork the repository on GitHub
git clone https://github.com/YourUsername/Summer.git
cd Summer

# Add upstream remote
git remote add upstream https://github.com/neKamita/Summer.git

# Create development branch
git checkout -b feature/your-feature-name
```

### Building the Project

```bash
# Navigate to C++ source directory
cd code/CPP-OpenSource

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
# Linux
make -j$(nproc)

# Windows (Visual Studio)
cmake --build . --config Release
```

## 📝 Code Style Guidelines

### C++ Coding Standards

#### Naming Conventions

```cpp
// Classes: PascalCase
class ModuleLoader {
public:
    // Public methods: camelCase
    bool loadModule(const std::string& name);

    // Private members: m_ prefix + camelCase
private:
    std::vector<Module*> m_loadedModules;
    bool m_initialized;
};

// Constants: UPPER_CASE
const int MAX_MODULES = 32;

// Enums: PascalCase
enum class ModuleState {
    Unloaded,
    Loading,
    Loaded,
    Error
};
```

#### Code Organization

```cpp
// Header files (.h)
class AudioManager {
public:
    // Constructor/Destructor
    AudioManager();
    ~AudioManager();
    
    // Core functionality
    bool initialize();
    void shutdown();
    void update();
    
    // Specific operations
    bool playSound(const std::string& filename);
    void stopAllSounds();
    
private:
    // Member variables
    SDL_AudioSpec m_audioSpec;
    std::map<std::string, Mix_Chunk*> m_soundCache;
};
```

#### Memory Management

```cpp
// Use RAII and smart pointers
std::unique_ptr<Module> createModule(const std::string& name) {
    auto module = std::make_unique<CustomModule>();
    if (!module->initialize()) {
        return nullptr;
    }
    return module;
}

// Proper cleanup
class ResourceManager {
    ~ResourceManager() {
        for (auto& [name, resource] : m_resources) {
            resource.reset();
        }
    }
};
```

### Configuration Files

- Use consistent indentation (4 spaces)
- Add descriptive comments
- Group related settings together
- Use meaningful variable names

```cfg
// User bindings - Movement
bind "w" "+forward"
bind "a" "+moveleft"
bind "s" "+back"
bind "d" "+moveright"

// User bindings - Weapons
bind "1" "slot1"
bind "2" "slot2"
bind "3" "slot3"
```

## 🧩 Module Development

### Module Interface

All modules must implement the `IModule` interface:

```cpp
class IModule {
public:
    virtual ~IModule() = default;
    
    // Module identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Lifecycle
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void update() = 0;
    
    // Event handling
    virtual void onGameStateChanged(const GameState& state) = 0;
};
```

### Example Module Implementation

```cpp
// CustomModule.h
class CustomModule : public IModule {
public:
    std::string getName() const override { return "CustomModule"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Custom functionality"; }
    
    bool initialize() override;
    void shutdown() override;
    void update() override;
    void onGameStateChanged(const GameState& state) override;
    
private:
    bool m_enabled;
    ConfigData m_config;
};

// CustomModule.cpp
bool CustomModule::initialize() {
    // Load configuration
    if (!loadConfig("config/custom_module.json")) {
        return false;
    }
    
    m_enabled = true;
    return true;
}

void CustomModule::onGameStateChanged(const GameState& state) {
    if (!m_enabled) return;
    
    // Handle game state changes
    if (state.round.phase == "live") {
        // Round started logic
    }
}
```

## 🧪 Testing Guidelines

### Unit Testing

```cpp
// Use a testing framework like Google Test
#include <gtest/gtest.h>

class ModuleLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        loader = std::make_unique<ModuleLoader>();
    }
    
    std::unique_ptr<ModuleLoader> loader;
};

TEST_F(ModuleLoaderTest, LoadValidModule) {
    EXPECT_TRUE(loader->loadModule("TestModule"));
    EXPECT_EQ(loader->getLoadedModuleCount(), 1);
}

TEST_F(ModuleLoaderTest, LoadInvalidModule) {
    EXPECT_FALSE(loader->loadModule("NonExistentModule"));
    EXPECT_EQ(loader->getLoadedModuleCount(), 0);
}
```

### Integration Testing

- Test GSI server responses
- Verify module interactions
- Test configuration loading
- Validate audio playback

### Manual Testing

1. **In-game Testing**:
   - Load CS2 with the configuration
   - Test all module functionalities
   - Verify GSI events are processed
   - Check audio feedback

2. **Performance Testing**:
   - Monitor CPU/memory usage
   - Test with multiple modules loaded
   - Verify no game performance impact

## 📋 Pull Request Process

### Before Creating a PR

1. **Update your fork**:
   ```bash
   git fetch upstream
   git checkout main
   git merge upstream/main
   git push origin main
   ```

2. **Rebase your feature branch**:
   ```bash
   git checkout feature/your-feature
   git rebase main
   ```

3. **Run tests**:
   ```bash
   # Build and test
   cd build
   make test
   ```

### PR Template

Use this template for your pull request:

```markdown
## Description
Brief description of changes.

## Changes Made
- List specific changes
- Include any breaking changes

## Testing
- How was this tested?
- What test scenarios were covered?

## Screenshots
Include screenshots for UI changes.

## Checklist
- [ ] Code follows style guidelines
- [ ] Tests pass
- [ ] Documentation updated
- [ ] No breaking changes (or documented)
```

### Code Review Process

1. **Automated Checks**: CI/CD will run automated tests
2. **Peer Review**: At least one maintainer will review
3. **Testing**: Changes will be tested in development environment
4. **Approval**: Required before merging

## 🐛 Issue Reporting

### Bug Reports

Include the following information:

- **System Information**:
  - Operating System
  - CS2 Version
  - Project Version

- **Steps to Reproduce**:
  1. Detailed step-by-step instructions
  2. Expected behavior
  3. Actual behavior

- **Additional Information**:
  - Configuration files
  - Debug logs
  - Screenshots/videos

### Feature Requests

- **Use Case**: Describe the problem you're trying to solve
- **Proposed Solution**: Your suggested approach
- **Alternatives**: Other solutions you've considered
- **Implementation**: Any implementation details

## 📚 Documentation Standards

### Code Documentation

```cpp
/**
 * @brief Loads and initializes a module by name
 * @param name The module name to load
 * @param config Optional configuration data
 * @return true if successful, false otherwise
 * @throws ModuleException if module is corrupted
 */
bool loadModule(const std::string& name, const Config& config = {});
```

### Markdown Guidelines

- Use clear, descriptive headings
- Include code examples
- Add table of contents for long documents
- Use consistent formatting
- Include screenshots for UI elements

### API Documentation

- Document all public interfaces
- Include usage examples
- Explain parameters and return values
- Document exceptions and error conditions

---

## 🤝 Community Guidelines

- Be respectful and inclusive
- Help other contributors
- Follow the Code of Conduct
- Ask questions if unsure
- Share knowledge and best practices

Thank you for contributing to Summer/Autumn 3.1! 🎮