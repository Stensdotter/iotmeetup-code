/*
 * An example of a gateway device which is capable of communicating
 * directly with Oracle IoT Cloud Service. This sample illustrates
 * C code for sending data to the cloud service and does not fully explore the Client Library API.
 *
 * The sample uses the virtualization API. It presents a simple messenger.
 *
 * Device implements a device model. A device model is a set of related
 * attributes, actions, and message formats that can be represented in
 * a real device. For this example the "Hello World" device model is used.
 * This device model must be uploaded to the server before running this example.
 *
 ***********************************************************************************************
 *
 * In this tutorial we are using stderr for output to make it easy to distiguish it
 * from the output to stdout from the IoT Client Library
 * When running the code do "sh run_iotclient.sh 1> log" to only see the output from your app
 *
 * /Peter Karlsson, Oracle Presales
 *
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

/************************************************************************************************
** Main
************************************************************************************************/
int main(int argc, char** argv) {
  const char* ts_path = argv[1];
  const char* ts_password = argv[2];

  if (argc < 3) {
      error("Too few parameters.\n"
              "\nUsage:"
              "\n\tiotclient.out path password"
              "\n\tpath is a path to trusted assets store."
              "\n\tpassword is a password for trusted assets store.");
  }

	// Define Variables
  iotcs_result rv;
	int i = 0;
	int result = -1;
	float humidity = 0;
	float temperature = 0;

	fprintf(stderr,"iotcs: iotclient starting!\n");
	fprintf(stderr,"iotcs: Device Urn: %s\n", device_urns[0]);
	fprintf(stderr,"iotcs: Loading configuration from: %s\n", ts_path);

  /*
   * Initialize the library before any other calls.
   * Initiate all subsystems like ssl, TAM, request dispatcher,
   * async message dispatcher, etc which needed for correct library work.
   */

  sleep (20); // Allow network services to startup

  if (iotcs_init(ts_path, ts_password) != IOTCS_RESULT_OK) {
    error("Initialization failed");
  }

  /*
  * Activate the device, if it's not already activated.
  * Always check if the device is activated before calling activate.
  * The device model URN is passed into the activate call to tell
  * the server the device model(s) that are supported by this
  * device
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

	// Read values from the sensor.
	fprintf(stderr,"iotcs: Reading from the DHT%u sensor!\n", sensor_type);
	result = pi_2_dht_read(sensor_type, gpio_pin, &humidity, &temperature);
	if (result != DHT_SUCCESS) {
		fprintf(stderr,"iotcs: Warning, Bad data from the DHT%u sensor\n", sensor_type);
	}
	fprintf(stderr,"iotcs: temperature = %2.2f, humidity= %2.2f\n", temperature, humidity);

	// Start update "session" with IOT
	iotcs_virtual_device_start_update(device_handle);

	// Set attribute
	rv = iotcs_virtual_device_set_float(device_handle, "temperature", temperature);
	if (rv != IOTCS_RESULT_OK) {
		fprintf(stderr,"iotcs_virtual_device_set_float 1 failed\n");
		return IOTCS_RESULT_FAIL;
	}

	// Set attribute
	rv = iotcs_virtual_device_set_float(device_handle, "humidity", humidity);
	if (rv != IOTCS_RESULT_OK) {
		fprintf(stderr,"iotcs_virtual_device_set_float 2 failed\n");
		return IOTCS_RESULT_FAIL;
	}

	// We are done. IOT can sync the virtual device
	iotcs_virtual_device_finish_update(device_handle);

  /* free device handle */
  iotcs_free_virtual_device_handle(device_handle);
  /* free device model handle */
  iotcs_free_device_model_handle(device_model_handle);

  /*
   * Calling finalization of the library ensures communications channels are closed,
   * previously allocated temporary resources are released.
   */
  iotcs_finalize();

  return EXIT_SUCCESS;
}
