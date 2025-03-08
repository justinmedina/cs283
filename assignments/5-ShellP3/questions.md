### 1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

**Answer:** 
Our implementation ensures that all child processes complete by calling `waitpid()` on each child process in the pipeline. This ensures that the shell does not proceed with accepting new user input until all commands in the pipeline have finished execution. 

If we forget to call `waitpid()`, the child processes become **zombie processes**, which means they have completed execution but are still present in the system’s process table. Over time, this can lead to resource exhaustion and undefined behavior in the shell.

---

### 2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

**Answer:** 
After calling `dup2()`, it is necessary to close unused pipe ends to prevent file descriptor leaks. If a pipe end remains open in a process that does not need it, the operating system will not detect an EOF (end-of-file) when reading from the pipe, causing processes to hang indefinitely.

Leaving pipes open can also lead to **resource exhaustion**, as each open file descriptor consumes system resources. Properly closing unused pipe ends ensures that data flows correctly through the pipeline and that the shell does not encounter unintended hangs or memory leaks.

---

### 3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

**Answer:** 
`cd` (change directory) must be implemented as a built-in command because **changing the working directory is a process-wide operation**. If `cd` were implemented as an external process, it would run in a child process, change the directory, and then exit—leaving the parent shell’s working directory unchanged.

Challenges of implementing `cd` as an external command:
- Each child process has its own copy of the environment, so `cd` would not persist beyond the process that executed it.
- The parent shell would remain in the same directory, rendering `cd` ineffective.
- The only way to change the shell’s directory is by modifying the `PWD` environment variable in the parent process, which can only be done within the shell itself.

Thus, `cd` must be built-in to modify the shell’s current working directory directly.

---

### 4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

**Answer:** 
To allow an **arbitrary number of piped commands**, we would:
1. **Use dynamic memory allocation**: Instead of a static array of `CMD_MAX` commands, we would allocate memory dynamically using `malloc()` and resize as needed with `realloc()`.
2. **Implement linked lists**: Instead of an array, we could use a linked list to store commands, allowing for unlimited pipes without a predefined limit.
3. **Iterate over dynamically allocated commands**: The execution logic would be adjusted to iterate over dynamically allocated structures instead of a fixed array.

**Trade-offs:**
- **Increased complexity**: Managing dynamic memory requires proper allocation and deallocation to prevent memory leaks.
- **Potential performance overhead**: Dynamic allocation can be slower than using a fixed-size array, especially if frequent resizing (`realloc()`) occurs.
- **More error handling required**: We must handle cases where `malloc()` or `realloc()` fails due to insufficient memory.

By carefully balancing these trade-offs, we can create a shell that is both flexible and efficient in handling an arbitrary number of piped commands.

