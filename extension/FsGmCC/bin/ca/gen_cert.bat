@echo off

rem call cmd
set OPENSSL_CONF=openssl.cnf
mkdir certs

rem 生成根密钥
openssl genrsa -out ca.key 1024
rem 生成根证书(自签证书)
openssl req -new -x509 -key ca.key -out ca.crt

rem 生成gmcc服务证书
openssl genrsa -out certs/gmcc.key 1024
rem 生成gmcc证书签署请求
openssl req -new -key certs/gmcc.key -out certs/gmcc.csr 

rem 接下来要把上一步生成的证书请求csr文件，发到CA服务器上，在CA上执行
openssl ca -in certs/gmcc.csr -out certs/gmcc.crt

pause
@echo on