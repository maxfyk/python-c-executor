import subprocess

process = subprocess.Popen('./c_code/calculator.exe', stdout=subprocess.PIPE, stdin=subprocess.PIPE)
process.stdin.write('1\n'.encode('utf-8'))
process.stdin.write('2\n'.encode('utf-8'))

process.stdin.flush()
process.stdin.write('*\n'.encode('utf-8'))
result = process.stdout.read()
print(result)