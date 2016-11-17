#!/usr/bin/perl -w
#
#  Main authors:
#     Christian Schulte <schulte@gecode.org>
#
#  Copyright:
#     Christian Schulte, 2009
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

# Shortens labels and references to avoid problems with the generated PDF
# hyperlinks

while ($l = <>) {
  if ($l =~ /(.*)\\litfile\{(.+)\}\{(.+)\}\{(.+)\}(.*)/) {
    print "$1\\litfile\{".relabel($2)."\}\{$3\}\{$4\}$5";
  } elsif ($l =~ /(.*)\\litlabel\{(.+)\}\{(.+)\}(.*)/) {
    print "$1\\litlabel\{".relabel($2)."\}\{$3\}$4";
  } elsif ($l =~ /(.*)\\litref\{(.+)\}\{(.+)\}(.*)/) {
    print "$1\\litref\{".relabel($2)."\}\{$3\}$4";
  } elsif ($l =~ /(.*)\\hyperref\[(.+)\]\{\\autoref\*\{(.+)\}(.+)\}(.*)/) {
    print "$1\\hyperref[".relabel($2)."]\{\\autoref*\{".relabel($3)."\}$4\}$5";
  } elsif ($l =~ /\\autoref/) {
    $o = "";
    while ($l =~ /(.*)\\autoref\{([^\}]+)\}(.*)/) {
      $l = $1;
      $o = "\\autoref\{".relabel($2)."\}".$3.$o;
    }
    print "$l$o\n";
  } elsif ($l =~ /\\label/) {
    $o = "";
    while ($l =~ /(.*)\\label\{([^\}]+)\}(.*)/) {
      $l = $1;
      $o = "\\label\{".relabel($2)."\}".$3.$o;
    }
    print "$l$o\n";
  } else {
    print $l;
  }
}

sub relabel {
  my $l = $_[0];
  if (!$label{$l}) {
    $ln++;
    $label{$l} = "l:$ln";
  }
  return $label{$l};
}
