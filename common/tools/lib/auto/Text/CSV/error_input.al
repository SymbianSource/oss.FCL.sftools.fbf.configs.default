# NOTE: Derived from blib\lib\Text\CSV.pm.
# Changes made here will be lost when autosplit again.
# See AutoSplit.pm.
package Text::CSV;

#line 81 "blib\lib\Text\CSV.pm (autosplit into blib\lib\auto/Text\CSV/error_input.al)"
################################################################################
# error_input
#
#    object method returning the first invalid argument to the most recent
#    combine() or parse().  there are no side-effects.
################################################################################
sub error_input {
  my $self = shift;
  return $self->{'_ERROR_INPUT'};
}

# end of Text::CSV::error_input
1;
