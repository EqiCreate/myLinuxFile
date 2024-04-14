## 1.venv
```shell
python3 -m venv --system-site-packages ./venv37
source ./venv37/bin/activate  # sh, bash, or zsh
```

### 2.setting in vscode
```
 (ctrl+shift+p)setting->python interperpor   #set interpretor
 settings->runner->python-u => python3 -u # set code runner
```
```json
#debug with args
{
    // 欲了解更多信息，请访问: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Python: 当前文件",
            "type": "python",
            "request": "launch",
            "program": "${file}",
            "console": "integratedTerminal",
            "justMyCode": true, 
            "args": ["train"]
        }
    ]
}
```

### 3.setting in environment
```py
pip install pysocks #install pysocks without proxy
```
```bash
echo 'export http_proxy="socks5://localhost:1090"' >> ~/.bashrc
echo 'export https_proxy="socks5://localhost:1090"' >> ~/.bashrc
source ~/.bashrc
```