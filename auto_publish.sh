#! /bin/bash

function read_dir(){
    pushd $1
    for file in `ls ./`
    do
        if [ -d $file ] 
        then
            if [ "$file" != "CMakeFiles" ]
            then
                pushd $file
                npm publish ./
                popd
            fi
        fi
    done
    popd
}

read_dir $1
