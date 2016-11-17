#!/usr/bin/perl -w
#
#  Main authors:
#     Guido Tack <tack@gecode.org>
#
#  Copyright:
#     Guido Tack, 2008
#
#  Last modified:
#     $Date: 2008-04-28 17:47:23 +0200 (Mo, 28 Apr 2008) $ by $Author: raphael $
#     $Revision: 6797 $
#
#  This file is part of Gecode, the generic constraint
#  development environment:
#     http://www.gecode.org
#
#  Permission is hereby granted, free of charge, to any person obtaining
#  a copy of this software and associated documentation files (the
#  "Software"), to deal in the Software without restriction, including
#  without limitation the rights to use, copy, modify, merge, publish,
#  distribute, sublicense, and/or sell copies of the Software, and to
#  permit persons to whom the Software is furnished to do so, subject to
#  the following conditions:
#
#  The above copyright notice and this permission notice shall be
#  included in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
#  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
#  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#

# Gecode literate programming helper
#
# This script reads a tex file from standard input, replaces inline blocks
# of C++ code by properly formatted code blocks with references between them,
# and writes each source code to a separate file.
#
# Source code is inserted into the latex file in a litcode environment, which 
# takes one argument, the name of the file. Inside that environment, blocks 
# can be defined using a litblock environment, which takes one argument, the 
# name of the block. Blocks can be nested.
#
# There are three special block names:
# - ignore       (do not produce output for this block in latex)
# - anonymous    (produce ... for this block in latex)
# - texonly      (do not include this block in the generated .cc file)
#
# Note that blocks cannot be nested inside texonly blocks.
#
# The indentation of the \end{litblock} determines the indentation of the 
# generated reference.
#
# In the latex source, you can include the source code of blocks using the
# macro \insertlitcode{id}, where id is a hierarchic identifier, constructed 
# as <name of the file>:block:innerblock (if you want to include an 
# "innerblock" of a "block" defined in some file.
#
# You can refer to a certain block (or a file) using the \litref{id}{text} 
# macro, which will output text and make it a hyperlink to id.
#
# Multi-line comments (those between /* and */) are treated like ignore 
# blocks. They are only recognized if they start and end on their own line. 
# Inline comments are kept.
#
# Links to the Gecode reference documentation can be inserted with the
# following macros:
# \gecoderef[type]{text}
# where type can be: class, example, file, group, namespace, page. The Gecode::
# namespace must be omitted.
#
# For group references to work, there has to be a file gl.db in the
# current directory. If the file is not found but the latex source contains
# a group reference, or if an undefined group is referenced, an error is
# reported. The gl.db is generated from the gecode source and a doxygen tag 
# file by:
#   ./gendocref <path to gecode> > gl.db
#

my @keywords = (
"ann", "annotation", "any", "array", "assert", "bool",
"constraint", "else", "endif",
"enum", "float", "function", "if", "in", "include",
"int", "list", "of", "op", "opt", "output", "minimize", "maximize",
"par", "predicate", "record", "set", "solve", "string", "test",
"then", "tuple", "type", "var", "where"
);

my $curcode; my $curcodefile; my $curcodeopen;
my %blocks;
my %blocknames;
my @blockstack;
my $curblock = "";
my $inlitcode = 0;
my $line = 0;
my @document;
my $intexonlyblock = 0;

while ($l = <>) {
  $line++;
  if ($l =~ /\\begin{litcode}\[texonly\]{(.*)}/) {
    if ($inlitcode == 1) {
      die "Found nested \\begin{litcode} in line $line."
    }
    $inlitcode = 1;
    $curcode = $1;
    $curcodeopen = 0;
  } elsif ($l =~ /\\begin{litcode}[^{]/) {
    die "Missing argument to \\begin{litcode} in line $line."
  } elsif ($l =~ /\\begin{litcode}{(.*)}{(.*)}/) {
    if ($inlitcode == 1) {
      die "Found nested \\begin{litcode} in line $line."
    }
    $inlitcode = 1;
    $curcode = $1;
    $fileext = $2;
    $filename = $curcode;
    $filename =~ s| |-|og;
    if (!($filename =~ /.*\..*/)) {
      $filename = $filename . "." . $fileext;
    }
    $download{$curcode} = "$filename";
    $curcodeopen = 1;
    open($curcodefile, ">", "$filename") || die "Cannot open outputfile $filename.";

  } elsif ($l =~ /\\end{litcode}/) {
    if ($inlitcode == 0) {
      die "Found unmatched \\end{litcode} in line $line."
    }
    $inlitcode = 0;
    if ($curcodeopen) {
      close($curcodefile);
    }
    $blocks{$curcode} = $curblock;
    $curblock = "";
    $blocknames{$curcode} = $curcode;
  } elsif ($inlitcode == 0) {
    if ($l =~ /(.*)\\\?([^\?]+)\?(.*)/) {
      while ($l =~ /(.*)\\\?([^\?]+)\?(.*)/) {
	$pre = $1; $mid = $2; $post = $3;
	$l = $pre . "\\CppInline{";
	my @splitline = split(/(\"(?:[^\"\n\\]|(?:\\(?:.|\n)))*\")/, $mid);
	my $isstring = 0;
	foreach my $entry (@splitline) {
	  $entry = quote($entry);
	  if ($isstring == 0) {
	    $isstring = 1;
	    foreach my $kw (@keywords) {
	      $entry =~ s/\b($kw)\b/{\\litkw{$1}}/g;
	    }
	  } else {
	    $isstring = 0;
	    $entry = "{\\litstr{$entry}}";
	  }
	  $entry =~ s/ /\\lits{}/g;
	  $l = $l . $entry;
	}
	$l = $l . "}" . $post;
      }
      $l = "$l\n";
    }
    push(@document, $l);
  } else {
    if ($l =~ /\\begin{litblock}{(.*)}/) {
      if ($1 eq "texonly") {
        $intexonlyblock = 1;
        push(@blockstack, $1);
      } else {
        push(@blockstack, $curblock);
        push(@blockstack, $1);
        $curblock = "";
	$comment = $1;
	if ($curcodeopen && !($comment =~ "anonymous")) {
	  if ($comment =~ /.*\:(.*)/) {
	    $comment = $1;
	  }
	  $l =~ s|\\begin{litblock}.*|\% $comment|;
	  print $curcodefile $l;
	}
      }
    } elsif ($l =~ /\/\*/) {
      push(@blockstack, $curblock);
      push(@blockstack, "ignore");
      $curblock = $l;
      if (($intexonlyblock == 0) && $curcodeopen) {
        print $curcodefile $l;
      }
    } elsif ($l =~ /(\s*)\\end{litblock}/) {
      my $whitespace = $1;
      my $n = pop(@blockstack);
      my $t = $n;
      if ($t =~ /.*\:(.*)/) {
	$t = $1;
      }
      if ($n eq "texonly") {
        $intexonlyblock = 0;
      } else {
        $blocks{$curcode.":".$n} = $curblock;
        $blocknames{$curcode.":".$n} = $t;
        $curblock = pop(@blockstack);
        if ($n eq "anonymous") {
          $curblock .= $whitespace."...\n";
        } elsif (!($n eq "ignore")) {
          $curblock .= 
            $whitespace."\\litref{lit:".$curcode.":".$n."}{$t}\n";
        }
      }
    } elsif ($l =~ /\*\//) {
      my $n = pop(@blockstack);
      if (! ($n eq "ignore")) {
        die "Found block inside multiline comment.";
      }
      $curblock = pop(@blockstack);
      if (($intexonlyblock == 0) && $curcodeopen) {
        print $curcodefile $l;
      }
    } else {
      $curblock .= $l;
      if (($intexonlyblock == 0) && $curcodeopen) {
        print $curcodefile $l;
      }
    }
  }
}

$incode = 0;
$fst    = 0;
foreach my $line (@document) {
  if (($line =~ /\\end{code}/) ||
      ($line =~ /\\end{smallcode}/) ||
      ($line =~ /\\end{cmd}/) ||
      ($line =~ /\\end{smallcmd}/)) {
    $incode = 0; $fst = 0;
    print "}\n"
  } elsif ($incode) {
    if (!$fst) {
      print "\\\\\n";
    } else {
      $fst = 0;
    }
    fontify($line);
  } elsif ($line =~ /\\insertlitcode\[direct\]{(.*)}/) {
    if (! (exists $blocknames{$1})) {
      die "No block $1 defined.";
    }
    outputBlock($1, $blocknames{$1}, $blocks{$1},0,0);
  } elsif ($line =~ /\\insertlitcode{(.*)}/) {
    if (! (exists $blocknames{$1})) {
      die "No block $1 defined.";
    }
    outputBlock($1, $blocknames{$1}, $blocks{$1},1,0);
  } elsif ($line =~ /\\insertsmalllitcode{(.*)}/) {
    if (! (exists $blocknames{$1})) {
      die "No block $1 defined.";
    }
    outputBlock($1, $blocknames{$1}, $blocks{$1},1,1);
  } elsif ($line =~ /\\begin{code}/) {
    print "\\litcodeblock{\%\n";
    $incode = 1; $fst = 1;
  } elsif ($line =~ /\\begin{smallcode}/) {
    print "\\smalllitcodeblock{\%\n";
    $incode = 1; $fst = 1;
  } elsif ($line =~ /\\begin{cmd}/) {
    print "\\litcmdblock{\%\n";
    $incode = 1; $fst = 1;
  } elsif ($line =~ /\\begin{smallcmd}/) {
    print "\\smalllitcmdblock{\%\n";
    $incode = 1; $fst = 1;
  } else {
    print replaceDocRefs($line);
  }
}

sub outputBlock {
  my $blockref = $_[0];
  my $dl = $download{$blockref};
  $blockref =~ s/ /\\lits{}/g;
  my $blockname = $_[1];
  $blockname =~ s/ /\\lits{}/g;
  my $code = $_[2];
  $code =~ /(\s*)/;
  my $withlabel = $_[3];
  my $small = $_[4];
  my $whitespace = length($1);
  my @codelines = split(/\n/,$code);
  if ($small) {
    print "\\smalllitcodeblock{\%\n";
  } else {
    print "\\litcodeblock{\%\n";
  }
  if ($withlabel) {
    if ($dl) {
      print "\\noindent\\litfile{lit:$blockref}{$blockname}{$dl}\\\\\n";
    } else {
      print "\\noindent\\litlabel{lit:$blockref}{$blockname}\\\\\n";
    }
  } else {
    print "\\noindent";
  }
  my $fst = 1;
  foreach $line (@codelines) {
    if ($fst) {
      $fst = 0;
    } else {
      print "\\\\\n";
    }
    if (! ($line =~ /\\litref/)) {
      fontify(substr($line,$whitespace));
    } else {
      $line = substr($line,$whitespace);
      $line =~ s/ /\\lits{}/g;
      print "\\lits\\lits{}".$line;
    }
  }
  print "}\n"
}

sub fontify {

  my $line = $_[0];
  print "\\lits\\lits{}";
  if ($line =~ /^\#([a-zA-Z]+) (.*)/) {
    print "\\litkw{\\#$1}\\lits{}\\litstr{" . quote($2) . "}";
  } elsif ($line =~ /^\#([a-zA-Z]+)/) {
    print "\\litkw{\\#$1}";
  } else {
    my $comment = "";
    if ($line =~ /(.*?)\/\/(.*)/) {
      $line = $1;
      $comment = quote($2);
    }
    my @splitline = split(/(\"(?:[^\"\n\\]|(?:\\(?:.|\n)))*\")/, $line);
    my $isstring = 0;
    foreach my $entry (@splitline) {
      $entry = quote($entry);
      if ($isstring == 0) {
        $isstring = 1;
	foreach my $kw (@keywords) {
          $entry =~ s/\b($kw)\b/{\\litkw{$1}}/g;
        }
      } else {
        $isstring = 0;
        $entry = "{\\litstr{$entry}}";
      }
      $entry =~ s/ /\\lits{}/g;
      print $entry;
    }
    if (! ($comment eq "")) {
      print "{\\litc{\%$comment}}";
    }
  }
}

sub quote {
  my $s = $_[0];
  $s =~ s/\\/BaCkSlAsH/g;
#  $s =~ s/\\/\\\(\\textbackslash\\\)/g;
  $s =~ s/([\${}#&_%])/\\$1/g;
  $s =~ s/BaCkSlAsH/\\textbackslash{}/g;
#  $s =~ s/\\/\\textbackslash{}/g;
  $s =~ s/[~]/\\textasciitilde{}/g;
  $s =~ s/(<)/\\symbol{60}/g;
  $s =~ s/(>)/\\symbol{62}/g;
  $s =~ s/\.\.\./\\litanon/g;
  $s =~ s/--/-{}-/g;
  $s =~ s/--/-{}-/g;
  $s =~ s/\^/\\\^{}/g;
  return $s;
}

sub replaceDocRefs {
  my $l = $_[0];
  chop($l);
  return $l."\n";
}
