# NOTE: Derived from blib\lib\Text\CSV.pm.
# Changes made here will be lost when autosplit again.
# See AutoSplit.pm.
package Text::CSV;

#line 231 "blib\lib\Text\CSV.pm (autosplit into blib\lib\auto/Text\CSV/_bite.al)"
################################################################################
# _bite
#
#    *private* class/object method returning success or failure.  the arguments
#    are:
#      - a reference to a comma-separated value string
#      - a reference to a return string
#      - a reference to a return boolean
#    upon success the first comma-separated value of the csv string is
#    transferred to the return string and the boolean is set to true if a comma
#    followed that value.  in other words, "bite" one value off of csv
#    returning the remaining string, the "piece" bitten, and if there's any
#    more.  failure can be the result of the csv string containing an invalid
#    sequence of characters.
#   
#    from the csv string and 
#    to be a valid comma-separated value.  failure can be the result of
#    no arguments or an argument containing an invalid sequence of characters.
#    side-effects include:
#      setting status()
#      setting fields()
#      setting string()
#      setting error_input()
################################################################################
sub _bite {
  my ($self, $line_ref, $piece_ref, $bite_again_ref) = @_;
  my $in_quotes = 0;
  my $ok = 0;
  $$piece_ref = '';
  $$bite_again_ref = 0;
  while (1) {
    if (length($$line_ref) < 1) {

      # end of string...
      if ($in_quotes) {

	# end of string, missing closing double-quote...
	last;
      } else {

	# proper end of string...
	$ok = 1;
	last;
      }
    } elsif ($$line_ref =~ /^\042/) {

      # double-quote...
      if ($in_quotes) {
	if (length($$line_ref) == 1) {

	  # closing double-quote at end of string...
	  substr($$line_ref, 0, 1) = '';
	  $ok = 1;
	  last;
	} elsif ($$line_ref =~ /^\042\042/) {

	  # an embedded double-quote...
	  $$piece_ref .= "\042";
	  substr($$line_ref, 0, 2) = '';
	} elsif ($$line_ref =~ /^\042,/) {

	  # closing double-quote followed by a comma...
	  substr($$line_ref, 0, 2) = '';
	  $$bite_again_ref = 1;
	  $ok = 1;
	  last;
	} else {

	  # double-quote, followed by undesirable character (bad character sequence)...
	  last;
	}
      } else {
	if (length($$piece_ref) < 1) {

	  # starting double-quote at beginning of string
	  $in_quotes = 1;
	  substr($$line_ref, 0, 1) = '';
	} else {

	  # double-quote, outside of double-quotes (bad character sequence)...
	  last;
	}
      }
    } elsif ($$line_ref =~ /^,/) {

      # comma...
      if ($in_quotes) {

	# a comma, inside double-quotes...
	$$piece_ref .= substr($$line_ref, 0 ,1);
	substr($$line_ref, 0, 1) = '';
      } else {

	# a comma, which separates values...
	substr($$line_ref, 0, 1) = '';
	$$bite_again_ref = 1;
	$ok = 1;
	last;
      }
    } elsif ($$line_ref =~ /^[\t\040-\176]/) {

      # a tab, space, or printable...
      $$piece_ref .= substr($$line_ref, 0 ,1);
      substr($$line_ref, 0, 1) = '';
    } else {

      # an undesirable character...
      last;
    }
  }
  return $ok;
}

1;
# end of Text::CSV::_bite
