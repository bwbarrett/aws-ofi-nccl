/*
 * Copyright (c) 2023 Amazon.com, Inc. or its affiliates. All rights reserved.
 */

#include "config.h"

#include <unistd.h>

#include "stats.h"

int
main(int argc, char *argv[])
{
	histogram<int, histogram_linear_binner<int> > histogram("testing!",
								histogram_linear_binner<int>(0, 2, 5));;

	histogram.insert(0);
	histogram.insert(1);
	histogram.insert(2);
	histogram.insert(3);
	histogram.insert(4);
	histogram.insert(5);
	histogram.insert(6);
	histogram.insert(7);
	histogram.insert(8);
	histogram.insert(9);
	histogram.insert(10);
	histogram.insert(0);

	timer_histogram timer_histogram("timers!", 1000000, 10);

	timer_histogram.start_timer();
	timer_histogram.stop_timer();
	timer_histogram.start_timer();
	sleep(1);
	timer_histogram.stop_timer();
	timer_histogram.start_timer();
	sleep(5);
	timer_histogram.stop_timer();

        return 0;
}
