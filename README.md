# STL Viewer (OpenGL)

A lightweight STL file viewer built using C++ and OpenGL.
This project focuses on rendering 3D geometry, parsing STL files, and building a basic CAD-style viewer.

---

## 🚀 Features

* Supports ASCII & Binary STL files
* Real-time 3D rendering using OpenGL
* Camera controls (zoom, rotate, pan)
* Shader-based pipeline (GLSL)
* Efficient mesh handling using VAO & VBO

---

## 🛠️ Tech Stack

* C++
* OpenGL
* GLFW
* GLAD
* GLM
* stb

---

## 📂 Project Structure

```
src/            → Source files (.cpp)
include/        → Header files (.h)
shaders/        → Vertex & Fragment shaders
resources/      → STL models / assets
Libraries/      → External dependencies (ignored in Git)
```

---

## ⚙️ Setup Instructions

### 1. Clone the repository

```
git clone https://github.com/YOUR_USERNAME/STL-Viewer.git
```

---

### 2. Setup Dependencies

This project uses external libraries which are **not included in the repo**.

Create this structure inside the project:

```
Libraries/
 ├── include/
 └── lib/
```

Then download and place:

#### 🔹 Headers → `Libraries/include/`

* glad
* GLFW
* glm
* stb

#### 🔹 Library → `Libraries/lib/`

* glfw3.lib

---

### 3. Open Project

* Open `STLReflect.sln` in Visual Studio
* Build and run 🚀

---

## 🎯 Future Improvements

* Lighting (Phong / PBR)
* Wireframe mode
* GUI using ImGui
* Drag & drop STL loading
* Model transformations

---

## 📸 Preview

(Add screenshots here)

---

## 👨‍💻 Author

Shubham Agrahari
Software Engineer | CAD & 3D Graphics Enthusiast
