# NOTE: Derived from blib\lib\Text\CSV.pm.
# Changes made here will be lost when autosplit again.
# See AutoSplit.pm.
package Text::CSV;

#line 70 "blib\lib\Text\CSV.pm (autosplit into blib\lib\auto/Text\CSV/status.al)"
################################################################################
# status
#
#    object method returning the success or failure of the most recent combine()
#    or parse().  there are no side-effects.
################################################################################
sub status {
  my $self = shift;
  return $self->{'_STATUS'};
}

# end of Text::CSV::status
1;
