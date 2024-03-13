import os
import cgi, cgitb

page_data = '''<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<title> CGI </title>
</head>

<body>
		<form action="post.py" method="POST">
			Username: <input type="text" name="username" />
			
			<br />

			Email Address: <input type="email" name="emailaddress" />

			<input type="submit" value="Submit" />
		</form>
</body>
</html>
	'''

method = os.environ['REQUEST_METHOD']
if method == 'POST':
	form = cgi.FieldStorage()

	username = form["username"].value
	emailaddress = form["emailaddress"].value

	print("Content-type:text/html\r\n\r\n")

	print("<html>")
	print("<head>")
	print("<title> MY FIRST CGI FILE </title>")
	print("</head>")
	print("<body>")
	print("<h3> This is HTML's Body Section </h3>")
	print(username)
	print(emailaddress)
	print("</body>")
	print("</html>")
else:
	print("Status: 200 OK")
	print("Content-Type: text/html")
	print()  # ヘッダーの終わりを示す空行
	print(page_data)