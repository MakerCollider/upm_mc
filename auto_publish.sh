#! /bin/bash
function read_dir(){
    for file in `ls $1`
    do
        if [ -d $1"/"$file ] 
        then
            if [ "$file" != "CMakeFiles" ]
            then
                cd $file
                npm publish ./
                cd ../
            fi
        fi
    done
}

read_dir $1
