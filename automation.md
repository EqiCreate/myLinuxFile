#/usr/local/gitcode
# Status
+ ## Sytem
    + systemctl status
    + systemctl [xxx] start|restart
    + systemctl enable/disable [xxx] : to set up startup services|apps
      + service --status-all : check service status
+ ## Application
+ ## Net
    + ``netstat -ntlp | grep 22``
    + ``curl cip.cc // look at location ip``
    + ~~-A INPUT -s 10.0.0.36/32 -p tcp -m multiport --dports 22,80,443,8080,7231 -j ACCEPT~~
    + ~~firewall-cmd --permanent --add-rich-rule="rule family="ipv4" source address=" 10.0.0.36" port protocol="tcp" port="7231" accept"~~
    + <mark>ufw status;ufw allow xxx ;ufw enable/disable;</mark>
+ ## vscode
    + <mark> proxy set for http & https
    + cappsettings.json --ip</mark>

{
        "server":"服务器ip地址",
        "server_port":12345,
        "local_port":1080,
        "password":"12345",
        "timeout":600,
        "method":"aes-256-cfb"
}
