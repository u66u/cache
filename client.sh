#!/bin/bash

echo "Testing Cache Server"

echo -e "\nSetting key 'hello' to value 'world'"
curl -X POST -d "command=SET&key=hello&value=world" http://127.0.0.1:9999

echo -e "\n\nGetting value for key 'hello'"
curl -X POST -d "command=GET&key=hello" http://127.0.0.1:9999

echo -e "\n\nTrying to get a non-existent key 'notfound'"
curl -X POST -d "command=GET&key=notfound" http://127.0.0.1:9999

echo -e "\n\nSetting key 'special characters' to 'value with spaces'"
curl -X POST -d "command=SET&key=special%20characters&value=value%20with%20spaces" http://127.0.0.1:9999

echo -e "\n\nGetting value for key 'special characters'"
curl -X POST -d "command=GET&key=special%20characters" http://127.0.0.1:9999

echo -e "\n" 