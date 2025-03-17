//
// Copyright (c) 2024 Amazon.com, Inc. or its affiliates. All rights reserved.
//

#ifndef NCCL_OFI_STATS
#define NCCL_OFI_STATS

#include <cassert>
#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

template <typename T>
class histogram_linear_binner {
public:
	histogram_linear_binner(const T& min_val_arg, const T& bin_size_arg, std::size_t num_bins_arg)
		: min_val(min_val_arg),bin_size(bin_size_arg), num_bins(num_bins_arg)

	{
	}

	std::size_t get_bin(const T& input_val)
	{
		assert(input_val >= min_val);
		std::size_t bin =  (input_val - min_val) / bin_size;
		if (bin >= num_bins) {
			bin = num_bins - 1;
		}
		return bin;
	}

        std::size_t get_num_bins(void) const
	{
		return num_bins;
	}

private:
	const T min_val;
	const T bin_size;

	const std::size_t num_bins;
};


template <typename T, class Binner>
class histogram {
public:
	histogram(const std::string& description_arg, Binner binner_arg)
		: description(description_arg), binner(binner_arg),
		bins(binner.get_num_bins()), num_samples(0), first_insert(true)
	{
	}

	~histogram()
	{
		print_stats();
	}

	void insert(const T& input_val)
	{
		if (OFI_UNLIKELY(first_insert)) {
			max_val = min_val = input_val;
			first_insert = false;
		}

		if (input_val > max_val) {
			max_val = input_val;
		} else if (input_val < min_val) {
			min_val = input_val;
		}
		bins[binner.get_bin(input_val)]++;
		num_samples++;
	}

	void print_stats(void) {
		printf("%s\n", description.c_str());
		/* TODO: Need better output! */
		printf("  min: %ld, max: %ld, num_samples: %lu\n",
		       (long int)min_val, (long int)max_val, num_samples);
		for (std::size_t i = 0 ; i < bins.size() ; ++i) {
			printf("    %4lu  %8ld\n", i, (long int)bins[i]);
		}
	}

private:
	std::string description;
	Binner binner;
	std::vector<std::size_t> bins;
	T max_val;
	T min_val;
	std::size_t num_samples;
	bool first_insert;
};


template <class T = std::size_t,
	  class Binner = histogram_linear_binner<std::size_t>,
	  class clock = std::chrono::steady_clock
	  >
class timer_histogram : public histogram<T, Binner> {
public:
	using histogram<T, Binner>::insert;

	timer_histogram(const std::string &description_arg, std::size_t bucket_usecs,
			std::size_t num_buckets)
		: histogram<T, Binner>(description_arg, Binner(0, bucket_usecs, num_buckets))
	{
	}

	void start_timer(void)
	{
		start_time = clock::now();
	}

	T stop_timer(void)
	{
		auto now = clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start_time);
		insert(duration.count());
		return duration.count();
	}


private:
	typename clock::time_point start_time;
};

#endif
