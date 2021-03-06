/*
 * Copyright (c) 2007,2008,2009,2010 Mij <mij@sshguard.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * SSHGuard. See http://www.sshguard.net
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include "sshguard.h"
#include "sshguard_logsuck.h"
#include "sshguard_options.h"
#include "sshguard_procauth.h"
#include "sshguard_whitelist.h"

sshg_opts opts;

static void usage(void) {
    fprintf(stderr, "usage: sshguard [-v] [-a thresh] [-b thresh:file] [-e script]\n"
                    "\t\t[-f service:pid-file] [-i pidfile] [-l source] [-p interval]\n"
                    "\t\t[-s interval] [-w address | file]\n");
}

static void version(void) {
    fprintf(stderr, PACKAGE_STRING "\n");
}

int get_options_cmdline(int argc, char *argv[]) {
    struct stat event_script_buf;
    int status;
    int optch;

    opts.blacklist_filename = NULL;
    opts.my_pidfile = NULL;
    opts.blacklist_threshold = DEFAULT_BLACKLIST_THRESHOLD;
    opts.pardon_threshold = DEFAULT_PARDON_THRESHOLD;
    opts.stale_threshold = DEFAULT_STALE_THRESHOLD;
    opts.abuse_threshold = DEFAULT_ABUSE_THRESHOLD;
    opts.has_polled_files = 0;
    while ((optch = getopt(argc, argv, "b:p:s:a:w:f:l:i:e:vh")) != -1) {
        switch (optch) {
            case 'b':   /* threshold for blacklisting (num abuses >= this implies permanent block */
                opts.blacklist_filename = (char *)malloc(strlen(optarg)+1);
                if (sscanf(optarg, "%u:%s", & opts.blacklist_threshold, opts.blacklist_filename) != 2) {
                    /* argument contains only the blacklist filename */
                    strcpy(opts.blacklist_filename, optarg);
                }
                break;

            case 'p':   /* pardon threshold interval */
                opts.pardon_threshold = strtol(optarg, (char **)NULL, 10);
                if (opts.pardon_threshold < 1) {
                    fprintf(stderr, "Doesn't make sense to have a pardon time lower than 1 second. Terminating.\n");
                    usage();
                    return -1;
                }
                break;

            case 's':   /* stale threshold interval */
                opts.stale_threshold = strtol(optarg, (char **)NULL, 10);
                if (opts.stale_threshold < 1) {
                    fprintf(stderr, "Doesn't make sense to have a stale threshold lower than 1 second. Terminating.\n");
                    usage();
                    return -1;
                }
                break;

            case 'a':   /* abuse threshold count */
                opts.abuse_threshold = strtol(optarg, (char **)NULL, 10);
                if (opts.abuse_threshold < DEFAULT_ATTACKS_DANGEROUSNESS) {
                    fprintf(stderr,
                            "Abuse threshold should be greater than one attack (%d danger)\n",
                            DEFAULT_ATTACKS_DANGEROUSNESS);
                    return -1;
                }

                if (opts.abuse_threshold % DEFAULT_ATTACKS_DANGEROUSNESS != 0) {
                    fprintf(stderr,
                            "Warning: abuse threshold should be a multiple of %d\n",
                            DEFAULT_ATTACKS_DANGEROUSNESS);
                }
                break;

            case 'w':   /* whitelist entries */
                if (optarg[0] == '/' || optarg[0] == '.') {
                    /* add from file */
                    if (whitelist_file(optarg) != 0) {
                        fprintf(stderr, "Could not handle whitelisting for %s.\n", optarg);
                        usage();
                        return -1;
                    }
                } else {
                    /* add raw content */
                    if (whitelist_add(optarg) != 0) {
                        fprintf(stderr, "Could not handle whitelisting for %s.\n", optarg);
                        usage();
                        return -1;
                    }
                }
                break;

            case 'f':   /* process pid authorization */
                if (procauth_addprocess(optarg) != 0) {
                    fprintf(stderr, "Could not parse service pid configuration '%s'.\n", optarg);
                    usage();
                    return -1;
                }
                break;

            case 'l':   /* add source for log sucker */
                if (! opts.has_polled_files) {
                    logsuck_init();
                }
                if (logsuck_add_logsource(optarg) != 0) {
                    fprintf(stderr, "Unable to poll from '%s'!\n", optarg);
                    return -1;
                }
                opts.has_polled_files = 1;
                break;

            case 'i':   /* specify pidfile for my PID */
                opts.my_pidfile = optarg;
                break;

            case 'e':     /* provide a script executed each time a firewall
                           event is risen */
                status = stat(optarg, &event_script_buf);
                /* check the existence of the file */
                if (status == 0)
                    setenv("SSHGUARD_EVENT_EXECUTE", optarg, 1);
                else
                    fprintf(stderr, "Unable to access file %s. Ignoring hook.\n", optarg);
                break;

            case 'v':     /* version */
                version();
                return -1;

            case 'h':   /* help */
            default:    /* or anything else: print help */
                usage();
                return -1;
        }
    }

    if (opts.blacklist_threshold < opts.abuse_threshold) {
        fprintf(stderr, "error: blacklist (%u) is less than abuse threshold (%u)\n",
                opts.blacklist_threshold, opts.abuse_threshold);
        return -1;
    }

    return 0;
}
