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
	"os/user"
	"strings"
	"builtins"
	"utils"
	"log"
)

/*** data ***/

const ArgsSize = 20
const FlagsSize = 25

var builtinFuncMap = map[string]func(c *utils.Command) {
	"pwd": builtins.Pwd,
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
		make([]string, 0, ArgsSize),
		make([]string, 0, FlagsSize),
	}
}

func readLine(c *utils.Command) {
	scanner := bufio.NewScanner(os.Stdin)
	scanner.Scan()
	c.CmdLineInput = scanner.Text()
}

func parseLine(c *utils.Command) error {
	c.Args = strings.Fields(c.CmdLineInput)
	if len(c.Args) > 0 {
		c.Script = strings.ToLower(c.Args[0])
	}
	err := c.ExtractFlags()

	return err
}

/*** prompt creator ***/

func getPrompt() (string, error) {
	currUser, err := user.Current()
	hostname, err2 := os.Hostname()

	if err != nil {
		return "", err
	} else if err2 != nil {
		return "", err2
	}

	username := currUser.Username
	path, err3 := os.Getwd()

	if err3 != nil {
		return "", err3
	}

	parts := strings.Split(path, "/") 
	dir := parts[len(parts) - 1]

	prompt := fmt.Sprintf("%s@%s %s > ", username, hostname, dir)
	return prompt, nil
}

/*** main ***/

func Loop() {
	cmd := newCommand()

	for {
		prompt, promptErr := getPrompt()

		if promptErr != nil {
			log.Fatalf("Error with getting prompt: %s\n", promptErr)
		}

		fmt.Print(prompt)
		readLine(&cmd)
		err := parseLine(&cmd)

		if err == nil {
			argHandler(&cmd)
		}
	}
}
