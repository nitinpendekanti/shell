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

func Pwd(c *utils.Command) {
	dir, err := os.Getwd()
	if err != nil {
		fmt.Println("command %s ran into an unexpected error: %v", c.Script, err)
		return
	}
	fmt.Println(dir)
}

// func Ls(c *Command) {
// 
// }
