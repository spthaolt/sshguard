noinst_LIBRARIES = libfwall.a

if FWALL_HOSTS
libfwall_a_SOURCES = hosts.c
else
# FWALL_AIX, FWALL_IPFILTER, FWALL_IPTABLES, FWALL_PF, FWALL_NULL
libfwall_a_SOURCES = command.c
endif
