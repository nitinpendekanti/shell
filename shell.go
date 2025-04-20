/**********************************************************/
/*** Simple Shell Written in Golang -- Nitin Pendekanti ***/
/**********************************************************/

package main

/*** imports ***/

import (
	"fmt"
	"bufio"
	"os"
	"os/exec"
	"strings"
)

/*** data ***/

const ArgsSize = 20

var builtinFuncMap map[string]func(c *Command) 

/*** structs ***/

type Command struct {
	cmdLineInput string
	script string
	args []string
}

/*** built-in functions ***/



/*** arguments handlers ***/

func executeArgs(c *Command) {
	cmd := exec.Command(c.script, c.args[1:]...)
	cmd.Stdout = os.Stdout
	if err := cmd.Run(); err != nil {
		fmt.Printf("Command not found: %s\n", c.script)
	}		
}

func argHandler(c *Command) {
	if c.script == "exit" {
		os.Exit(0)
	} else if c.script == "" {
		return
	} else if builtinFunc, ok := builtinFuncMap[c.script]; ok {
		builtinFunc(c)
	} else {
		executeArgs(c)
	}
}

/*** command initializer & methods ***/

func NewCommand() Command {
	return Command{
		"",
		"",
		make([]string, ArgsSize),
	}
}

func (c *Command) readLine() {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Scan()
	c.cmdLineInput = scanner.Text()
}

func (c *Command) parseLine() {
	c.args = strings.Fields(c.cmdLineInput)
	if len(c.args) > 0 {
		c.script = strings.ToLower(c.args[0])
	}
}

/*** main ***/

func loop() {
	cmd := NewCommand()

	for {
		fmt.Print("> ")
		cmd.readLine()
		cmd.parseLine()
		
		argHandler(&cmd)
	}
}

func main() {
	loop()
}
