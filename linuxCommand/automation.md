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
+ ## General
  + ps aux , kill -9 xxx(pid)
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
    + curl --proxy "socks5://127.0.0.1:1090" "https://baidu.com" -k
    + 重置proxy unset `env | grep -iE "all?_proxy" | cut -d= -f1`
    `unset http_proxy https_proxy all_proxy ALL_PROXY ftp_proxy socks_proxy no_proxy HTTP_PROXY HTTPS_PROXY FTP_PROXY SOCKS_PROXY NO_PROXY`
    + 1 ```
    export no_proxy=localhost,127.0.0.1,::1
export DOTNET_ROOT=$HOME/dotnet
export PATH="/usr/bin:/bin:$HOME/dotnet:$PATH"
# export PATH="$HOME/anaconda3/bin:$PATH"  # commented out by conda initialize
#export PATH=/home/michael/.meteor:$PATH
#export PATH="$HOME/.pyenv/bin:$PATH"
#eval "$(pyenv init -)"
#eval "$(pyenv virtualenv-init -)"

export http_proxy="socks5h://127.0.0.1:1090"
export https_proxy="socks5h://127.0.0.1:1090"
export all_proxy="socks5h://127.0.0.1:1090"
export ALL_PROXY="socks5h://127.0.0.1:1090"

    ```

+ ## disk
  + check the space used 
  ```shell
   sudo du -h --max-depth=1 . | sort -hr # check space in pwd
   rm -rf xxx #delete folder xxx
  ```
  + 删除系统日志 近期
  ```shell
  sudo journalctl --vacuum-time=2weeks
  ```

