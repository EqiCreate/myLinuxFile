## installed script
### github
- git 
  ```c#
  sudo apt install git;
  git config [key] [value];
  git config --global http.proxy 127xxxx;[https] //for developing nodejs
  ```
- ### access
 
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

- ufw & systemd& service
```
   ufw allow 22;
```
 - other 
 ```c#
  sudo apt install npm/curl;
  rz //upload to server
  sz //download 
 ```
 