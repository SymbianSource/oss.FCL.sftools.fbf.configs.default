# NOTE: Derived from blib\lib\Text\CSV.pm.
# Changes made here will be lost when autosplit again.
# See AutoSplit.pm.
package Text::CSV;

#line 105 "blib\lib\Text\CSV.pm (autosplit into blib\lib\auto/Text\CSV/fields.al)"
################################################################################
# fields
#
#    object method returning the result of the most recent parse() or the input
#    to the most recent combine(), whichever is more recent.  there are no
#    side-effects.
################################################################################
sub fields {
  my $self = shift;
  if (ref($self->{'_FIELDS'})) {
    return @{$self->{'_FIELDS'}};
  }
  return undef;
}

# end of Text::CSV::fields
1;
