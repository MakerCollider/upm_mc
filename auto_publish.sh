#! /bin/bash

function read_dir(){
    pushd $1
    for file in `ls $1`
    do
        if [ -d $1"/"$file ] 
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
