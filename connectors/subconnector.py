import subprocess


class SubConnector:
    '''Allows running C code from python using subprocess library'''

    def __init__(self, c_file_path):
        # make file????
        self._c_file_path = c_file_path  # path to the .c file
        self._cmd = subprocess.Popen([f'cd c_code & make all & ./{c_file_path}'], stdout=subprocess.PIPE,
                                     stdin=subprocess.PIPE, shell=True)

    def execute_cmd(self, command):
        '''Execute command (pass the input to the shell)'''
        self._cmd.stdin.write(
            (str(command) + '\n').encode('utf-8')
            # convert to string and add newline (emulate input confirm.) and convert to bytes
        )

    def get_full_output(self):
        '''Return subprocess output'''
        self._cmd.stdin.flush()  # flush the buffer

        lines = []
        while True:  # loop until all the output is read
            line = self._cmd.stdout.readline().decode('utf-8')
            if not line:
                break
            lines.append(line.strip())
        return lines

    def get_results(self, result_pattern='result = '):
        '''Return only results from subprocess output'''
        out = self.get_full_output()
        print('OUT:', out)
        # return only the results (output lines that contain the result_pattern)
        return [res.split(result_pattern)[-1] for res in out if result_pattern in res]

    def disconnect(self):
        '''Kill the subprocess'''
        self._cmd.kill()

    def restart(self):
        '''Restart the C program subprocess'''
        self.disconnect()
        self._cmd = subprocess.Popen([f'./{self._c_file_path}'], stdout=subprocess.PIPE, stdin=subprocess.PIPE)
