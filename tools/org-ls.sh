#!/bin/bash

dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

ls $1 | $dir/_ls-org.awk > ls.org

