1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  
    We use `fork()` first to create a new process because if we just called `execvp()` directly, it would replace the current shell process with the command we want to run. This means the shell would stop running. By using `fork()`, we make a new child process where the command can run, and the parent (the shell) can keep going and handle more things, like running more commands or waiting for the child to finish.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  
    If `fork()` fails, it returns `-1`, and no new process is created. In my code, I check if `fork()` returns `-1` and if it does, I print an error message and exit the program. This is to make sure we don't keep trying to do things without having a valid process to work with.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  
    `execvp()` looks for the command in directories listed in the `PATH` environment variable. The `PATH` variable is a list of directories that the system searches through to find the command you want to run. If it doesn't find the command in any of these directories, `execvp()` will return an error.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  
    `wait()` in the parent process is important because it makes the parent wait for the child to finish and collect its exit status. If we didn’t call `wait()`, the child process could become a "zombie," meaning it's still there but isn't being properly cleaned up by the system. This can waste resources and leave orphaned processes.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  
    `WEXITSTATUS()` is used to get the exit status of the child process after it finishes. This tells us whether the child process completed successfully or encountered an error. It’s important because it helps the parent process know how the child exited, so the parent can handle it properly (like reporting errors or proceeding with next steps).

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  
    My implementation of `build_cmd_buff()` makes sure that any text in quotes is treated as a single argument, even if it has spaces in it. For example, `echo "Hello World"` should be seen as one argument instead of two (`echo` and `Hello World`). This is necessary to make sure the command is parsed correctly when there are spaces inside quotes, which is common in shell commands.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  
    Compared to the previous assignment, I had to update the parsing logic to handle things like quoted strings and escape characters more carefully. In the past, my code didn’t account for things like spaces inside quotes, so I had to change the way I handled input. One challenge was figuring out how to properly manage nested quotes or escaped characters, but once I understood how to track them, it worked better.

8. For this question, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

    - **What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?**

        > **Answer**:  
        Signals are used in Linux to send notifications to processes about things like errors, termination requests, or interruptions. They’re different from other IPC methods like pipes or shared memory because signals are just notifications that tell a process to stop, pause, or handle something. Other IPC methods allow for more complex interactions, like sending data between processes.

    - **Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?**

        > **Answer**:  
        1. **SIGKILL**: This is used to immediately kill a process, and it can't be stopped or ignored. It's often used when a process is unresponsive and needs to be forcibly terminated.
        2. **SIGTERM**: This is a more graceful termination signal that gives the process a chance to clean up before it exits. It's commonly used to ask a process to shut down properly.
        3. **SIGINT**: This is sent when a user interrupts a process (like pressing `Ctrl+C` in the terminal). It tells the process to stop what it's doing, and the process can choose to handle it or ignore it.

    - **What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?**

        > **Answer**:  
        When a process gets `SIGSTOP`, it gets paused immediately, and the process can't catch or ignore it like `SIGINT`. `SIGSTOP` is special because it’s used to pause a process, and the system makes sure it happens regardless of what the process is doing. You can resume the process later using a `SIGCONT` signal.

