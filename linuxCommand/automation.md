# Status
+ ## Sytem
    + systemctl status
    + systemctl list-units --type=service (--state=runnint)
    + systemctl [xxx] start|restart
    + systemctl enable/disable [xxx] : to set up startup services|apps
      + service --status-all : check service status
    + ufw 
      + sudo ufw status
      + sudo ufw enable
      + udo ufw default allow outgoing
      + sudo ufw allow 22/tcp
+ ## Application
 + ## vscode
    + <mark> proxy set for http & https
    ```json
    cappsettings.json --ip</mark>
    {
            "server":"服务器ip地址",
            "server_port":12345,
            "local_port":1080,
            "password":"12345",
            "timeout":600,
            "method":"aes-256-cfb"
    }
    ```
+ ## Net
    + ``netstat -ntlp | grep 22``
    + ``curl cip.cc // look at location ip``
    + ~~-A INPUT -s 10.0.0.36/32 -p tcp -m multiport --dports 22,80,443,8080,7231 -j ACCEPT~~
    + ~~firewall-cmd --permanent --add-rich-rule="rule family="ipv4" source address=" 10.0.0.36" port protocol="tcp" port="7231" accept"~~
    + <mark>ufw status;ufw allow xxx ;ufw enable/disable;</mark>

+ ## disk
  + check the space used 
  ```shell
   du -h --max-depth=1 . | sort -hr # check space in pwd
   rm -rf xxx #delete folder xxx
  ```

