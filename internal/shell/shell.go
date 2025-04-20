/**********************************************************/
/*** Simple Shell Written in Golang -- Nitin Pendekanti ***/
/**********************************************************/

package shell 

/*** imports ***/

import (
	"fmt"
	"bufio"
	"os"
	"os/exec"
	"strings"
	"builtins"
	"utils"
)

/*** data ***/

const ArgsSize = 20

var builtinFuncMap = map[string]func(c *utils.Command) {
	"pwd": builtins.Pwd,
	"ls":  builtins.Ls,
}

/*** arguments handlers ***/

func executeArgs(c *utils.Command) {
	cmd := exec.Command(c.Script, c.Args[1:]...)
	cmd.Stdout = os.Stdout
	if err := cmd.Run(); err != nil {
		fmt.Printf("Command not found: %s\n", c.Script)
	}		
}

func argHandler(c *utils.Command) {
	if c.Script == "exit" {
		os.Exit(0)
	} else if c.Script == "" {
		return
	} else if builtinFunc, ok := builtinFuncMap[c.Script]; ok {
		builtinFunc(c)
	} else {
		executeArgs(c)
	}
}

/*** command initializer & methods ***/

func newCommand() utils.Command {
	return utils.Command{
		"",
		"",
		make([]string, ArgsSize),
	}
}

func readLine(c *utils.Command) {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Scan()
	c.CmdLineInput = scanner.Text()
}

func parseLine(c *utils.Command) {
	c.Args = strings.Fields(c.CmdLineInput)
	if len(c.Args) > 0 {
		c.Script = strings.ToLower(c.Args[0])
	}
}

/*** main ***/

func Loop() {
	cmd := newCommand()

	for {
		fmt.Print("> ")
		readLine(&cmd)
		parseLine(&cmd)
		argHandler(&cmd)
	}
}
