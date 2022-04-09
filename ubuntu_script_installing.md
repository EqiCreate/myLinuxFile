## installed script
### github
- git 
  ```
  sudo apt install git;
  git config [key] [value];
  ```
- access
 
 ```
    sudp chmod o+rwx [name] ;

 ```
 - touchpad
 ```
    xinput list; (look for touchpad 's id as %id)
    xinput set-prop 14 "Device Enabled" 0
 ```
 - tar
 ```
    mv * ../;# mv to uplevel file Dictory
    mv !(nodejs) nodejs # move to nodejs exclude node itself;
 ```
 - bashrc
 ```c#
    vim ~/.bashrc;
    source ~/.bashrc;# update sources
 ```
 - node
  ```
    npm install yarn -g;
  ```
- dotnet 

  [Dotnet SDK or Runtime ](https://docs.microsoft.com/en-us/dotnet/core/install/linux-snap)
 - other 
 ```
  sudo apt install npm/curl;
 ```
 