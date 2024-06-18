#
#   hdrdoc - attempt to record function documentation straight from the header files.
#
my %db = ();

$/ = /^(\w+)\s*(__fastcall__)?\s*(\w+)\s*\((.*)\);/;

foreach my $headerfile (<../include/*.h>)
{
	open my $fp, '<', $headerfile;
	my @data = <$fp>;
	close $fp;

    my ($header) = $headerfile =~ /(\w+.h)$/;

	foreach my $line (@data)
	{
		next if $line =~ /^typedef/;
		next unless $line =~ /^(signed|unsigned)\s*?(\w+)\s*(__fastcall__)?\s*(\w+)\s*\((.*)\);/;
		my ($sign, $returntype, $fastcall, $name, $args) = ($1, $2, $3, $4);

        $line =~ s/#if .*?#endif//sg;
		$line =~ s/#define .*//;

        #my ($line, @junk) = split /\b#if /;

		my ($call, $doc) = split /\n/, $line, 2;
		#print STDERR "$returntype, $fastcall, $name, $args\n$doc\n\n";

		print STDERR "$name ($header)\n";

        $returntype = "$sign $returntype" if $sign;

    	$doc =~ s/\/\*//;
	    $doc =~ s/\*\///;
	    $doc =~ s/\*\*//g;

		$db{ "$name ($header)" } = {
			name 	=> $name,
			include => $header,
			fastcall => $fastcall,
			returntype => $returntype,
			args		=> $args,
			doc        => $doc,
			call		=> "$returntype $fastcall $name($args);"
		};
	}
}

my @out = ();
foreach my $key (sort keys %db)
{
	push @out, $db{$key};
}

print "<html><body>\n";

print "<table><tr>\n";

my $i = 0;

foreach my $rec (@out)
{
	my %rec = %$rec;
	print "<td><font size=-1><a href=#$rec{name}>$rec{name}</a></font></td>";
	$i++;
	if ($i == 5)
	{
		$i = 0;
		print "</tr><tr>\n";
	}
}

print "</tr></table>\n";
print "<br/><br/><pre>\n";

foreach my $rec (@out)
{
	my %rec = %$rec;
	print<<EOENTRY;
<hr/>
<a name=$rec{name}></a>
<table width=100%>
<tr><td>$rec{name}</td><td align=right>$rec{name}</td></tr>
<tr><td>$rec{include}</td><td align=right>$rec{include}</td></tr>
<tr height=10px/>
<tr><td>$rec{call}</td><td align=right>[ <a href=#top>top</a> ]</rd></tr>
</table>
$rec{doc}

EOENTRY
}

print "</pre></body></html>\n";