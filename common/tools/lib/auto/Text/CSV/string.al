# NOTE: Derived from blib\lib\Text\CSV.pm.
# Changes made here will be lost when autosplit again.
# See AutoSplit.pm.
package Text::CSV;

#line 93 "blib\lib\Text\CSV.pm (autosplit into blib\lib\auto/Text\CSV/string.al)"
################################################################################
# string
#
#    object method returning the result of the most recent combine() or the
#    input to the most recent parse(), whichever is more recent.  there are no
#    side-effects.
################################################################################
sub string {
  my $self = shift;
  return $self->{'_STRING'};
}

# end of Text::CSV::string
1;
