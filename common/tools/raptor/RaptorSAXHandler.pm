# Copyright (c) 2009 Symbian Foundation Ltd
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Symbian Foundation Ltd - initial contribution.
#
# Contributors:
#
# Description:
# SAX Handler for the Raptor log

package RaptorSAXHandler;
use base qw(XML::SAX::Base);

sub new
{
    my ($type) = @_;
    
    return bless {}, $type;
}

sub set_init_status
{
	my ($self, $initialstatus) = @_;
	
	$self->{status} = $initialstatus;
}

sub start_document
{
	my ($self, $doc) = @_;
	# process document start event
	
	#print "start_document\n";
}
  
sub start_element
{
	my ($self, $el) = @_;
	# process element start event
	
	my $tagname = $el->{LocalName};
	
	#print "start_element($tagname)\n";
	
	
	if (defined $self->{status}->{next_status}->{$tagname})
	{
		my $oldstatus = $self->{status};
		$self->{status} = $self->{status}->{next_status}->{$tagname};
		#print "status is now $self->{status}->{name}\n";
		$self->{status}->{next_status}->{$tagname} = $oldstatus;
		&{$self->{status}->{on_start}}($el) if (defined $self->{status}->{on_start});
	}
	elsif (defined $self->{status}->{next_status}->{'?default?'})
	{
		#print "changing to default status\n";
		my $oldstatus = $self->{status};
		$self->{status} = $self->{status}->{next_status}->{'?default?'};
		#print "status is now ?default?\n";
		$self->{status}->{next_status}->{$tagname} = $oldstatus;
		&{$self->{status}->{on_start}}($el) if (defined $self->{status}->{on_start});
	}
}

sub end_element
{
	my ($self, $el) = @_;
	# process element start event
	
	my $tagname = $el->{LocalName};
	
	#print "end_element($tagname)\n";
	
	if (defined $self->{status}->{next_status}->{$tagname})
	{
		&{$self->{status}->{on_end}}($el) if (defined $self->{status}->{on_end});
		$self->{status} = $self->{status}->{next_status}->{$tagname};
		#print "status is now $self->{status}->{name}\n";
	}
}

sub characters
{
	my ($self, $ch) = @_;
	
	&{$self->{status}->{on_chars}}($ch) if (defined $self->{status}->{on_chars});
}

1;
