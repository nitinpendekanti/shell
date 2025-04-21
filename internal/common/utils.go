/**********************************************************/
/*** Simple Shell Written in Golang -- Nitin Pendekanti ***/
/**********************************************************/

package utils

import (
	"errors"
)

type Command struct {
	CmdLineInput string
	Script string
	Args []string
	Flags []string
}

func (c *Command) ExtractFlags() error {
	for _, arg := range c.Args {
		if len(arg) < 2 {
			return errors.New("invalid flag entered")
		}

		if arg[0] == '-' && arg[1] == '-' {
			c.Flags = append(c.Flags, arg[2:])
		} else {
			for _, char := range arg {
				if char != '-' {
					c.Flags = append(c.Flags, string(char))
				}
			}
		}
	}

	return nil
}
