
proc Error {} {
	puts -nonewline "\033\[31mERROR\033\[0m"
}

proc Ok {} {
	puts -nonewline "\033\[32mOK   \033\[0m"
}

proc Check {cmd result} {
	send -- $cmd\r
	expect {
		$result	Ok
		timeout { Error; send -- $cmd\r}
	}
	puts "  $cmd"
}

