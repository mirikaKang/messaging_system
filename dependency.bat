@ECHO OFF

cd ..
IF NOT EXIST "./vcpkg/" (
    git clone https://github.com/microsoft/vcpkg.git
)
cd vcpkg
git pull
call bootstrap-vcpkg.bat
vcpkg install lz4:x86-windows lz4:x64-windows lz4:arm64-linux
vcpkg install fmt:x86-windows fmt:x64-windows fmt:arm64-linux 
vcpkg install cpprestsdk:x86-windows cpprestsdk:x64-windows cpprestsdk:arm64-linux
vcpkg install cryptopp:x86-windows cryptopp:x64-windows cryptopp:arm64-linux
vcpkg install asio:x86-windows asio:x64-windows asio:arm64-linux
vcpkg install python3:x86-windows python3:x64-windows python3:arm64-linux
vcpkg install crossguid:x86-windows crossguid:x64-windows crossguid:arm64-linux
vcpkg install libpq:x86-windows libpq:x64-windows libpq:arm64-linux
vcpkg install gtest:x86-windows gtest:x64-windows gtest:arm64-linux
vcpkg upgrade --no-dry-run
cd ..
cd messaging_system
