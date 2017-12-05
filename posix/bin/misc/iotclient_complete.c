/*
 * An example of a directly connected device which is capable of communicating
 * directly with Oracle IoT Cloud Service. This sample illustrates
 * C code for sending data to the cloud service and does not fully explore the Client Library API.
 *
 * The sample uses the virtualization API. It presents a simple messenger.
 *
 * Device implements a device model. A device model is a set of related
 * attributes, actions, and message formats that can be represented in
 * a real device. For this example the "Hello World" device model is used.
 * This device model must be uploaded to the server before running this example.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "pi_2_dht_read.h"

/* include common public types */
#include "iotcs.h"
/* include iot cs device model APIs */
#include "iotcs_virtual_device.h"
/* include methods for device client*/
#include "iotcs_device.h"

/* Current device value */
static const char* device_current_value;
/* Device model handle */
static iotcs_device_model_handle device_model_handle = NULL;
/* Device handle */
static iotcs_virtual_device_handle device_handle = NULL;

/* print error message and terminate the program execution */
static void error(const char* message) {
    fprintf(stderr,"iotcs: Error occurred: %s\n", message);
    exit(EXIT_FAILURE);
}

/*
** Define const Variables
*/
// Set sensor type DHT11=11, DHT22=22
const int sensor_type = 22;
// The sensor is on GPIO pin=4
const int gpio_pin = 4;
// This is the URN of your device model
const char* device_urns[] = {
  "urn:com:oracle:demo:esensor",
  NULL
};

int main(int argc, char** argv) {
  const char* ts_path = argv[1];
  const char* ts_password = argv[2];
  const char* ts_startmode = argv[3];

  if (argc < 3) {
    error("Too few parameters.\n"
              "\nUsage:"
              "\n\tdirectly_connected_device.out path password"
              "\n\tpath is a path to trusted assets store."
              "\n\tpassword is a password for trusted assets store.");
  }

	/*
	** Define Variables
	*/
  iotcs_result rv;
	// Set sensor type DHT11=11, DHT22=22, GPIO pin=4
	const int sensor_type = 22;
	const int gpio_pin = 4;
	// Startup delay to allow network to initialize
	const int startup_delay=30;
	// Number of retries when the sensor gives bad data
	const int retries=3;
	// Time (secs) before trying to read the sensor again
	const int retry_timer = 10;
	// Read interval in secs
	const int read_interval = 300;
	const int read_interval_testing = 10; // For testing

	fprintf(stderr,"iotcs: device starting!\n");
	fprintf(stderr,"iotcs: Loading configuration from: %s\n" ,ts_path);

	/*
	 * PK: During prod startup wait startup_delay secs for all services to startup before trying to init IOT
	*/
	if (argc > 3) {
		if (strcmp (ts_startmode, "test") == 0) {
			fprintf(stderr,"iotcs: startmode=test\n");
		} else {
			// Wait for network services to start
			fprintf(stderr,"iotcs: Wait for network services to start\n");
			sleep(startup_delay);
		}
	} else {
		// Wait for network services to start
		fprintf(stderr,"iotcs: Wait for network services to start\n");
		sleep(startup_delay);
	}

  /*
   * Initialize the library before any other calls.
   * Initiate all subsystems like ssl, TAM, request dispatcher,
   * async message dispatcher, etc which needed for correct library work.
   */

  if (iotcs_init(ts_path, ts_password) != IOTCS_RESULT_OK) {
    error("Initialization failed");
  }

  /*
   * Activate the device, if it's not already activated.
   * Always check if the device is activated before calling activate.
   * The device model URN is passed into the activate call to tell
   * the server the device model(s) that are supported by this
   * directly connected device
   */

  if (!iotcs_is_activated()) {
    if (iotcs_activate(device_urns) != IOTCS_RESULT_OK) {
      error("Sending activation request failed");
    }
  }

  /* get device model handle */
  if (iotcs_get_device_model_handle(device_urns[0], &device_model_handle) != IOTCS_RESULT_OK) {
    fprintf(stderr,"iotcs_get_device_model_handle method failed\n");
    return IOTCS_RESULT_FAIL;
  }

  /* get device handle */
  if (iotcs_get_virtual_device_handle(iotcs_get_endpoint_id(), device_model_handle, &device_handle) != IOTCS_RESULT_OK) {
    fprintf(stderr,"iotcs_get_device_handle method failed\n");
    return IOTCS_RESULT_FAIL;
  }

	/* Init vars for main loop */
	int i = 0;
	int result;
	float humidity, temperature;

  /* Main loop - Read the sensor and send message to IOT */
	while(1)
	{
		int ix=0;
		time_t mytime;

		humidity = 0;
		temperature = 0;
		result = -1;

		// PK: Read values from the sensor. Retry on bad data
		while ((result != DHT_SUCCESS) && (ix < retries)) {
			fprintf(stderr,"iotcs: Reading from the DHT%u sensor!\n", sensor_type);
			result = pi_2_dht_read(sensor_type, gpio_pin, &humidity, &temperature);
			if (result != DHT_SUCCESS) {
				fprintf(stderr,"iotcs: Warning, Bad data from the DHT%u sensor, trying again %u/%u times.\n", sensor_type, ix+1, retries);

				ix++;

				if (ix == retries) {
					fprintf(stderr,"iotcs: Warning, failed to read %u times from the DHT%u sensor, skipping to next cycle!\n", retries, sensor_type);
				} else {
					// wait for sensor for "retry_timer" secs
					sleep (retry_timer);
				}
			}
		}

		// Only report successful sensor readings
		if (result == DHT_SUCCESS) {

			mytime = time(NULL);
			printf(ctime(&mytime));

			// PK: print what we report to IOT
			fprintf(stderr,"\n<*******************************************************************>\n");
			fprintf(stderr, ctime(&mytime));
			fprintf(stderr,"iotcs: result = %u, humidity = %2.2f, temperature= %2.2f\n", result, humidity, temperature);
			fprintf(stderr,"<*******************************************************************>\n\n");

			// PK: Start setting attribute for IOT
			iotcs_virtual_device_start_update(device_handle);

			// PK: Set attribute
			rv = iotcs_virtual_device_set_float(device_handle, "temperature", temperature);
			if (rv != IOTCS_RESULT_OK) {
				fprintf(stderr,"iotcs_virtual_device_set_float method 1 failed\n");
				return IOTCS_RESULT_FAIL;
			}

			// PK: Set attribute
			rv = iotcs_virtual_device_set_float(device_handle, "humidity", humidity);
			if (rv != IOTCS_RESULT_OK) {
				fprintf(stderr,"iotcs_virtual_device_set_float method 2 failed\n");
				return IOTCS_RESULT_FAIL;
			}

			// PK: We are done. Send message to IOT
			iotcs_virtual_device_finish_update(device_handle);

		}

		// PK: How long to sleep before next sensor reading
		if (argc > 3) {
			if (strcmp (ts_startmode, "test") == 0) {
				// Startmode = test mode
				fprintf(stderr,"iotcs: Sleeping %u secs, startmode=test\n", read_interval_testing);
				sleep(read_interval_testing);
			} else {
				// Startmode = production mode
				fprintf(stderr,"iotcs: Sleeping %u secs, startmode=prod\n", read_interval);
				sleep(read_interval);
			}
		} else {
			// Startmode = production mode
			fprintf(stderr,"iotcs: Sleeping %u secs, startmode=prod\n", read_interval);
			sleep(read_interval);
		}
	}

    /* get current device value
    if (iotcs_virtual_device_get_string(device_handle, "message", &device_current_value) != IOTCS_RESULT_OK) {
        fprintf(stderr,"iotcs_virtual_device_get_string method failed\n");
        return IOTCS_RESULT_FAIL;
    }

    fprintf(stderr,"Message is %s\n", device_current_value);
 */
    /* free device handle */
    iotcs_free_virtual_device_handle(device_handle);
    /* free device model handle */
    iotcs_free_device_model_handle(device_model_handle);

    /*
     * Calling finalization of the library ensures communications channels are closed,
     * previously allocated temporary resources are released.
     */
    iotcs_finalize();
    printf("OK\n");
    return EXIT_SUCCESS;
}
