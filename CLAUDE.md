# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Global
Remember that you are on a windows 11 machine and you are running in powershell, so always use powershell commands when executing terminal commands, never bash commands.

## Build System

This is a Visual Studio 2022 C++ project using MSBuild. The main solution file is `Direct3D11Renderer.sln`.

### Common Build Commands

**IMPORTANT: After making any code changes, ALWAYS run the build using these exact commands to verify the build succeeds:**

Use the full path to MSBuild to ensure it's found correctly:
- **Build Debug**: `& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "Direct3D11Renderer.sln" /p:Configuration=Debug /p:Platform=x64`
- **Build Release**: `& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "Direct3D11Renderer.sln" /p:Configuration=Release /p:Platform=x64`
- **Clean**: `& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" "Direct3D11Renderer.sln" /t:Clean`

**Alternative shorter commands (if MSBuild is in PATH):**
- **Build Debug**: `MSBuild "Direct3D11Renderer.sln" /p:Configuration=Debug /p:Platform=x64`
- **Build Release**: `MSBuild "Direct3D11Renderer.sln" /p:Configuration=Release /p:Platform=x64`
- **Clean**: `MSBuild "Direct3D11Renderer.sln" /t:Clean`

**Build Verification:**
- Always run a build after making code changes to ensure compilation succeeds
- Check for any build errors or warnings and address them before proceeding
- The build output will be in `x64/Debug/` or `x64/Release/` directories

### Project Configuration
- **Language Standard**: C++20
- **Platform**: x64 (primary), Win32 (secondary)
- **Runtime Library**: MultiThreaded (Release) / MultiThreadedDebug (Debug)
- **Dependencies**: DirectXTex.lib, assimp-vc143-mt.lib
- **Output**: Windows subsystem application (Direct3D11Renderer.exe)

## Project Architecture

### Core Components
- **Application** (`Core/Application.h/cpp`): Main application class that orchestrates the rendering loop, manages the window, timer, camera, lighting, and model loading
- **Graphics** (`Core/Graphics.h/cpp`): DirectX 11 abstraction layer handling device creation, render targets, view/projection matrices, and frame rendering
- **Window** (`Core/Window.h/cpp`): Win32 window management with message handling

### Rendering Pipeline
- **Bindable System** (`Bindable/`): Resource binding abstraction for D3D11 objects (shaders, buffers, textures, etc.)
  - `BindableCache.h`: Caches bindable resources to avoid duplication
  - Individual bindables: `VertexShader`, `PixelShader`, `VertexBuffer`, `IndexBuffer`, `Texture`, etc.
- **Renderable Objects** (`Renderable/`): Objects that can be rendered in the scene
  - `Model`: Complex 3D models loaded via Assimp with scene graph structure
  - `Node`: Scene graph nodes with transformation matrices for hierarchical rendering
  - `Mesh`: Individual mesh data with associated bindables
- **Shaders**: HLSL shaders in `shaders/` compiled to `.cso` files in `shaders/Output/`

### Asset Loading
- **Model Loading**: Uses Assimp library for loading 3D models (OBJ, GLTF, GLB formats)
- **Texture Loading**: DirectXTex for texture loading and processing
- **Shader Compilation**: HLSL shaders compiled during build process

### Camera System
- **FreeFlyCamera**: Free-flying camera with WASD movement and mouse look
- **FixedCamera**: Static camera for fixed viewpoints

### Utilities and Exception Handling
- **Exception System** (`Exceptions/`): Comprehensive error handling with custom exceptions for DirectX, windowing, model loading, and bindable lookup
- **Utilities** (`Utilities/`): Helper classes for timing (`D3Timer`), math operations (`MathUtils`), and string conversion (`D3Utils`)

### External Dependencies
- **ImGui**: Embedded for debugging UI and runtime controls
- **Assimp**: For 3D model loading and scene graph construction
- **DirectXTex**: For texture loading and processing
- **DirectX 11**: Core graphics API

### File Structure
- `src/`: Implementation files (.cpp)
- `include/`: Header files (.h) organized by functionality
- `shaders/`: HLSL shader source files
- `assets/`: Textures, icons, and 3D model files
- `lib/`: Static library files
- Third-party headers are included in `include/assimp/` and `include/imgui/`

### Key Architectural Patterns
- **Scene Graph**: Models use a hierarchical node structure for complex transformations
- **Resource Caching**: Bindable resources are cached to prevent duplicate D3D11 object creation
- **Component-Based Rendering**: Renderable objects compose bindables for flexible rendering configurations
- **RAII**: Smart pointers and ComPtr used throughout for automatic resource management