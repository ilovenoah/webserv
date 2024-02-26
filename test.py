import os
import socket
import subprocess
import time
import difflib

WEBSERV_FILE_NAME = 'webserv'
WEBSERV_FILE_PATH = './' + WEBSERV_FILE_NAME
TESTDIR_PATH = './tests'
UPLOAD_STORE_PATH = TESTDIR_PATH + '/upload'
DELETE_DIR_PATH = TESTDIR_PATH + '/delete'
DELETE_FILE_PATH = DELETE_DIR_PATH + '/test.html'
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

def get_body(sections):
	if len(sections) > 1: request_section = sections[1]
	else: request_section = ''
	splited_request = request_section.split(' ')
	if len(splited_request) > 1: return splited_request[1]
	return ''
	

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
	if (2 > len(sections)): return None
	return sections[1].split(' ')[0]

def get_response_status(request_data):
	return request_data.split(' ')[1]

def get_response_status_message(request_data):
	return request_data.split(' ')[2]

def	 print_diff(response_act, response_exp):
	for line in difflib.unified_diff(
			response_act.split(CRLF), response_exp.split(CRLF), lineterm=''):
		print(line)

def init_delete_file():
	f = open(DELETE_FILE_PATH, 'x')
	f.close()

def assert_str(act, exp):
	if (act != exp):
		print(RED + '===== KO =====' + END)
		print_diff(act, exp)
	else: print(GREEN + '===== OK =====' + END)

def assert_post(exp_status, sections):
	status = int(exp_status)
	if (status >= 400 and status < 600):
		file_path = [os.path.join(UPLOAD_STORE_PATH, path) for path in os.listdir(UPLOAD_STORE_PATH)][0]
		if (os.path.exists(file_path) == True): print(RED + '===== KO =====' + END)
		else: print(GREEN + '===== OK =====' + END)

	else:
		file_path = [os.path.join(UPLOAD_STORE_PATH, path) for path in os.listdir(UPLOAD_STORE_PATH)][0]
		act = get_body(sections)
		exp = get_file_content(file_path)
		assert_str(act, exp)
	if os.path.exists(file_path): os.remove(file_path)

def assert_delete(exp_status):
	status = int(exp_status)
	
	if (status >= 400 and status < 600):
		if (os.path.exists(DELETE_FILE_PATH) == False): print(GREEN + '===== OK =====' + END)
		else: print(RED + '===== KO =====' + END)
		if os.path.exists(DELETE_FILE_PATH): os.remove(DELETE_FILE_PATH)

	else:
		if (os.path.exists(DELETE_FILE_PATH) == True): print(RED + '===== KO =====' + END)
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
			if (method == 'DELETE'): init_delete_file()
			response_act = send_raw_data(host, port, request_data)
			response_exp = get_file_content(os.path.join(testdir, RESPONSE_FILEDIR_NAME, response_file_path))
			exp_status = get_response_status(response_exp)
			print()
			print('File name: \"%s\"' % (response_file_path))
			assert_str(response_act, response_exp)
			if (method == 'POST'): assert_post(exp_status, sections)
			elif (method == 'DELETE'): assert_delete(exp_status)

		process.terminate()
		process.wait()

if __name__ == '__main__':
	main()