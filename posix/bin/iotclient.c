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
*
* Add your code here!
*
*/

  return EXIT_SUCCESS;
}
