# 设置lark.exe进程名称和dump文件保存路径
$LARK_PROCESS_NAME = "lark.exe"
$DUMP_PATH = "C:\DumpFiles\"

# 定义加密密钥，您需要妥善保管这个密钥，确保只有授权的人员可以访问
$ENCRYPTION_KEY = "your_encryption_key_here"

function Generate-Dump {
    # 使用procdump工具生成dump文件
    & procdump.exe -ma -e $LARK_PROCESS_NAME $DUMP_PATH
}

function Encrypt-and-Compress {
    # 获取生成的dump文件路径
    $dumpFilePath = Join-Path $DUMP_PATH "$LARK_PROCESS_NAME.dmp"

    # 加载加密密钥
    $key = $ENCRYPTION_KEY

    # 读取dump文件内容
    $data = Get-Content $dumpFilePath -Encoding Byte

    # 加密文件内容
    $encryptedData = [Convert]::ToBase64String((New-Object Security.Cryptography.RijndaelManaged).CreateEncryptor($key, $key).TransformFinalBlock($data, 0, $data.Length))

    # 保存加密后的文件
    $encryptedFilePath = Join-Path $DUMP_PATH "$LARK_PROCESS_NAME\_encrypted.dmp"
    [System.IO.File]::WriteAllBytes($encryptedFilePath, [Convert]::FromBase64String($encryptedData))

    # 压缩加密后的文件
    $compressedFilePath = Join-Path $DUMP_PATH "$LARK_PROCESS_NAME\_encrypted_compressed.zip"
    Compress-Archive -Path $encryptedFilePath -DestinationPath $compressedFilePath

    # 删除临时文件
    Remove-Item $encryptedFilePath
}

# 主程序
Generate-Dump
Encrypt-and-Compress
