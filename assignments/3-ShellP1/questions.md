1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: fgets() is a good choice because it safely reads input from the user while preventing buffer overflows by limiting the number of characters read. It also handles EOF correctly. Additionally, fgets() retains spaces and newlines, which is useful for parsing commands accurately.
 

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() allows dynamic memory allocation, making our shell more flexible in handling varying input sizes. A fixed-size array could waste memory if it's too large or lead to buffer overflows if it's too small. Dynamic allocation ensures that the shell can handle larger command lines when needed without hardcoding an arbitrary size limit.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Using malloc() allows dynamic memory allocation, making our shell more flexible in handling varying input sizes. A fixed-size array could waste memory if it's too large or lead to buffer overflows if it's too small. Dynamic allocation ensures that the shell can handle larger command lines when needed without hardcoding an arbitrary size limit. Additionally, trailing spaces could interfere with argument parsing, leading to unexpected behavior when executing commands.


4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: Redirection examples:

    command > output.txt: Redirects STDOUT to a file.
    command < input.txt: Redirects input from a file instead of the keyboard.
    command 2> error.log: Redirects STDERR to a file.

	 Challenges:

    Implementing proper file handling using open(), dup2(), and close().
    Ensuring that file descriptors are correctly restored after redirection.
    Handling cases where files don’t exist or lack the necessary permissions.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection controls where input and output are sent. Piping connects the output of one command to the input of another, enabling data to flow between processes. Redirection affects files, while piping allows commands to work together in a single execution stream.


- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:   Separating STDERR and STDOUT allows error messages to be displayed or logged independently from regular output. This prevents errors from getting mixed into valid data, making debugging easier.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our shell should detect command failures using return codes and check errno for system errors. It should print meaningful error messages to STDERR while allowing STDOUT to function normally. We should also provide a way to merge STDERR and STDOUT using 2>&1, which redirects STDERR to STDOUT
