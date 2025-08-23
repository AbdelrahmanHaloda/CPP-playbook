# GDB, Objdump & Debugging Quick Guide / Cheat Sheet


## 1. Quick Guide
### 🔹 What is GDB?

GDB (GNU Debugger) helps debug programs written in C, C++, etc.

Lets you: **run**, **pause**, **step**, inspect variables, backtrace crashes, attach to processes.

### 🔹 What is Objdump?

Objdump displays information about object files (.o, executables, libraries).

Useful for symbol tables, disassembly, and checking compiled binaries.

### 🔹 What is Demangle?

C++ compilers mangle symbol names (e.g., _Z3fooi instead of foo(int)).

c++filt or objdump --demangle makes symbols human-readable.

### 🔹 What is a Core Dump?

Snapshot of a program’s memory at crash.

**Debug with:**
```shell
gdb ./a.out core
```

---

### 2. GDB Quick Guide (Common Commands)

| Command             | Explanation                     |
| ------------------- | ------------------------------- |
| `gdb ./a.out`       | Start debugging program         |
| `run [args]`        | Run program with optional args  |
| `break main`        | Break at function `main`        |
| `break file.cpp:42` | Break at line 42                |
| `next` (`n`)        | Step over (skip into functions) |
| `step` (`s`)        | Step into function              |
| `continue` (`c`)    | Resume execution                |
| `print var`         | Print value of variable         |
| `display var`       | Auto-print every step           |
| `info locals`       | Show local variables            |
| `backtrace` (`bt`)  | Show call stack                 |
| `frame n`           | Switch to stack frame `n`       |
| `quit`              | Exit GDB                        |

---

### 3. Cheat Sheet (Full Command Reference)
#### 🔹 Running & Attaching

```shell
gdb ./a.out            # Start GDB with binary
gdb ./a.out core       # Debug core dump
gdb -p <pid>           # Attach to running process
```

#### 🔹 Breakpoints
```shell
break main
break file.cpp:42
break myFunction
delete 1                # Remove breakpoint 1
disable 2               # Disable breakpoint 2
```

#### 🔹 Execution
```shell
run arg1 arg2
continue   # resume
next       # step over
step       # step into
finish     # run until function returns
```

#### 🔹 Inspecting
```shell
print var
print *ptr
print arr[5]
info locals
info args
info registers
display var
```

#### 🔹 Memory
```shell
x/10x &var      # Examine 10 hex words
x/20i $pc       # Disassemble 20 instructions at program counter
```

#### 🔹 Stack
```shell
backtrace       # Show stack trace
frame 2         # Switch to frame
info frame      # Details about frame
```

#### 🔹 Objdump
```shell
objdump -t a.out                # Symbol table
objdump -d a.out                # Disassemble binary
objdump -t a.out --demangle     # Demangle C++ symbols
```

#### 🔹 Demangle Directly
```shell
c++filt _Z3fooi    # Output: foo(int)
```

#### 🔹 Grep Variables
```shell
objdump -t a.out --demangle | grep temp
nm -C a.out | grep myVar
```

#### 🔹 Core Dumps
```shell
ulimit -c unlimited   # Enable core dumps
gdb ./a.out core
bt  
```

---

### 5. Tips & Tricks

**Conditional Breakpoint:**
```gdb
break myFunction if x > 5
```

**Watchpoints (stop when variable changes):**
```gdb
watch myVar
```

**Load pretty printers (C++ STL debugging):**
```gdb
python import sys; sys.path.insert(0, '/usr/share/gcc-*/python')
from libstdcxx.v6.printers import register_libstdcxx_printers
register_libstdcxx_printers(None)
```

**Attach to a live process:**
```gdb
gdb -p <pid>
```

**Disassemble function inside GDB:**
```gdb
disassemble main
```

**Redirect GDB output:**
```gdb
set logging on
```

---