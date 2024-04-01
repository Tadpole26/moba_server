for %%i in (*.proto) do (
"protoc.exe" -I ./ --cpp_out ../../Libs/protocol %%i
"protoc.exe" -I ./ --python_out ../../Tools/client %%i
)
pause