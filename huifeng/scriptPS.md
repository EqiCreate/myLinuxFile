### 搜索
- dir - recurse -include "*.dll"
- get-childitem .\ -filter *exe -recurse | findstr "MSBuild"

### xsd
- ./xsd.exe xxx.xsd /o folderpath /nologo

### 文件操作
- compare-object (get-content 1.xsd) (get-content 2.xsd)
- (get-acl .).access #show folder access
- cat > xxx.txt # touch
  
### 配置beyondcompare
- git config --global difftool.bc4.cmd \"BCoompare.exe\" \"$LOCAL\" \"$REMOTE\"
- git config --global mergetool.bc4.cmd '"xxx/BCompare.exe "$LOCAL" "$REMOTE" "$BASE" "$MERGED""'

### management
- rundll32.exe user32.dll LockWorkStation #锁屏
  
### Trick
- $xxx | get-member # 获取所有成员
  - $xxx | get-member -property m1,m2 # 获取某些成员
- findstr -i 'xxx' # 忽略大小写
- new object # 新对象
- 引用静态对象用方括号
  - i.e. [System.Environment]::Commandline
- WMI
  - get-CimINstance -Class Win32_OperatingSystem
- Com
- $ie=new-object -comobject InternetExplorer.Application
- $json = get-content 1.txt -encoding uft8 | convert-json #转换为json 赋值给对象

### performance

### monitor
- get-process -name 'EXCEL' | stop-process -f
  
### API
- Invoke-WebRequest -Uri http://xxxxx -Method 
