
#ifndef __INTERRUPT_HPP_INCLUDED__
#define __INTERRUPT_HPP_INCLUDED__

#include <signal.h>

static int s_interrupted = 0;

static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, 0);
    sigaction (SIGTERM, &action, 0);
}

#endif // __INTERRUPT_HPP_INCLUDED__


