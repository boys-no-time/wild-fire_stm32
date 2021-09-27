// Copyright (c) Acconeer AB, 2020-2021
// All rights reserved
// This file is subject to the terms and conditions defined in the file
// 'LICENSES/license_acconeer.txt', (BSD 3-Clause License) which is part
// of this source code package.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "acc_detector_distance.h"
#include "acc_hal_definitions.h"
#include "acc_hal_integration.h"
#include "acc_rss.h"
#include "acc_version.h"

/** \example example_detector_distance.c
 * @brief This is an example on how the distance detector can be used
 * @n
 * The example executes as follows:
 *   - Activate Radar System Software (RSS)
 *   - Create a distance detector configuration
 *   - Create a distance detector using the previously created configuration
 *   - Destroy the distance detector configuration
 *   - Activate the distance detector
 *   - Get the result and print it 5 times
 *   - Deactivate and destroy the distance detector
 *   - Deactivate Radar System Software (RSS)
 */
#define THRESHOLD (100)

static void update_configuration_distance(acc_detector_distance_configuration_t distance_configuration);

static void print_distances(acc_detector_distance_result_t *result, uint16_t reflection_count);


bool acc_example_detector_distance(void);


bool acc_example_detector_distance(void)
{
	printf("Acconeer software version %s\n", acc_version_get());

	const acc_hal_t *hal = acc_hal_integration_get_implementation();

	if (!acc_rss_activate(hal))
	{
		printf("acc_rss_activate() failed\n");
		return false;
	}

	acc_detector_distance_configuration_t distance_configuration = acc_detector_distance_configuration_create();

	if (distance_configuration == NULL)
	{
		printf("acc_detector_distance_configuration_create() failed\n");
		acc_rss_deactivate();
		return false;
	}

	update_configuration_distance(distance_configuration);

//	acc_detector_distance_configuration_requested_start_set(distance_configuration, 0.2);

//	acc_detector_distance_configuration_requested_length_set(distance_configuration, 1);

	acc_detector_distance_handle_t distance_handle = acc_detector_distance_create(distance_configuration);

	if (distance_handle == NULL)
	{
		printf("acc_detector_distance_create() failed\n");
		acc_detector_distance_configuration_destroy(&distance_configuration);
		acc_rss_deactivate();
		return false;
	}

	acc_detector_distance_configuration_destroy(&distance_configuration);

	acc_detector_distance_metadata_t metadata;
	acc_detector_distance_metadata_get(distance_handle, &metadata);

	printf("Start: %u mm\n", (unsigned int)(metadata.start_m * 1000.0f));
	printf("Length: %u mm\n", (unsigned int)(metadata.length_m * 1000.0f));
	printf("Threshold: %u\n", (unsigned int)THRESHOLD);
	printf("\n");


	if (!acc_detector_distance_activate(distance_handle))
	{
		printf("acc_detector_distance_activate() failed\n");
		acc_detector_distance_destroy(&distance_handle);
		acc_rss_deactivate();
		return false;
	}

	bool                                success         = true;
	const int                           iterations      = 5;
	uint16_t                            number_of_peaks = 5;
	acc_detector_distance_result_t      result[number_of_peaks];
	acc_detector_distance_result_info_t result_info;

	for (int i = 0; i < iterations; i++)
	{
		success = acc_detector_distance_get_next(distance_handle, result, number_of_peaks, &result_info);

		if (!success)
		{
			printf("acc_detector_distance_get_next() failed\n");
			break;
		}

		print_distances(result, result_info.number_of_peaks);
	}

	bool deactivated = acc_detector_distance_deactivate(distance_handle);

	acc_detector_distance_destroy(&distance_handle);

	acc_rss_deactivate();

	return deactivated && success;
}

void update_configuration_distance(acc_detector_distance_configuration_t distance_configuration)
{
	float start_m  = 0.2f;
	float length_m = 3.0f;

	acc_detector_distance_configuration_requested_start_set(distance_configuration, start_m);
	acc_detector_distance_configuration_requested_length_set(distance_configuration, length_m);

	acc_detector_distance_configuration_fixed_threshold_set(distance_configuration, THRESHOLD);	//������ֵ

}

static void print_distances(acc_detector_distance_result_t *result, uint16_t reflection_count)
{
	printf("Found %u peaks:\n", (unsigned int)reflection_count);

	for (uint16_t i = 0; i < reflection_count; i++)
	{
		printf("Amplitude %u at %u mm\n", (unsigned int)result[i].amplitude,
		       (unsigned int)(result[i].distance_m * 1000));
	}
}
