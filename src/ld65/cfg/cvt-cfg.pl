#!/usr/bin/perl

# Check number of params
die "Usage: cvt-cfg.pl input output varname\n" unless ($#ARGV == 2);

# Get the parameters
$InputName  = shift (@ARGV);
$OutputName = shift (@ARGV);
$VarName    = shift (@ARGV);

# Open both files
open (IN, "<$InputName") or die "Cannot open $InputName\n";
open (OUT, ">$OutputName") or die "Cannot open $OutputName\n";

# Print the header to the output file
print OUT "static const char $VarName [] = \n";

# Read from input, print to output
while ($Line = <IN>) {

    # Remove the newline
    chomp $Line;

    # Separate an existing comment. No need to be overly clever, just ignore
    # hash marks in strings.
    if ($Line =~ /(.*?)(\s*)(\#\s*)(.*?)\s*$/) {
 	$Line 	      = $1;
 	$CommentSpace = $2;
 	$Comment      = $4;
    } else {
 	$CommentSpace = "";
 	$Comment      = "";
    }

    # Remove leading and trailing spaces
    $Line =~ s/^\s*|\s*$//g;

    # Replace control chars
    $Line =~ s/\\/\\\\/g;
    $Line =~ s/\"/\\\"/g;
    $Line =~ s/\'/\\\'/g;

    # Print to output
    print OUT "\"$Line\"";

    # Add a comment if we have one
    if ($Comment ne "") {
	print OUT "$CommentSpace/* $Comment */";
    }

    # Terminate the line
    print OUT "\n";
}

# Terminate the variable declaration
print OUT ";\n";

# Close the files
close IN;
close OUT;

# Done
exit 0;




