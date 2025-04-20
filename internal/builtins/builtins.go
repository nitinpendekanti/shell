/**********************************************************/
/*** Simple Shell Written in Golang -- Nitin Pendekanti ***/
/**********************************************************/

package builtins

/*** imports ***/

import (
	"fmt"
	"os"
	"utils"
)

/*** builtin functions ***/

func printErr(script string, e error) {
	fmt.Printf("command %s ran into an unexpected error: %v\n", script, e)	
}

func Pwd(c *utils.Command) {
	dir, err := os.Getwd()
	if err != nil {
		printErr(c.Script, err)
		return
	}
	fmt.Println(dir)
}

func Ls(c *utils.Command) {
	dir, err := os.Getwd()
	if err != nil {
		printErr(c.Script, err)
		return
	}

	files, err2 := os.ReadDir(dir)
	if err2 != nil {
		printErr(c.Script, err2)
		return
	}

	for _, file := range files {
		fileName := file.Name()

		if fileName[0] != '.' {
			fmt.Printf("%s    ", file.Name())
		}
	}
	fmt.Println()
}
