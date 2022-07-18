import os
import serial
import subprocess

COMPILER_COMMANDS = ['cd arm-compiler',
                     'cp c_file_path demo.c',
                     'make TARGET=stm32f4 demo.bin',
                     'st-flash write build/demo.bin 0x8000000',
                     '''gdb-multiarch -ex 'set remotetimeout 10' -ex 'target remote :4242' -ex load -ex 'c' build/demo.elf''', ]


class BoardConnector:
    '''Allows running C code from python using subprocess library'''

    def __init__(self, c_file_path, tty='/dev/ttyUSB0'):
        self.tty = tty
        self._c_file_path = c_file_path
        COMPILER_COMMANDS[1] = COMPILER_COMMANDS[1].replace('c_file_path', c_file_path)

        self._serial = serial.Serial(self.tty, 115200)
        self._cmd = subprocess.Popen(COMPILER_COMMANDS,
                                     stdout=subprocess.PIPE,
                                     stdin=subprocess.PIPE, shell=True, env=os.environ)
        self._st_util = subprocess.Popen([f'st-util'],
                                         stdout=subprocess.PIPE,
                                         stdin=subprocess.PIPE, shell=True, env=os.environ)

    def execute_cmd(self, command):
        '''Execute command (pass the input to the shell)'''
        self._serial.write(
            (str(command) + '\r').encode('utf-8')
            # convert to strig and add newline (emulate input confirm.) and convert to bytes
        )

    def get_full_output(self):
        '''Return subprocess output'''

        lines = []
        while True:  # loop until all the output is read
            line = self._serial.readline()  # .decode('utf-8')
            if not line:
                break
            lines.append(line.strip())
        return lines

    def get_results(self, result_pattern='result = '):
        '''Return only results from subprocess output'''
        out = self.get_full_output()
        # return only the results (output lines that contain the result_pattern)
        print('OUT', out)
        return [res.split(result_pattern)[-1].strip() for res in out if result_pattern in res]

    def disconnect(self):
        '''Kill the subprocess'''
        self._serial.read()
        self._cmd.kill()
        self._st_util.kill()

    def restart(self):
        '''Restart the C program subprocess'''
        self.disconnect()
        self._cmd = subprocess.Popen(COMPILER_COMMANDS,
                                     stdout=subprocess.PIPE,
                                     stdin=subprocess.PIPE, shell=True, env=os.environ)
        self._st_util = subprocess.Popen([f'st-util'],
                                         stdout=subprocess.PIPE,
                                         stdin=subprocess.PIPE, shell=True, env=os.environ)
