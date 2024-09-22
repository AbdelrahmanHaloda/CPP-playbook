#  BUILD TOOLS

**Make**(opens in a new tab) and **CMake**(opens in a new tab) are two separate and similar build tools(opens in a new tab) that both serve to help simplify the process of building(opens in a new tab) software.

In particular, build tools automate the process of compiling multiple source code files into object files, linking those object files together, and generating an executable. Build tools also often automate the process of determining which files have changed since the last build and thus need to be recompiled.

C++ Build Process Diagram: Preprocessing, Compiling, Linking
C++ Build Process

---
<u>**Make**</u>
GNU **Make**(opens in a new tab) is a widely-used build tool that relies on **Make**files to automate the process of building a project.

A **Make**file typically includes one or more "targets". Each target performs a different action.

build is a common target name that is configured in the **Make**file to compile all of the project's source code into an executable file. clean, on the other hand, is a common target to delete all object files and other artifacts of the build process, resulting in a clean, unbuilt project state.

Running either **Make** build or **Make** clean (or any other target) on the command line would cause **Make** to search for a local **Make**file, search for a matching target within that **Make**file, and then execute the target.

---
<u>**CMake**</u>
**CMake**(opens in a new tab) is a build tool that facilitates cross-platform builds, so that it is straightforward to build the same source code on Linux, macOS, Windows, or any other operating system. **CMake** relies on a **CMake**Lists.txt file, which configures appropriate cross-platform targets.

Building a **CMake**Lists.txt file can be a bit daunting, but **CMake** provides a helpful tutorial(opens in a new tab).

In this Nanodegree program, you will not need to build your own **Make**files or **CMake**Lists.txt files. We provide the appropriate configuration files for each project and instruct you as to their usage.

[Getting Started with CMake](https://cmake.org/getting-started/)


[GNU Make](https://www.gnu.org/software/make/manual/html_node/index.html#Top)

