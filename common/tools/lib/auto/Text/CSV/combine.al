# NOTE: Derived from blib\lib\Text\CSV.pm.
# Changes made here will be lost when autosplit again.
# See AutoSplit.pm.
package Text::CSV;

#line 125 "blib\lib\Text\CSV.pm (autosplit into blib\lib\auto/Text\CSV/combine.al)"
################################################################################
# combine
#
#    object method returning success or failure.  the given arguments are
#    combined into a single comma-separated value.  failure can be the result of
#    no arguments or an argument containing an invalid character.   side-effects
#    include:
#      setting status()
#      setting fields()
#      setting string()
#      setting error_input()
################################################################################
sub combine {
  my $self = shift;
  my @part = @_;
  $self->{'_FIELDS'} = \@part;
  $self->{'_ERROR_INPUT'} = undef;
  $self->{'_STATUS'} = 0;
  $self->{'_STRING'} = '';
  my $column = '';
  my $combination = '';
  my $skip_comma = 1;
  if ($#part >= 0) {

    # at least one argument was given for "combining"...
    for $column (@part) {
      if ($column =~ /[^\t\040-\176]/) {

	# an argument contained an invalid character...
	$self->{'_ERROR_INPUT'} = $column;
	return $self->{'_STATUS'};
      }
      if ($skip_comma) {

	# do not put a comma before the first argument...
	$skip_comma = 0;
      } else {

	# do put a comma before all arguments except the first argument...
	$combination .= ',';
      }
      $column =~ s/\042/\042\042/go;
      $combination .= "\042";
      $combination .= $column;
      $combination .= "\042";
    }
    $self->{'_STRING'} = $combination;
    $self->{'_STATUS'} = 1;
  }
  return $self->{'_STATUS'};
}

# end of Text::CSV::combine
1;
