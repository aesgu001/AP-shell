# AP-shell
An emulator of the [GNU Bash Shell](https://www.gnu.org/software/bash/) developed using C. It parses the command line input, generates commands, and exexcutes them as 
processes. The program also has the "change directory" `cd` command built in to it.
## Input/Output Redirection
In addition to extracting their arguments, *AP-shell* can direct commands to read from an existing file with the `<` operator, and write to a new or existing file with
`>` or `>>`. The `>` operator allows users to overwrite an existing file via truncation, while `>>` appends to it instead. Each command, however, only accepts 1 input 
and 1 output redirection at a time.
## Command Piping
Commands can be linked to one or more commands with the `|` operator. During the execution phase, the program creates a pipeline for processes to communicate with one 
another through standard I/O. The first process can also perform input redirection, while the last process can do output redirection.
## Change Directory
Similar to *Bash*, *AP-shell* has a built-in `cd` command as the `exec` system calls do not recognize it as an executable file. In this program, `cd` changes its
current working directory to one specified by the user. Option `-` refers to a previous directory, whereas `~` refers to the home directory.
## Interruption Signal
Users can return control back to the shell by pressing `CTRL+C`. This stops any foreground process as well as other queued commands from executing before returning to 
the command prompt.
## Demo
Click the link [here](https://github.com/aesgu001/AP-shell/blob/main/docs/Demo.md) to view a demo of the project.
## Build and Usage
**Note:** The GNU C compiler package must be installed to build the application.

Building and running *AP-shell* takes 5 steps using the Linux terminal:
```
git clone https://github.com/aesgu001/AP-shell.git
cd AP-shell
git checkout v1.0.0
make
bin/AP-shell
```
