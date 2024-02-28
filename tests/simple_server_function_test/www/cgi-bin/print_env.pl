use strict;
use warnings;
use CGI;

my $q = CGI->new;

# ステータスコード200 OKとContent-Typeをtext/htmlとして出力
print $q->header(-type => 'text/html', -status => '200 OK');

print <<'EOF';
<!DOCTYPE html>
<html>
<head>
    <title>CGI Environment Variables</title>
</head>
<body>
    <h1>Environment Variables</h1>
    <table border="1">
        <tr><th>Variable</th><th>Value</th></tr>
EOF

# foreach my $key (sort keys %ENV) {
#     print "        <tr><td>" . $q->escapeHTML($key) . "</td><td>" . $q->escapeHTML($ENV{$key}) . "</td></tr>\n";
# }

print <<'EOF';
    </table>
</body>
</html>
EOF