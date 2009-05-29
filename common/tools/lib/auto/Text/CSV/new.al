# NOTE: Derived from blib\lib\Text\CSV.pm.
# Changes made here will be lost when autosplit again.
# See AutoSplit.pm.
package Text::CSV;

#line 52 "blib\lib\Text\CSV.pm (autosplit into blib\lib\auto/Text\CSV/new.al)"
################################################################################
# new
#
#    class/object method expecting no arguments and returning a reference to a
#    newly created Text::CSV object.
################################################################################
sub new {
  my $proto = shift;
  my $class = ref($proto) || $proto;
  my $self = {};
  $self->{'_STATUS'} = undef;
  $self->{'_ERROR_INPUT'} = undef;
  $self->{'_STRING'} = undef;
  $self->{'_FIELDS'} = undef;
  bless $self, $class;
  return $self;
}

# end of Text::CSV::new
1;
