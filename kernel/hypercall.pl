#!/usr/bin/perl -w

# Generate hypercall.S, the stubs for hypercalls.

print "# generated by hypercall.pl - do not edit\n";

print "#include \"../kernel/hypercall.h\"\n";

sub entry {
    my $name = shift;
    print ".global guest_$name\n";
    print "guest_${name}:\n";
    print " li a7, HC_${name}\n";
    print " ecall\n";
    print " ret\n";
}

entry("consolewrite");
entry("consoleread");
entry("memsize");
