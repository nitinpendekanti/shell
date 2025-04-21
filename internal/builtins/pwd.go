package builtins

/*** imports ***/

import (
	"utils"
	"log"
	"os"
	"fmt"
)

/*** pwd flag struct ***/

type PwdConfig struct {
	Logical bool
	Physical bool
}

var config PwdConfig

/*** set flags ***/

func setFlag(arg *string) {
	switch (*arg){
	case "L":
		fallthrough
	case "logical":
		config.Logical = true
	case "P":
		fallthrough
	case "physical":
		config.Physical = true
	}
}

/*** main ***/

func Pwd(c *utils.Command) {
	for _, flag := range c.Flags {
		setFlag(&flag)
	}

	dir, err := os.Getwd()
	if err != nil {
		log.Printf("Error occured with Getwd()\n")
	}
	fmt.Println(dir)
}
