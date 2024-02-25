import os
import socket
import subprocess
import time
import difflib

WEBSERV_FILE_NAME = 'webserv'
WEBSERV_FILE_PATH = './' + WEBSERV_FILE_NAME
TESTDIR_PATH = './tests'
CONFIG_FILEDIR_NAME = 'config'
CONFIG_FILE_NAME = 'default.conf'
REQUEST_FILEDIR_NAME = 'request_file'
RESPONSE_FILEDIR_NAME = 'response_file'
CRLF = '\r\n'

RED = '\033[31m'
GREEN = '\033[32m'
END = '\033[0m'

def get_file_content(file_path):
	with open(file_path, 'rb') as f:
		raw_data = f.read().decode('ascii')
	return raw_data

def get_section_list(file_content):
	return file_content.split(CRLF + CRLF, 2)

def get_socket(sections):
	socket_section = sections[0].split(CRLF)

	host = ''
	port = 80
	for line in socket_section:
		if line.startswith('host,'): host = line.split(',', 1)[1].strip()
		elif line.startswith('port,'): port = int(line.split(',', 1)[1].strip())
	return host, port

def get_request_data(sections):
	if len(sections) > 1: request_section = sections[1]
	else: request_section = ''
	request_section += CRLF + CRLF
	return request_section

def send_raw_data(host, port, request_data):
	response = ''
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		s.connect((host, port))
		s.sendall(request_data.encode())
		while True:
			part = s.recv(100000)
			if not part: break
			response += part.decode()
	return response

def get_method(sections):
	return sections[1].split(' ')[0]

def	 print_diff(response_act, response_exp):
	for line in difflib.unified_diff(
			response_act.split(CRLF), response_exp.split(CRLF), lineterm=''):
		print(line)

def assert_response(act, exp):
	if (act != exp):
		print(RED + '===== KO =====' + END)
		print_diff(act, exp)
	else: print(GREEN + '===== OK =====' + END)

def	 main():
	for testdir in [os.path.join(TESTDIR_PATH, path) for path in os.listdir(TESTDIR_PATH)]:
		config_path = os.path.join(testdir, CONFIG_FILEDIR_NAME, CONFIG_FILE_NAME)
		process = subprocess.Popen([WEBSERV_FILE_PATH, config_path],
									stdout=subprocess.PIPE,
									stderr=subprocess.PIPE)
		print('webserv is starting up...')
		time.sleep(0.1)
		for request_file_path, response_file_path in zip(os.listdir(os.path.join(testdir, REQUEST_FILEDIR_NAME)),
										 		os.listdir(os.path.join(testdir, RESPONSE_FILEDIR_NAME))):
			sections = get_section_list(get_file_content(os.path.join(testdir, REQUEST_FILEDIR_NAME, request_file_path)))
			host, port = get_socket(sections)
			request_data = get_request_data(sections)
			method = get_method(sections)
			response_act = send_raw_data(host, port, request_data)
			response_exp = get_file_content(os.path.join(testdir, RESPONSE_FILEDIR_NAME, response_file_path))
			print()
			print('File name: \"%s\"' % (response_file_path))
			assert_response(response_act, response_exp)
			# if (method == 'POST'):
			# elif (method == 'DELETE'):
		process.terminate()
		process.wait()

if __name__ == '__main__':
	main()