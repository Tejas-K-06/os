#!/opt/homebrew/bin/bash

for file in $(find . -type f -name "*.tmp" -mtime -3)
do
    echo "Delete $file? (y/n)"
    read answer

    if [ "$answer" = "y" ]
    then
        rm "$file"
        echo "$file is successfully deleted."
    fi
done
