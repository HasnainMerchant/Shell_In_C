# SCH

SCH is a simple implementation of a shell in C. It demonstrates the basics of how a shell works. That is: read, parse, fork, exec, and wait. Since its purpose is demonstration, it has many limitations, including:

- Commands must be on a single line.
- Arguments must be separated by whitespace.
- No quoting arguments or escaping whitespace.
- No piping or redirection.
- Only builtins are: `cd`, `help`, `exit`.

## Running

Use the following command to compile:

```sh
gcc -o sch sch.c
```

Then, run the shell with:

```sh
./sch
```

## Contributing

I'm looking to extend it with additional features but not at this time. So I can accept pull requests that are related to bug fixes or improvement (and I'm sure there are still bugs in the code!).

Feel free to customize it further if needed! If you have any other requests or need more help, just let me know.
